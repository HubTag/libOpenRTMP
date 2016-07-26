/*
    rtmp_stream.c

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

#include <stdlib.h>
#include <string.h>
#include "rtmp/rtmp_stream.h"


typedef struct rtmp_amf_cb{
    char* name;
    rtmp_message_type_t type;
    rtmp_stream_amf_proc callback;
    void *user;
} rtmp_amf_cb_t;

typedef struct rtmp_msg_cb{
    rtmp_message_type_t type;
    rtmp_stream_msg_proc callback;
    void *user;
} rtmp_msg_cb_t;

typedef struct rtmp_usr_cb{
    rtmp_usr_evt_t type;
    rtmp_stream_usr_proc callback;
    void *user;
} rtmp_usr_cb_t;

struct rtmp_stream{
    rtmp_chunk_conn_t connection;
    rtmp_chunk_assembler_t assembler;

    size_t chunk_id, message_id;
    void *scratch;
    size_t scratch_len;

    rtmp_amf_cb_t *amf_callbacks;
    size_t amf_callbacks_len;

    rtmp_msg_cb_t *msg_callbacks;
    size_t msg_callbacks_len;

    rtmp_usr_cb_t *usr_callbacks;
    size_t usr_callbacks_len;

    rtmp_event_proc event_cb;
    void *event_cb_data;

    rtmp_log_proc log_cb;
    void *log_cb_data;
};

static rtmp_cb_status_t rtmp_stream_call_msg(
    rtmp_stream_t stream,
    rtmp_message_type_t message,
    const byte *data,
    size_t length,
    size_t remaining
){
    rtmp_cb_status_t ret = RTMP_CB_CONTINUE;

    for( size_t i = 0; i < stream->msg_callbacks_len; ++i ){
        rtmp_msg_cb_t *cb = &stream->msg_callbacks[i];
        if( cb->callback && ( cb->type == RTMP_ANY || cb->type == message ) ){
            ret = cb->callback( stream, message, data, length, remaining, cb->user );
            if( ret != RTMP_CB_CONTINUE ){
                break;
            }
        }
    }
    return ret;
}

rtmp_cb_status_t rtmp_stream_call_amf(
    rtmp_stream_t stream,
    rtmp_message_type_t message,
    amf_t object
){
    rtmp_cb_status_t ret = RTMP_CB_CONTINUE;

    char* name = nullptr;
    size_t len;

    for( size_t i = 0; i < stream->amf_callbacks_len; ++i ){
        rtmp_amf_cb_t *cb = &stream->amf_callbacks[i];
        if( cb->callback && ( cb->type == RTMP_ANY || cb->type == message ) ){

            if( amf_get_count( object ) > 0 ){
                amf_value_t item = amf_get_item( object, 0 );
                if( amf_value_is_like( item, AMF0_TYPE_STRING ) ){
                    name = amf_value_get_string( item, &len );
                }
            }

            if( cb->name == nullptr || strncmp( cb->name, name, len ) ){
                ret = cb->callback( stream, message, object, cb->user );
                if( ret != RTMP_CB_CONTINUE ){
                    break;
                }
            }
        }
    }
    return ret;
}


rtmp_cb_status_t rtmp_stream_call_usr(
    rtmp_stream_t stream,
    rtmp_usr_evt_t message,
    uint32_t param1,
    uint32_t param2
){
    rtmp_cb_status_t ret = RTMP_CB_CONTINUE;

    for( size_t i = 0; i < stream->usr_callbacks_len; ++i ){
        rtmp_usr_cb_t *cb = &stream->usr_callbacks[i];
        if( cb->callback && ( cb->type == RTMP_ANY || cb->type == message ) ){
            ret = cb->callback( stream, message, param1, param2, cb->user );
            if( ret != RTMP_CB_CONTINUE ){
                break;
            }
        }
    }
    return ret;
}



//Procedure prototype for handling partial chunk information
rtmp_cb_status_t rtmp_stream_chunk_proc(
    rtmp_chunk_conn_t conn,
    const byte *contents,
    size_t available,
    size_t remaining,
    rtmp_chunk_stream_message_t *msg,
    void *user
){
    if( remaining != 0 ){
        return RTMP_CB_ABORT;
    }
    rtmp_stream_t self = (rtmp_stream_t) user;
    amf_t amf;
    rtmp_cb_status_t ret = RTMP_CB_CONTINUE;
    size_t amount = 0;
    uint32_t param1 = 0;
    uint32_t param2 = 0;
    int amf_ver = -1;

    ret = rtmp_stream_call_msg( self, msg->message_type, contents, available, remaining );
    if( ret != RTMP_CB_CONTINUE ){
        return ret;
    }

    switch( msg->message_type ){
        case RTMP_MSG_AMF0_CMD:
        case RTMP_MSG_AMF0_DAT:
        case RTMP_MSG_AMF0_SO:
            amf_ver = 0;
        case RTMP_MSG_AMF3_CMD:
        case RTMP_MSG_AMF3_DAT:
        case RTMP_MSG_AMF3_SO:
            if( amf_ver == -1 ){
                amf_ver = 3;
            }
            amf = amf_create( amf_ver );
            if( !amf ){
                return RTMP_CB_ERROR;
            }
            if( amf_read( amf, contents, available, &amount ) != AMF_ERR_NONE ){
                amf_destroy( amf );
                return RTMP_CB_ERROR;
            }
            ret = rtmp_stream_call_amf( self, msg->message_type, amf );
            amf_destroy( amf );
            break;
        case RTMP_MSG_USER_CTL:
            if( available >= 4 ){
                param1 = ntoh_read_ud( contents );
            }
            if( available >= 8 ){
                param2 = ntoh_read_ud( contents + 4 );
            }
            ret = rtmp_stream_call_usr( self, msg->message_type, param1, param2 );
            break;
    }

    return ret;
}

//Procedure prototype for handling chunk stream events
rtmp_cb_status_t rtmp_stream_event_proc(
    rtmp_chunk_conn_t conn,
    rtmp_event_t event,
    void *user
){
    rtmp_stream_t self = (rtmp_stream_t) user;
    if( self->event_cb ){
        return self->event_cb( conn, event, self->event_cb_data );
    }
    return RTMP_CB_CONTINUE;
}

//Procedure prototype for handling logging
void rtmp_stream_log_proc(
    rtmp_err_t err,
    size_t line,
    const char* file,
    const char* message,
    void *user
){
    rtmp_stream_t self = (rtmp_stream_t) user;
    if( self->log_cb ){
        self->log_cb( err, line, file, message, self->log_cb_data );
    }
}


rtmp_stream_t rtmp_stream_create( bool client ){
    rtmp_stream_t stream = calloc( 1, sizeof( struct rtmp_stream ) );
    stream->connection = rtmp_chunk_conn_create( client );
    stream->assembler = rtmp_chunk_assembler_create( RTMP_MAX_CHUNK_CACHE, rtmp_stream_chunk_proc, rtmp_stream_event_proc, rtmp_stream_log_proc, stream );
    rtmp_chunk_assembler_assign( stream->assembler, stream->connection );
    return stream;
}

void rtmp_stream_destroy( rtmp_stream_t stream ){
    rtmp_chunk_conn_close( stream->connection );
    rtmp_chunk_assembler_destroy( stream->assembler );
    for( size_t i = 0; i < stream->amf_callbacks_len; ++i ){
        free( stream->amf_callbacks[i].name );
    }
    free( stream->amf_callbacks );
    free( stream->msg_callbacks );
    free( stream->usr_callbacks );
    free( stream->scratch );
}

rtmp_err_t rtmp_stream_reg_amf( rtmp_stream_t stream, rtmp_message_type_t type, const char *name, rtmp_stream_amf_proc proc, void *user ){
    if( !proc ){
        return AMF_ERR_INVALID_DATA;
    }
    rtmp_amf_cb_t *ptr = realloc( stream->amf_callbacks, sizeof(rtmp_amf_cb_t) * (stream->amf_callbacks_len + 1) );
    if( !ptr ){
        return AMF_ERR_OOM;
    }
    stream->amf_callbacks = ptr;

    rtmp_amf_cb_t *value = &ptr[stream->amf_callbacks_len];
    value->callback = proc;
    value->name = nullptr;
    value->type = type;
    value->user = user;

    if( name != nullptr ){
        size_t len = strlen( name );
        value->name = malloc( sizeof( char ) * len );
        if( !value->name ){
            //Since the length hasn't been incremented, we can safely abort without completing initialization.
            return AMF_ERR_OOM;
        }
        strcpy( value->name, name );
    }

    stream->amf_callbacks_len ++;
    return AMF_ERR_NONE;
}

rtmp_err_t rtmp_stream_reg_msg( rtmp_stream_t stream, rtmp_message_type_t type, rtmp_stream_msg_proc proc, void *user ){
    if( !proc ){
        return AMF_ERR_INVALID_DATA;
    }
    rtmp_msg_cb_t *ptr = realloc( stream->msg_callbacks, sizeof( rtmp_msg_cb_t ) * (stream->msg_callbacks_len + 1) );
    if( !ptr ){
        return AMF_ERR_OOM;
    }
    stream->msg_callbacks = ptr;

    rtmp_msg_cb_t *value = &ptr[stream->msg_callbacks_len];
    value->callback = proc;
    value->type = type;
    value->user = user;

    stream->msg_callbacks_len ++;
    return AMF_ERR_NONE;
}

rtmp_err_t rtmp_stream_reg_usr( rtmp_stream_t stream, rtmp_usr_evt_t type, rtmp_stream_usr_proc proc, void *user ){
    if( !proc ){
        return AMF_ERR_INVALID_DATA;
    }
    rtmp_usr_cb_t *ptr = realloc( stream->usr_callbacks, sizeof( rtmp_usr_cb_t ) * (stream->usr_callbacks_len + 1) );
    if( !ptr ){
        return AMF_ERR_OOM;
    }
    stream->usr_callbacks = ptr;

    rtmp_usr_cb_t *value = &ptr[stream->usr_callbacks_len];
    value->callback = proc;
    value->type = type;
    value->user = user;

    stream->usr_callbacks_len ++;
    return AMF_ERR_NONE;
}

rtmp_err_t rtmp_stream_reg_event( rtmp_stream_t stream, rtmp_event_proc proc, void *user ){
    stream->event_cb = proc;
    stream->event_cb_data = user;
    return RTMP_ERR_NONE;
}

rtmp_err_t rtmp_stream_reg_log( rtmp_stream_t stream, rtmp_log_proc proc, void *user ){
    stream->log_cb = proc;
    stream->log_cb_data = user;
    return RTMP_ERR_NONE;
}

rtmp_chunk_conn_t rtmp_stream_get_conn( rtmp_stream_t stream ){
    return stream->connection;
}


void rtmp_stream_set_chunk_stream( rtmp_stream_t stream, size_t chunk_id ){
    stream->chunk_id = chunk_id;
}

void rtmp_stream_set_msg_stream( rtmp_stream_t stream, size_t msg_id ){
    stream->message_id = msg_id;
}

rtmp_err_t rtmp_stream_send_audio( rtmp_stream_t stream, rtmp_time_t timestamp, const byte *data, size_t len, size_t *written ){
    return rtmp_stream_send_audio2( stream, stream->chunk_id, stream->message_id, timestamp, data, len, written );
}

rtmp_err_t rtmp_stream_send_video( rtmp_stream_t stream, rtmp_time_t timestamp, const byte *data, size_t len, size_t *written ){
    return rtmp_stream_send_video2( stream, stream->chunk_id, stream->message_id, timestamp, data, len, written );
}

rtmp_err_t rtmp_stream_send_cmd( rtmp_stream_t stream, rtmp_time_t timestamp, amf_t amf, size_t *written  ){
    return rtmp_stream_send_cmd2( stream, stream->chunk_id, stream->message_id, timestamp, amf, written );
}

rtmp_err_t rtmp_stream_send_so( rtmp_stream_t stream, rtmp_time_t timestamp, amf_t amf, size_t *written  ){
    return rtmp_stream_send_so2( stream, stream->chunk_id, stream->message_id, timestamp, amf, written );
}

rtmp_err_t rtmp_stream_send_dat( rtmp_stream_t stream, rtmp_time_t timestamp, amf_t amf, size_t *written  ){
    return rtmp_stream_send_dat2( stream, stream->chunk_id, stream->message_id, timestamp, amf, written );
}



static rtmp_err_t rtmp_stream_prepare_amf( rtmp_stream_t stream, amf_t amf ){
    size_t len;
    if( amf_write( amf, nullptr, 0, &len ) != AMF_ERR_NONE ){
        return RTMP_ERR_BAD_WRITE;
    }
    free( stream->scratch );
    stream->scratch = malloc( len * sizeof(byte) );
    stream->scratch_len = len;
    if( !stream->scratch ){
        return RTMP_ERR_OOM;
    }
    if( amf_write( amf, stream->scratch, len, &len ) != AMF_ERR_NONE ){
        return RTMP_ERR_BAD_WRITE;
    }
    return RTMP_ERR_NONE;
}

static rtmp_err_t rtmp_stream_send_amf(
    rtmp_stream_t stream,
    rtmp_message_type_t msg,
    size_t chunk_id, size_t msg_id,
    rtmp_time_t timestamp,
    amf_t amf,
    size_t *written
){
    rtmp_err_t ret = RTMP_ERR_NONE;
    if( !written || *written == 0 ){
        ret = rtmp_stream_prepare_amf( stream, amf );
    }
    if( ret == RTMP_ERR_NONE ){
        ret = rtmp_chunk_conn_send_message( stream->connection, msg, chunk_id, msg_id, timestamp, stream->scratch, stream->scratch_len, written );
    }
    return ret;
}

rtmp_err_t rtmp_stream_send_audio2( rtmp_stream_t stream, size_t chunk_id, size_t msg_id, rtmp_time_t timestamp, const byte *data, size_t len, size_t *written ){
    return rtmp_chunk_conn_send_message(
            stream->connection,
            RTMP_MSG_AUDIO,
            chunk_id,
            msg_id,
            timestamp,
            data,
            len,
            written );
}
rtmp_err_t rtmp_stream_send_video2( rtmp_stream_t stream, size_t chunk_id, size_t msg_id, rtmp_time_t timestamp, const byte *data, size_t len, size_t *written ){
    return rtmp_chunk_conn_send_message(
            stream->connection,
            RTMP_MSG_VIDEO,
            chunk_id,
            msg_id,
            timestamp,
            data,
            len,
            written );
}
rtmp_err_t rtmp_stream_send_cmd2( rtmp_stream_t stream, size_t chunk_id, size_t msg_id, rtmp_time_t timestamp, amf_t amf, size_t *written ){
    return rtmp_stream_send_amf(
            stream,
            RTMP_MSG_AMF0_CMD,
            chunk_id,
            msg_id,
            timestamp,
            amf,
            written );
}
rtmp_err_t rtmp_stream_send_so2( rtmp_stream_t stream, size_t chunk_id, size_t msg_id, rtmp_time_t timestamp, amf_t amf, size_t *written ){
    return rtmp_stream_send_amf(
            stream,
            RTMP_MSG_AMF0_SO,
            chunk_id,
            msg_id,
            timestamp,
            amf,
            written );
}
rtmp_err_t rtmp_stream_send_dat2( rtmp_stream_t stream, size_t chunk_id, size_t msg_id, rtmp_time_t timestamp, amf_t amf, size_t *written ){
    return rtmp_stream_send_amf(
            stream,
            RTMP_MSG_AMF0_DAT,
            chunk_id,
            msg_id,
            timestamp,
            amf,
            written );
}

rtmp_err_t rtmp_stream_send_stream_begin( rtmp_stream_t stream, uint32_t stream_id ){
    byte buffer[6];
    ntoh_write_s( buffer, RTMP_USR_EVT_STREAM_BEGIN );
    ntoh_write_ud( buffer + 2, stream_id );
    return rtmp_chunk_conn_send_message(
            stream->connection,
            RTMP_MSG_USER_CTL,
            RTMP_CONTROL_CHUNK_STREAM,
            RTMP_CONTROL_MSG_STREAM,
            0,
            buffer,
            sizeof( buffer ),
            nullptr );
}

rtmp_err_t rtmp_stream_send_stream_eof( rtmp_stream_t stream, uint32_t stream_id ){
    byte buffer[6];
    ntoh_write_s( buffer, RTMP_USR_EVT_STREAM_EOF );
    ntoh_write_ud( buffer + 2, stream_id );
    return rtmp_chunk_conn_send_message(
            stream->connection,
            RTMP_MSG_USER_CTL,
            RTMP_CONTROL_CHUNK_STREAM,
            RTMP_CONTROL_MSG_STREAM,
            0,
            buffer,
            sizeof( buffer ),
            nullptr );
}

rtmp_err_t rtmp_stream_send_stream_dry( rtmp_stream_t stream, uint32_t stream_id ){
    byte buffer[6];
    ntoh_write_s( buffer, RTMP_USR_EVT_STREAM_DRY );
    ntoh_write_ud( buffer + 2, stream_id );
    return rtmp_chunk_conn_send_message(
            stream->connection,
            RTMP_MSG_USER_CTL,
            RTMP_CONTROL_CHUNK_STREAM,
            RTMP_CONTROL_MSG_STREAM,
            0,
            buffer,
            sizeof( buffer ),
            nullptr );
}

rtmp_err_t rtmp_stream_send_set_buf_len( rtmp_stream_t stream, uint32_t stream_id, uint32_t len_ms ){
    byte buffer[10];
    ntoh_write_s( buffer, RTMP_USR_EVT_SET_BUF_LEN );
    ntoh_write_ud( buffer + 2, stream_id );
    ntoh_write_ud( buffer + 6, len_ms );
    return rtmp_chunk_conn_send_message(
            stream->connection,
            RTMP_MSG_USER_CTL,
            RTMP_CONTROL_CHUNK_STREAM,
            RTMP_CONTROL_MSG_STREAM,
            0,
            buffer,
            sizeof( buffer ),
            nullptr );
}

rtmp_err_t rtmp_stream_send_is_recorded( rtmp_stream_t stream, uint32_t stream_id ){
    byte buffer[6];
    ntoh_write_s( buffer, RTMP_USR_EVT_STREAM_IS_REC );
    ntoh_write_ud( buffer + 2, stream_id );
    return rtmp_chunk_conn_send_message(
            stream->connection,
            RTMP_MSG_USER_CTL,
            RTMP_CONTROL_CHUNK_STREAM,
            RTMP_CONTROL_MSG_STREAM,
            0,
            buffer,
            sizeof( buffer ),
            nullptr );
}

rtmp_err_t rtmp_stream_send_ping_req( rtmp_stream_t stream, uint32_t ping_time ){
    byte buffer[6];
    ntoh_write_s( buffer, RTMP_USR_EVT_PING_REQ );
    ntoh_write_ud( buffer + 2, ping_time );
    return rtmp_chunk_conn_send_message(
            stream->connection,
            RTMP_MSG_USER_CTL,
            RTMP_CONTROL_CHUNK_STREAM,
            RTMP_CONTROL_MSG_STREAM,
            0,
            buffer,
            sizeof( buffer ),
            nullptr );
}

rtmp_err_t rtmp_stream_send_ping_res( rtmp_stream_t stream, uint32_t ping_time ){
    byte buffer[6];
    ntoh_write_s( buffer, RTMP_USR_EVT_PING_RES );
    ntoh_write_ud( buffer + 2, ping_time );
    return rtmp_chunk_conn_send_message(
            stream->connection,
            RTMP_MSG_USER_CTL,
            RTMP_CONTROL_CHUNK_STREAM,
            RTMP_CONTROL_MSG_STREAM,
            0,
            buffer,
            sizeof( buffer ),
            nullptr );
}
