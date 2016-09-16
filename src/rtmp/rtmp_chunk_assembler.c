/*
    rtmp_chunk_assembler.c

    Copyright (C) 2016 Hubtag LLC.

    ----------------------------------------

    This file is part of libOpenRTMP.

    libOpenRTMP is free software: you can redistribute it and/or modify
    it under the terms of version 3 of the GNU Affero General Public License
    as published by the Free Software Foundation.

    libOpenRTMP is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with libOpenRTMP. If not, see <http://www.gnu.org/licenses/>.

*/

#include <openrtmp/rtmp/rtmp_chunk_assembler.h>
#include <openrtmp/util/vec.h>
#include <stdlib.h>

typedef struct rtmp_asm_buf{
    size_t chunk_id, msg_id;
    ringbuffer_t buffer;
} rtmp_asm_buf_t;

struct rtmp_chunk_assembler{
    rtmp_chunk_proc chunk_cb;
    rtmp_event_proc event_cb;
    rtmp_log_proc log_cb;
    size_t max_size;
    void *user;
    VEC_DECLARE(rtmp_asm_buf_t) buffers;
};

void rtmp_asm_buf_destroy(rtmp_asm_buf_t buffer){
    ringbuffer_destroy( buffer.buffer );
}

rtmp_cb_status_t rtmp_chunk_assembler_event_thunk(
    rtmp_chunk_conn_t conn,
    rtmp_event_t event,
    void * restrict user
){
    return RTMP_CB_CONTINUE;
}

void rtmp_chunk_assembler_log_thunk(
    rtmp_err_t err,
    size_t line,
    const char* restrict file,
    const char* restrict message,
    void * restrict user
){

}

rtmp_cb_status_t    rtmp_chunk_assembler_thunk(
        rtmp_chunk_conn_t conn,
        const byte * restrict contents,
        size_t available,
        size_t remaining,
        const rtmp_chunk_stream_message_t *msg,
        void * restrict user
){
    return RTMP_CB_CONTINUE;
}


rtmp_cb_status_t rtmp_chunk_assembler_event_cb(
    rtmp_chunk_conn_t conn,
    rtmp_event_t event,
    void * restrict user
){
    rtmp_chunk_assembler_t self = user;
    return self->event_cb(conn, event, self->user);
}

void rtmp_chunk_assembler_log_cb(
    rtmp_err_t err,
    size_t line,
    const char* restrict file,
    const char* restrict message,
    void * restrict user
){
    rtmp_chunk_assembler_t self = user;
    self->log_cb(err, line, file, message, self->user);
}


static rtmp_asm_buf_t * rtmp_chunk_assembler_get_buffer( rtmp_chunk_assembler_t self, size_t chunk_id, size_t msg_id ){
    size_t free_idx = 0;
    for( size_t i = 0; i < VEC_SIZE(self->buffers); ++i ){
        if( self->buffers[i].chunk_id == chunk_id && self->buffers[i].msg_id == msg_id ){
            return self->buffers + i;
        }
        if( free_idx == 0 && self->buffers[i].chunk_id == 0 ){
            free_idx = i;
        }
    }
    if( free_idx != 0 ){
        ringbuffer_clear( self->buffers[free_idx].buffer );
        self->buffers[free_idx].chunk_id = chunk_id;
        self->buffers[free_idx].msg_id = msg_id;
        return self->buffers + free_idx;
    }
    if( VEC_SIZE( self->buffers ) > RTMP_MAX_ASM_HARD_BUFFER ){
        return nullptr;
    }
    rtmp_asm_buf_t * item = VEC_PUSH(self->buffers);
    if( !item ){
        return item;
    }
    item->chunk_id = chunk_id;
    item->msg_id = msg_id;
    item->buffer = ringbuffer_create( self->max_size );
    if( !item->buffer ){
        VEC_POP( self->buffers );
        return nullptr;
    }
    return item;
}

static void rtmp_chunk_assembler_rm_buffer( rtmp_chunk_assembler_t self, rtmp_asm_buf_t * buffer ){
    if( VEC_SIZE(self->buffers) > RTMP_MAX_ASM_SOFT_BUFFER ){
        size_t idx = buffer - self->buffers;
        ringbuffer_destroy( buffer->buffer );
        VEC_ERASE( self->buffers, idx );
    }
    else{
        ringbuffer_clear( buffer->buffer );
        buffer->chunk_id = 0;
    }
}

rtmp_cb_status_t    rtmp_chunk_assembler_cb(
        rtmp_chunk_conn_t conn,
        const byte * restrict contents,
        size_t available,
        size_t remaining,
        const rtmp_chunk_stream_message_t *msg,
        void * restrict user
){
    rtmp_chunk_assembler_t self = user;
    rtmp_asm_buf_t * buffer = rtmp_chunk_assembler_get_buffer( self, msg->chunk_stream_id, msg->message_stream_id );
    if( !buffer ){
        return RTMP_CB_ABORT;
    }
    //printf("chunk %d\tmessage %d\n", buffer->chunk_id, buffer->msg_id);
    size_t original_size = ringbuffer_count( buffer->buffer );
    size_t copied = ringbuffer_copy_write( buffer->buffer, contents, available );
    if( copied < available || remaining == 0 ){
        if( available == 0 ){
            //Chunk was aborted.
            //printf("DONE chunk %d\tmessage %d\n", buffer->chunk_id, buffer->msg_id);
            rtmp_chunk_assembler_rm_buffer( self, buffer );
            return self->chunk_cb( conn, nullptr, 0, 0, msg, self->user );
        }
        //If the chunk is done, or if our buffer is full, run the callback
        unsigned long len;
        const void* data = ringbuffer_get_read_buf( buffer->buffer, &len );
        original_size += copied;
        original_size -= len;
        rtmp_cb_status_t ret = self->chunk_cb( conn, data, len, original_size + remaining + available - copied, msg, self->user );
        if( ret == RTMP_CB_CONTINUE && len < original_size + copied ){
            //The buffer wasn't exhausted
            ringbuffer_commit_read( buffer->buffer, len );
            data = ringbuffer_get_read_buf( buffer->buffer, &len );
            original_size -= len;
            ret = self->chunk_cb( conn, data, len, remaining + available - copied, msg, self->user );
        }
        ringbuffer_clear( buffer->buffer );
        if( ret != RTMP_CB_CONTINUE ){
            //printf("DONE chunk %d\tmessage %d\n", buffer->chunk_id, buffer->msg_id);
            rtmp_chunk_assembler_rm_buffer( self, buffer );
            return ret;
        }
        if( available > copied ){
            //There's still data which needs to be consumed by this callback
            ret = self->chunk_cb( conn, contents + copied, available - copied, remaining, msg, self->user );
        }
        if( remaining == 0 ){
            //We're done with this buffer.
            //printf("DONE chunk %d\tmessage %d\n", buffer->chunk_id, buffer->msg_id);
            rtmp_chunk_assembler_rm_buffer( self, buffer );
        }
        return ret;
    }
    return RTMP_CB_CONTINUE;
}

rtmp_chunk_assembler_t rtmp_chunk_assembler_create( size_t max_size, rtmp_chunk_proc chunk_cb, rtmp_event_proc event_cb, rtmp_log_proc log_cb, void *user ){
    rtmp_chunk_assembler_t ret = malloc( sizeof( struct rtmp_chunk_assembler ) );
    ret->chunk_cb = chunk_cb ? chunk_cb : rtmp_chunk_assembler_thunk;
    ret->event_cb = event_cb ? event_cb : rtmp_chunk_assembler_event_thunk;
    ret->log_cb = log_cb ? log_cb : rtmp_chunk_assembler_log_thunk;
    ret->user = user;
    ret->max_size = max_size;
    VEC_INIT(ret->buffers);
    VEC_RESERVE(ret->buffers, RTMP_MAX_ASM_SOFT_BUFFER);
    return ret;
}

void rtmp_chunk_assembler_destroy( rtmp_chunk_assembler_t assembler ){
    VEC_DESTROY_DTOR( assembler->buffers, rtmp_asm_buf_destroy );
    free( assembler );
}

void rtmp_chunk_assembler_assign( rtmp_chunk_assembler_t assembler, rtmp_chunk_conn_t connection ){
    rtmp_chunk_conn_register_callbacks( connection, rtmp_chunk_assembler_cb, rtmp_chunk_assembler_event_cb, rtmp_chunk_assembler_log_cb, assembler );
}

