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
#include "rtmp.h"
#include "rtmp/rtmp_private.h"


static rtmp_cb_status_t rtmp_stream_call_msg(
    rtmp_stream_t stream,
    rtmp_message_type_t message,
    const byte *data,
    size_t length,
    size_t remaining
){
    rtmp_cb_status_t ret = RTMP_CB_CONTINUE;

    for( size_t i = 0; i < VEC_SIZE(stream->msg_callback); ++i ){
        rtmp_msg_cb_t *cb = &stream->msg_callback[i];
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
    amf_print( object );
    rtmp_cb_status_t ret = RTMP_CB_CONTINUE;

    const char* name = nullptr;
    size_t len;

    if( amf_get_count( object ) > 0 ){
        amf_value_t item = amf_get_item( object, 0 );
        if( amf_value_is_like( item, AMF0_TYPE_STRING ) ){
            name = amf_value_get_string( item, &len );
        }
    }

    for( size_t i = 0; i < VEC_SIZE(stream->amf_callback); ++i ){
        rtmp_amf_cb_t *cb = &stream->amf_callback[i];
        if( cb->callback && ( cb->type == RTMP_ANY || cb->type == message ) ){
            if( cb->name == nullptr || strncmp( cb->name, name, len ) == 0 ){
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

    for( size_t i = 0; i < VEC_SIZE(stream->usr_callback); ++i ){
        rtmp_usr_cb_t *cb = &stream->usr_callback[i];
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
    const byte * restrict contents,
    size_t available,
    size_t remaining,
    const rtmp_chunk_stream_message_t *msg,
    void * restrict user
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
            if( amf_read( amf, contents, available, &amount ) < 0 ){
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
    void * restrict user
){
    rtmp_stream_t self = (rtmp_stream_t) user;
    rtmp_cb_status_t ret = RTMP_CB_CONTINUE;
    for( int i = 0; i < VEC_SIZE( self->event_callback ); ++i ){
        if( self->event_callback[i].type == RTMP_ANY || self->event_callback[i].type == event ){
            ret = self->event_callback[i].callback( self, event, self->event_callback[i].user );
        }
        if( ret != RTMP_CB_CONTINUE ){
            break;
        }
    }
    return ret;
}

//Procedure prototype for handling logging
void rtmp_stream_log_proc(
    rtmp_err_t err,
    size_t line,
    const char * restrict file,
    const char * restrict message,
    void * restrict user
){
    rtmp_stream_t self = (rtmp_stream_t) user;
    for( int i = 0; i < VEC_SIZE( self->log_callback ); ++i ){
        self->log_callback[i].callback( err, line, file, message, self->log_callback[i].user );
    }
}


rtmp_stream_t rtmp_stream_create( bool client ){
    rtmp_stream_t stream = malloc( sizeof( struct rtmp_stream ) );
    rtmp_stream_create_at( stream, client );
    return stream;
}

void rtmp_stream_create_at( rtmp_stream_t location, bool client ){
    memset( location, 0, sizeof( struct rtmp_stream ) );
    location->connection = rtmp_chunk_conn_create( client );
    location->assembler = rtmp_chunk_assembler_create( RTMP_MAX_CHUNK_CACHE, rtmp_stream_chunk_proc, rtmp_stream_event_proc, rtmp_stream_log_proc, location );
    rtmp_chunk_assembler_assign( location->assembler, location->connection );
    VEC_INIT( location->event_callback );
    VEC_INIT( location->amf_callback );
    VEC_INIT( location->log_callback );
    VEC_INIT( location->msg_callback );
    VEC_INIT( location->usr_callback );
}

void rtmp_stream_destroy( rtmp_stream_t stream ){
    rtmp_stream_destroy_at( stream );
    free( stream );
}

void rtmp_stream_destroy_at( rtmp_stream_t stream ){
    rtmp_chunk_conn_close( stream->connection );
    rtmp_chunk_assembler_destroy( stream->assembler );
    for( size_t i = 0; i < VEC_SIZE(stream->amf_callback); ++i ){
        free( stream->amf_callback[i].name );
    }
    VEC_DESTROY( stream->event_callback );
    VEC_DESTROY( stream->amf_callback );
    VEC_DESTROY( stream->log_callback );
    VEC_DESTROY( stream->msg_callback );
    VEC_DESTROY( stream->usr_callback );
    free( stream->scratch );
}

rtmp_err_t rtmp_stream_reg_amf( rtmp_stream_t stream, rtmp_message_type_t type, const char *name, rtmp_stream_amf_proc proc, void *user ){
    if( !proc ){
        return AMF_ERR_INVALID_DATA;
    }

    rtmp_amf_cb_t *value = VEC_PUSH( stream->amf_callback );
    if(!value){
        return RTMP_ERR_OOM;
    }

    value->callback = proc;
    value->name = nullptr;
    value->type = type;
    value->user = user;

    if( name != nullptr ){
        size_t len = strlen( name );
        value->name = malloc( sizeof( char ) * len + 1 );
        if( !value->name ){
            //Since the length hasn't been incremented, we can safely abort without completing initialization.
            return AMF_ERR_OOM;
        }
        strcpy( value->name, name );
    }

    return AMF_ERR_NONE;
}

rtmp_err_t rtmp_stream_reg_msg( rtmp_stream_t stream, rtmp_message_type_t type, rtmp_stream_msg_proc proc, void *user ){
    if( !proc ){
        return AMF_ERR_INVALID_DATA;
    }

    rtmp_msg_cb_t *value = VEC_PUSH( stream->msg_callback );
    if(!value){
        return RTMP_ERR_OOM;
    }
    value->callback = proc;
    value->type = type;
    value->user = user;

    return AMF_ERR_NONE;
}

rtmp_err_t rtmp_stream_reg_usr( rtmp_stream_t stream, rtmp_usr_evt_t type, rtmp_stream_usr_proc proc, void *user ){
    if( !proc ){
        return AMF_ERR_INVALID_DATA;
    }

    rtmp_usr_cb_t *value = VEC_PUSH( stream->usr_callback );
    if(!value){
        return RTMP_ERR_OOM;
    }

    value->callback = proc;
    value->type = type;
    value->user = user;

    return AMF_ERR_NONE;
}

rtmp_err_t rtmp_stream_reg_event( rtmp_stream_t stream, rtmp_event_t type, rtmp_stream_evt_proc proc, void *user ){
    if( !proc ){
        return AMF_ERR_INVALID_DATA;
    }

    rtmp_evt_cb_t *value = VEC_PUSH( stream->event_callback );
    if(!value){
        return RTMP_ERR_OOM;
    }

    value->callback = proc;
    value->type = type;
    value->user = user;

    return AMF_ERR_NONE;
}

rtmp_err_t rtmp_stream_reg_log( rtmp_stream_t stream, rtmp_log_proc proc, void *user ){
    if( !proc ){
        return AMF_ERR_INVALID_DATA;
    }

    rtmp_log_cb_t *value = VEC_PUSH( stream->log_callback );
    if(!value){
        return RTMP_ERR_OOM;
    }

    value->callback = proc;
    value->user = user;

    return AMF_ERR_NONE;
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

rtmp_err_t rtmp_stream_send_audio( rtmp_stream_t stream, rtmp_time_t timestamp, const byte * restrict data, size_t len, size_t *written ){
    return rtmp_stream_send_audio2( stream, stream->chunk_id, stream->message_id, timestamp, data, len, written );
}

rtmp_err_t rtmp_stream_send_video( rtmp_stream_t stream, rtmp_time_t timestamp, const byte * restrict data, size_t len, size_t *written ){
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
    size_t len = amf_write( amf, nullptr, 0, nullptr );
    if( len < 0 ){
        return RTMP_ERR_BAD_WRITE;
    }
    free( stream->scratch );
    stream->scratch = malloc( len * sizeof(byte) );
    stream->scratch_len = len;
    if( !stream->scratch ){
        return RTMP_ERR_OOM;
    }
    if( amf_write( amf, stream->scratch, len, &len ) < 0 ){
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

rtmp_err_t rtmp_stream_send_audio2( rtmp_stream_t stream, size_t chunk_id, size_t msg_id, rtmp_time_t timestamp, const byte * restrict data, size_t len, size_t *written ){
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
rtmp_err_t rtmp_stream_send_video2( rtmp_stream_t stream, size_t chunk_id, size_t msg_id, rtmp_time_t timestamp, const byte * restrict data, size_t len, size_t *written ){
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



rtmp_err_t rtmp_stream_call( rtmp_stream_t stream, const char *name, double id, ... ){
    va_list list;
    va_start( list, id );
    rtmp_err_t ret = rtmp_stream_call2_va( stream, 3, 0, name, id, list );
    va_end( list );
    return ret;
}

rtmp_err_t rtmp_stream_respond( rtmp_stream_t stream, const char *name, double id, ... ){
    va_list list;
    va_start( list, id );
    rtmp_err_t ret = rtmp_stream_call2_va( stream, 3, 0, name, id, list );
    va_end( list );
    return ret;
}

rtmp_err_t rtmp_stream_call2( rtmp_stream_t stream, size_t chunk_id, size_t msg_id, const char *name, double id, ... ){
    va_list list;
    va_start( list, id );
    rtmp_err_t ret = rtmp_stream_call2_va( stream, chunk_id, msg_id, name, id, list );
    va_end( list );
    return ret;
}

rtmp_err_t rtmp_stream_respond2( rtmp_stream_t stream, size_t chunk_id, size_t msg_id, const char *name, double id, ... ){
    va_list list;
    va_start( list, id );
    rtmp_err_t ret = rtmp_stream_call2_va( stream, chunk_id, msg_id, name, id, list );
    va_end( list );
    return ret;
}

rtmp_err_t rtmp_stream_call2_va( rtmp_stream_t stream, size_t chunk_id, size_t msg_id, const char *name, double id, va_list list ){
    amf_t amf = amf_create(0);
    if( !amf ){
        return RTMP_ERR_OOM;
    }
    amf_err_t err = AMF_ERR_NONE;
    err = err ? err : amf_push_string( amf, name );
    err = err ? err : amf_push_number( amf, id );
    err = err ? err : amf_push_simple_list( amf, list );
    err = err ? err : rtmp_stream_send_amf( stream, RTMP_MSG_AMF0_CMD, chunk_id, msg_id, 0, amf, nullptr );
    amf_print(amf);
    amf_destroy( amf );
    return rtmp_amferr( err );
}
