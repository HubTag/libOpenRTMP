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

#include "rtmp/chunk/rtmp_chunk_assembler.h"
#include <stdlib.h>

struct rtmp_chunk_assembler{
    rtmp_chunk_proc chunk_cb;
    rtmp_event_proc event_cb;
    rtmp_log_proc log_cb;
    void *user;
    ringbuffer_t buffer;
};

rtmp_cb_status_t    rtmp_chunk_assembler_cb(
        rtmp_chunk_conn_t conn,
        const byte *contents,
        size_t available,
        size_t remaining,
        rtmp_chunk_stream_message_t *msg,
        void *user
){
    rtmp_chunk_assembler_t self = user;
    size_t original_size = ringbuffer_count( self->buffer );
    size_t copied = ringbuffer_copy_write( self->buffer, contents, available );
    if( copied < available || remaining == 0 ){
        if( available == 0 ){
            //Chunk was aborted.
            ringbuffer_clear( self->buffer );
            return self->chunk_cb( conn, nullptr, 0, 0, msg, self->user );
        }
        //If the chunk is done, or if our buffer is full, run the callback
        unsigned long len;
        const void* buffer = ringbuffer_get_read_buf( self->buffer, &len );
        original_size += copied;
        original_size -= len;
        rtmp_cb_status_t ret = self->chunk_cb( conn, buffer, len, original_size + remaining, msg, self->user );
        if( ret == RTMP_CB_CONTINUE && len < original_size + copied ){
            //The buffer wasn't exhausted
            ringbuffer_commit_read( self->buffer, len );
            buffer = ringbuffer_get_read_buf( self->buffer, &len );
            original_size -= len;
            ret = self->chunk_cb( conn, buffer, len, original_size + remaining, msg, self->user );
        }
        ringbuffer_clear( self->buffer );
        if( ret != RTMP_CB_CONTINUE ){
            return ret;
        }
        if( available > copied ){
            //There's still data which needs to be consumed by this callback
            ret = self->chunk_cb( conn, contents + copied, available - copied, remaining, msg, self->user );
        }
        return ret;
    }
    return RTMP_CB_CONTINUE;
}

rtmp_chunk_assembler_t rtmp_chunk_assembler_create( size_t max_size, rtmp_chunk_proc chunk_cb, rtmp_event_proc event_cb, rtmp_log_proc log_cb, void *user ){
    rtmp_chunk_assembler_t ret = malloc( sizeof( struct rtmp_chunk_assembler ) );
    ret->chunk_cb = chunk_cb;
    ret->event_cb = event_cb;
    ret->log_cb = log_cb;
    ret->user = user;
    ret->buffer = ringbuffer_create( max_size );
    return ret;
}

void rtmp_chunk_assembler_destroy( rtmp_chunk_assembler_t assembler ){
    ringbuffer_destroy( assembler->buffer );
    free( assembler );
}

void rtmp_chunk_assembler_assign( rtmp_chunk_assembler_t assembler, rtmp_chunk_conn_t connection ){
    rtmp_chunk_conn_register_callbacks( connection, rtmp_chunk_assembler_cb, assembler->event_cb, assembler->log_cb, assembler );
}

