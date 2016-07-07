/*
    memutil.c

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

#include "rtmp_chunk_conn.h"
#include "rtmp_debug.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

rtmp_chunk_conn_t rtmp_chunk_conn_create( bool is_client, ors_data_t inflow, ors_data_t outflow ){
    rtmp_chunk_conn_t ret = malloc( sizeof( struct rtmp_chunk_conn ) );

    ret->inflow = inflow;
    ret->outflow = outflow;
    ret->status = RTMP_STATUS_UNINIT | (is_client ? RTMP_STATUS_IS_CLIENT : 0 );
    ret->nonce_c = ret->nonce_s = nullptr;
    ret->self_time = ret->peer_time = ret->peer_shake_recv_time = ret->self_shake_recv_time = 0;
    ret->callback_chunk = nullptr;
    ret->callback_event = nullptr;
    ret->userdata = nullptr;
    ret->control_message_len = 0;
    ret->peer_acknowledged = 0;

    ret->self_chunk_size = RTMP_DEFAULT_CHUNK_SIZE;
    ret->peer_chunk_size = RTMP_DEFAULT_CHUNK_SIZE;
    ret->self_window_size = RTMP_DEFAULT_WINDOW_SIZE;
    ret->peer_window_size = RTMP_DEFAULT_WINDOW_SIZE;
    ret->peer_bandwidth_type = RTMP_DEFAULT_BANDWIDTH_TYPE;

    ret->stream_cache_in = rtmp_cache_create();
    ret->stream_cache_out = rtmp_cache_create();

    return ret;
}

rtmp_err_t rtmp_chunk_conn_close( rtmp_chunk_conn_t conn, bool close_pipes ){
    if( close_pipes ){
        ors_data_close( conn->inflow );
        ors_data_close( conn->outflow );
        ors_data_destroy( conn->inflow );
        ors_data_destroy( conn->outflow );
    }
    rtmp_nonce_del( &conn->nonce_c );
    rtmp_nonce_del( &conn->nonce_s );
    rtmp_cache_destroy( conn->stream_cache_in );
    rtmp_cache_destroy( conn->stream_cache_out );

    free( conn );
    return RTMP_ERR_NONE;
}

bool rtmp_chunk_conn_connected( rtmp_chunk_conn_t conn ){
    return ( conn->status & RTMP_STATUS_SHAKING_DONE ) == RTMP_STATUS_SHAKING_DONE;
}



rtmp_err_t rtmp_chunk_conn_call_event( rtmp_chunk_conn_t conn, rtmp_event_t event ){
    printf("Event \"%s\" Called!\n", rtmp_get_event_name( event ) );
    if( conn->callback_event ){
        switch( conn->callback_event( conn, event, conn->userdata ) ){
            case RTMP_CB_CONTINUE:
                return RTMP_ERR_NONE;
            case RTMP_CB_ERROR:
                return RTMP_ERR_ERROR;
            case RTMP_CB_ABORT:
                return RTMP_ERR_ERROR;
        }
    }
    return RTMP_ERR_NONE;
}

rtmp_err_t rtmp_chunk_conn_call_chunk( rtmp_chunk_conn_t conn, size_t available, size_t remaining, rtmp_chunk_stream_message_t *msg ){
    printf("Chunk Called!\n");
    rtmp_print_message( msg );
    if( conn->callback_chunk ){
        switch( conn->callback_chunk( conn, conn->inflow, available, remaining, msg, conn->userdata ) ){
            case RTMP_CB_CONTINUE:
                return RTMP_ERR_NONE;
            case RTMP_CB_ERROR:
                return RTMP_ERR_ERROR;
            case RTMP_CB_ABORT:
                return RTMP_ERR_ERROR;
        }
    }
    return RTMP_ERR_NONE;
}


static void rtmp_chunk_conn_service_shake_tryfinalize( rtmp_chunk_conn_t conn ){
    if( ( conn->status & RTMP_STATUS_SHAKING_DONE ) == RTMP_STATUS_SHAKING_DONE ){
        rtmp_nonce_del( &conn->nonce_c );
        rtmp_nonce_del( &conn->nonce_s );
        rtmp_chunk_conn_call_event( conn, RTMP_EVENT_CONNECT_SUCCESS );
        conn->lag = 0;
        if( conn->peer_shake_recv_time > conn->self_time ){
            conn->lag = conn->peer_shake_recv_time - conn->self_time;
        }
        else{
            conn->lag = conn->self_time - conn->peer_shake_recv_time;
        }
    }
}

#define FAIL_IF_ERR(a) {rtmp_err_t err; if((err=(a))>=RTMP_ERR_ERROR){ rtmp_chunk_conn_call_event( conn, RTMP_EVENT_CONNECT_FAIL ); return err;}}

static rtmp_err_t rtmp_chunk_conn_service_shake_client_emit( rtmp_chunk_conn_t conn ){
    //Emit C0 if we haven't already
    if( !( conn->status & RTMP_STATUS_SHAKING_C0 ) ){
        FAIL_IF_ERR( rtmp_chunk_emit_shake_0( conn->outflow ) );
        conn->status |= RTMP_STATUS_SHAKING_C0;
    }
    //Emit C1 if we haven't already
    else if( !( conn->status & RTMP_STATUS_SHAKING_C1 ) ){
        FAIL_IF_ERR( rtmp_nonce_gen( &(conn->nonce_c), RTMP_NONCE_SIZE ) );
        conn->self_time = rtmp_get_time();
        FAIL_IF_ERR( rtmp_chunk_emit_shake_1( conn->outflow, conn->self_time, conn->nonce_c, RTMP_NONCE_SIZE ) );
        conn->status |= RTMP_STATUS_SHAKING_C1;
    }
    //Emit C2 if we have S0 and S1, and C2 hasn't yet been emitted.
    else if(!( conn->status & RTMP_STATUS_SHAKING_C2 )   &&
             ( conn->status & RTMP_STATUS_SHAKING_S0 )   &&
             ( conn->status & RTMP_STATUS_SHAKING_S1 )       ){
        FAIL_IF_ERR( rtmp_chunk_emit_shake_2( conn->outflow, conn->peer_time, conn->peer_shake_recv_time, conn->nonce_s, RTMP_NONCE_SIZE ) );
        conn->status |= RTMP_STATUS_SHAKING_C2;
        rtmp_chunk_conn_service_shake_tryfinalize( conn );
    }
    return RTMP_ERR_NONE;
}

static rtmp_err_t rtmp_chunk_conn_service_shake_client_ingest( rtmp_chunk_conn_t conn ){
    //Grab S0 if we haven't already
    if( !( conn->status & RTMP_STATUS_SHAKING_S0 ) ){
        //Server MUST wait until C0 before sending S0.
        if( !( conn->status & RTMP_STATUS_SHAKING_C0 ) ){
            return RTMP_ERR_NOT_READY;
        }
        FAIL_IF_ERR( rtmp_chunk_read_shake_0( conn->inflow ) );
        conn->status |= RTMP_STATUS_SHAKING_S0;
    }
    //Grab S1 if we haven't already
    else if( !( conn->status & RTMP_STATUS_SHAKING_S1 ) ){
        //Server MUST wait until C0 before sending S1.
        if( !( conn->status & RTMP_STATUS_SHAKING_C0 ) ){
            return RTMP_ERR_NOT_READY;
        }
        FAIL_IF_ERR( rtmp_nonce_alloc( &conn->nonce_s, RTMP_NONCE_SIZE ) );
        conn->peer_shake_recv_time = rtmp_get_time();
        FAIL_IF_ERR( rtmp_chunk_read_shake_1( conn->inflow, &conn->peer_time, conn->nonce_s, RTMP_NONCE_SIZE ) );
        conn->status |= RTMP_STATUS_SHAKING_S1;
    }
    //Grab S2 if we have already emitted C0 and C1.
    else if(!( conn->status & RTMP_STATUS_SHAKING_S2 )   &&
             ( conn->status & RTMP_STATUS_SHAKING_C0 )   &&
             ( conn->status & RTMP_STATUS_SHAKING_C1 )       ){
        rtmp_time_t verify_time;
        FAIL_IF_ERR( rtmp_chunk_read_shake_2( conn->inflow, &verify_time, &conn->self_shake_recv_time, conn->nonce_s, RTMP_NONCE_SIZE ) );
        #ifdef RTMP_SPEC_ENFORCE_HANDSHAKE_TIMES
        if( verify_time != conn->self_time ){
            return RTMP_ERR_INVALID;
        }
        #endif
        #ifdef RTMP_SPEC_ENFORCE_HANDSHAKE_NONCES
        if( memcmp( conn->nonce_c, conn->nonce_s, RTMP_NONCE_SIZE ) != 0 ){
            return RTMP_ERR_INVALID;
        }
        #endif
        conn->status |= RTMP_STATUS_SHAKING_S2;
        rtmp_chunk_conn_service_shake_tryfinalize( conn );
    }
    return RTMP_ERR_NONE;
}

static rtmp_err_t rtmp_chunk_conn_service_shake_server_emit( rtmp_chunk_conn_t conn ){
    //Emit S0 if we haven't already
    if( !( conn->status & RTMP_STATUS_SHAKING_S0 ) ){
        //Server MUST wait to emit S0 until C0 is received.
        if( !( conn->status & RTMP_STATUS_SHAKING_C0 ) ){
            return RTMP_ERR_NOT_READY;
        }
        FAIL_IF_ERR( rtmp_chunk_emit_shake_0( conn->outflow ) );
        conn->status |= RTMP_STATUS_SHAKING_S0;
    }
    //Emit S1 if we haven't already
    else if( !( conn->status & RTMP_STATUS_SHAKING_S1 ) ){
        //Server MUST wait to emit S0 until C0 is received.
        //Condition implied in previous block.
        FAIL_IF_ERR( rtmp_nonce_gen( &(conn->nonce_s), RTMP_NONCE_SIZE ) );
        conn->self_time = rtmp_get_time();
        FAIL_IF_ERR( rtmp_chunk_emit_shake_1( conn->outflow, conn->self_time, conn->nonce_s, RTMP_NONCE_SIZE ) );
        conn->status |= RTMP_STATUS_SHAKING_S1;
    }
    //Emit S2 if we have C0 and C1, and S2 hasn't yet been emitted.
    else if(!( conn->status & RTMP_STATUS_SHAKING_S2 )   &&
             ( conn->status & RTMP_STATUS_SHAKING_C0 )   &&
             ( conn->status & RTMP_STATUS_SHAKING_C1 )       ){
        FAIL_IF_ERR( rtmp_chunk_emit_shake_2( conn->outflow, conn->peer_time, conn->peer_shake_recv_time, conn->nonce_c, RTMP_NONCE_SIZE ) );
        conn->status |= RTMP_STATUS_SHAKING_S2;
        rtmp_chunk_conn_service_shake_tryfinalize( conn );
    }
    return RTMP_ERR_NONE;
}

static rtmp_err_t rtmp_chunk_conn_service_shake_server_ingest( rtmp_chunk_conn_t conn ){
    //Grab C0 if we haven't already
    if( !( conn->status & RTMP_STATUS_SHAKING_C0 ) ){
        FAIL_IF_ERR( rtmp_chunk_read_shake_0( conn->inflow ) );
        conn->status |= RTMP_STATUS_SHAKING_C0;
    }
    //Grab C1 if we haven't already
    else if( !( conn->status & RTMP_STATUS_SHAKING_C1 ) ){
        FAIL_IF_ERR( rtmp_nonce_alloc( &conn->nonce_c, RTMP_NONCE_SIZE ) );
        conn->peer_shake_recv_time = rtmp_get_time();
        FAIL_IF_ERR( rtmp_chunk_read_shake_1( conn->inflow, &conn->peer_time, conn->nonce_c, RTMP_NONCE_SIZE ) );
        conn->status |= RTMP_STATUS_SHAKING_C1;
    }
    //Grab C2 if we have already emitted S0 and S1.
    else if(!( conn->status & RTMP_STATUS_SHAKING_C2 )   &&
             ( conn->status & RTMP_STATUS_SHAKING_S0 )   &&
             ( conn->status & RTMP_STATUS_SHAKING_S1 )       ){
        rtmp_time_t verify_time;
        FAIL_IF_ERR( rtmp_chunk_read_shake_2( conn->inflow, &verify_time, &conn->self_shake_recv_time, conn->nonce_c, RTMP_NONCE_SIZE ) );
        #ifdef RTMP_SPEC_ENFORCE_HANDSHAKE_TIMES
        if( verify_time != conn->self_time ){
            return RTMP_ERR_INVALID;
        }
        #endif
        #ifdef RTMP_SPEC_ENFORCE_HANDSHAKE_NONCES
        if( memcmp( conn->nonce_c, conn->nonce_s, RTMP_NONCE_SIZE ) != 0 ){
            return RTMP_ERR_INVALID;
        }
        #endif
        conn->status |= RTMP_STATUS_SHAKING_C2;
        rtmp_chunk_conn_service_shake_tryfinalize( conn );
    }
    return RTMP_ERR_NONE;
}

static rtmp_err_t rtmp_chunk_conn_service_shake( rtmp_chunk_conn_t conn, rtmp_io_t io_status ){
    //Check if we're a client
    rtmp_err_t err = RTMP_ERR_NONE;
    if( conn->status & RTMP_STATUS_IS_CLIENT ){
        if( (io_status & RTMP_IO_OUT) && (err = rtmp_chunk_conn_service_shake_client_emit( conn ) ) >= RTMP_ERR_ERROR ){
            return err;
        }
        if( (io_status & RTMP_IO_IN) && (err = rtmp_chunk_conn_service_shake_client_ingest( conn ) ) >= RTMP_ERR_ERROR ){
            return err;
        }
    }
    else{
        if( (io_status & RTMP_IO_OUT) && (err = rtmp_chunk_conn_service_shake_server_emit( conn ) ) >= RTMP_ERR_ERROR ){
            return err;
        }
        if( (io_status & RTMP_IO_IN) && (err = rtmp_chunk_conn_service_shake_server_ingest( conn ) ) >= RTMP_ERR_ERROR ){
            return err;
        }
    }
    return err;
}

#undef FAIL_IF_ERR
#define FAIL_IF_ERR(a) {rtmp_err_t err; if((err=(a))>=RTMP_ERR_ERROR){ rtmp_chunk_conn_call_event( conn, RTMP_EVENT_FAILED ); return err;}}

static rtmp_err_t rtmp_chunk_conn_service_recv_set_chunk_size(rtmp_chunk_conn_t conn){
    if( conn->control_message_len >= 4 ){
        conn->peer_chunk_size = ntoh_read_ud( conn->control_message_buffer );
        printf("Peer set chunk size to %d\n", conn->peer_chunk_size);
        return RTMP_ERR_NONE;
    }
    return RTMP_ERR_INVALID;
}

static rtmp_err_t rtmp_chunk_conn_service_recv_abort(rtmp_chunk_conn_t conn){
    if( conn->control_message_len >= 4 ){
        unsigned int chunk_stream = ntoh_read_ud( conn->control_message_buffer );

        rtmp_chunk_stream_message_t msg;
        rtmp_chunk_stream_message_internal_t *cached = rtmp_cache_get( conn->stream_cache_in, chunk_stream);
        if( cached == nullptr ){
            return RTMP_ERR_INADEQUATE_CHUNK;
        }
        memcpy( &msg, cached, sizeof( rtmp_chunk_stream_message_t ) );
        msg.chunk_stream_id = chunk_stream;

        printf("Peer aborted chunk %d\n", chunk_stream);
        rtmp_chunk_conn_call_chunk( conn, 0, 0, &msg );
        return RTMP_ERR_NONE;
    }
    return RTMP_ERR_INVALID;
}

static rtmp_err_t rtmp_chunk_conn_service_recv_set_peer_bwidth(rtmp_chunk_conn_t conn){
    if( conn->control_message_len >= 5 ){
        rtmp_limit_t limit = conn->control_message_buffer[4];
        unsigned int new_size = ntoh_read_ud( conn->control_message_buffer );
        unsigned int old_size = conn->self_window_size;
        printf("Peer requested window size of %d\n", new_size);
        switch( limit ){
            case RTMP_LIMIT_DYNAMIC://If previous attempt was hard, treat as hard. Otherwise, ignore.
                if( conn->peer_bandwidth_type == RTMP_LIMIT_HARD ){
                    limit = RTMP_LIMIT_HARD;
                }
            case RTMP_LIMIT_HARD:   //don't change new size
                break;
            case RTMP_LIMIT_SOFT:   //Limit to whichever is smaller
                if( old_size < new_size ){
                    new_size = old_size;
                }
                break;
            default: return RTMP_ERR_INVALID;
        }
        conn->peer_bandwidth_type = limit;
        if( new_size == old_size ){
            return RTMP_ERR_NONE;
        }
        return rtmp_chunk_conn_set_window_ack_size( conn, new_size );
    }
    return RTMP_ERR_INVALID;
}

static rtmp_err_t rtmp_chunk_conn_service_recv_ack(rtmp_chunk_conn_t conn ){
    if( conn->control_message_len >= 4 ){
        size_t amount = ntoh_read_ud( conn->control_message_buffer );
        printf("Peer acknowledged %d bytes\n", amount);
        ors_data_reset_write_amount( conn->outflow, amount );
        return RTMP_ERR_NONE;
    }
    return RTMP_ERR_INVALID;
}

static rtmp_err_t rtmp_chunk_conn_service_recv_win_ack_size(rtmp_chunk_conn_t conn ){
    if( conn->control_message_len >= 4 ){
        conn->peer_window_size = ntoh_read_ud( conn->control_message_buffer );
        printf("Peer set window size to %d bytes\n", conn->peer_window_size);
        return rtmp_chunk_conn_acknowledge( conn );
    }
    return RTMP_ERR_INVALID;
}

static rtmp_err_t rtmp_chunk_conn_service_recv_cmd( rtmp_chunk_conn_t conn, size_t available, size_t remaining, rtmp_chunk_stream_message_t *msg ){
    printf("Service Chunk Called!\n");
    rtmp_print_message( msg );
    if( available == remaining && remaining == 0 ){
        return RTMP_ERR_NONE;
    }
    if( available + conn->control_message_len >= RTMP_CONTROL_BUFFER_SIZE ){
        available = RTMP_CONTROL_BUFFER_SIZE - conn->control_message_len;
    }
    if( ors_data_read( conn->inflow, conn->control_message_buffer, available ) < available ){
        return RTMP_ERR_BAD_READ;
    }
    conn->control_message_len += available;
    if( remaining == 0 ){
        switch( msg->message_type ){
            case RTMP_MSG_SET_CHUNK_SIZE:
                return rtmp_chunk_conn_service_recv_set_chunk_size( conn );
            case RTMP_MSG_ABORT:
                return rtmp_chunk_conn_service_recv_abort( conn );
            case RTMP_MSG_SET_PEER_BWIDTH:
                return rtmp_chunk_conn_service_recv_set_peer_bwidth( conn );
            case RTMP_MSG_ACK:
                return rtmp_chunk_conn_service_recv_ack( conn );
            case RTMP_MSG_WIN_ACK_SIZE:
                return rtmp_chunk_conn_service_recv_win_ack_size( conn );
        }
    }
    return RTMP_ERR_INVALID;
}

static rtmp_err_t rtmp_chunk_conn_service_recv( rtmp_chunk_conn_t conn, rtmp_io_t io_status ){
    rtmp_chunk_stream_message_t *msg;
    FAIL_IF_ERR( rtmp_chunk_read_hdr( conn->inflow, &msg, conn->stream_cache_in ) );

    rtmp_chunk_stream_message_internal_t *previous = rtmp_cache_get( conn->stream_cache_in, msg->chunk_stream_id );

    //Limit the data output to message_length
    size_t start = ors_data_amount_read(conn->inflow);

    rtmp_err_t ret = RTMP_ERR_NONE;

    size_t available = msg->message_length;
    size_t processing = previous->processed;
    if( processing == 0 ){
        previous->processed = available;
    }
    else{
        available = processing;
    }
    if( available > conn->peer_chunk_size ){
        available = conn->peer_chunk_size;
    }

    previous->processed -= available;
    processing = previous->processed;

    if( msg->chunk_stream_id == RTMP_CONTROL_CHUNK_STREAM && msg->message_stream_id == RTMP_CONTROL_MSG_STREAM ){
        ret = rtmp_chunk_conn_service_recv_cmd( conn, available, processing, msg );
    }
    else{
        ret = rtmp_chunk_conn_call_chunk( conn, available, processing, msg );
    }

    //If message_length bytes weren't read, fast forward data
    size_t end = ors_data_amount_read(conn->inflow);
    if( available - (end - start) > 0 ){
        ors_data_seek( conn->inflow, available - (end - start), ORS_SEEK_OFFSET );
    }
    return ret;
}

rtmp_err_t rtmp_chunk_conn_service( rtmp_chunk_conn_t conn, rtmp_io_t io_status ){
    if( ( conn->status & RTMP_STATUS_SHAKING_DONE ) != RTMP_STATUS_SHAKING_DONE ){
        return rtmp_chunk_conn_service_shake( conn, io_status );
    }
    if( io_status & RTMP_IO_IN ){
        return rtmp_chunk_conn_service_recv(conn, io_status );
    }
    return RTMP_ERR_NONE;
}


rtmp_err_t rtmp_chunk_conn_register_callbacks( rtmp_chunk_conn_t conn, rtmp_chunk_proc chunk_cb, rtmp_event_proc event_cb, void *user ){
    conn->callback_chunk = chunk_cb;
    conn->callback_event = event_cb;
    conn->userdata = user;
    return RTMP_ERR_NONE;
}


/*static rtmp_err_t rtmp_chunk_conn_emit_number(rtmp_chunk_conn_t conn, rtmp_message_type_t msg_type, unsigned int value){
    rtmp_chunk_stream_message_t msg;
    msg.chunk_stream_id = RTMP_CONTROL_CHUNK_STREAM;
    msg.message_stream_id = RTMP_CONTROL_MSG_STREAM;
    msg.message_length = 4;
    msg.timestamp = 0;
    msg.message_type = msg_type;
    FAIL_IF_ERR(rtmp_chunk_emit_hdr( conn->outflow, &msg, conn->stream_cache ));
    ntoh_write_ud( &value, value );
    if( ors_data_write( conn->outflow, (const byte*)&value, sizeof( value ) ) < sizeof( value ) ){
        return RTMP_ERR_BAD_WRITE;
    }
    return RTMP_ERR_NONE;
}*/

rtmp_err_t rtmp_chunk_conn_set_chunk_size( rtmp_chunk_conn_t conn, unsigned int size ){
    size &= ~(1 << 31);

    byte buffer[4];
    ntoh_write_ud( buffer, size );

    rtmp_err_t err =
        rtmp_chunk_conn_send_message(
            conn,
            RTMP_MSG_SET_CHUNK_SIZE,
            RTMP_CONTROL_CHUNK_STREAM,
            RTMP_CONTROL_MSG_STREAM,
            0,
            buffer,
            sizeof( buffer ) );

    if( err < RTMP_ERR_ERROR ){
        conn->self_chunk_size = size;
    }

    return err;
}

rtmp_err_t rtmp_chunk_conn_abort( rtmp_chunk_conn_t conn, unsigned int chunk_stream ){
    byte buffer[4];
    ntoh_write_ud( buffer, chunk_stream );

    return rtmp_chunk_conn_send_message(
        conn,
        RTMP_MSG_ABORT,
        RTMP_CONTROL_CHUNK_STREAM,
        RTMP_CONTROL_MSG_STREAM,
        0,
        buffer,
        sizeof( buffer ) );
}

rtmp_err_t rtmp_chunk_conn_acknowledge( rtmp_chunk_conn_t conn ){
    byte buffer[4];
    ntoh_write_ud( buffer, ors_data_amount_read(conn->inflow) );
    ors_data_reset_read_amount(conn->inflow, -1);

    return rtmp_chunk_conn_send_message(
        conn,
        RTMP_MSG_ACK,
        RTMP_CONTROL_CHUNK_STREAM,
        RTMP_CONTROL_MSG_STREAM,
        0,
        buffer,
        sizeof( buffer ) );
}

rtmp_err_t rtmp_chunk_conn_set_window_ack_size( rtmp_chunk_conn_t conn, unsigned int size ){
    conn->self_window_size = size;

    byte buffer[4];
    ntoh_write_ud( buffer, size );

    return rtmp_chunk_conn_send_message(
        conn,
        RTMP_MSG_WIN_ACK_SIZE,
        RTMP_CONTROL_CHUNK_STREAM,
        RTMP_CONTROL_MSG_STREAM,
        0,
        buffer,
        sizeof( buffer ) );
}

rtmp_err_t rtmp_chunk_conn_set_peer_bwidth( rtmp_chunk_conn_t conn, unsigned int size, rtmp_limit_t limit_type ){
    byte buffer[5];
    ntoh_write_ud( buffer, size );
    buffer[4] = limit_type;

    return rtmp_chunk_conn_send_message(
        conn,
        RTMP_MSG_SET_PEER_BWIDTH,
        RTMP_CONTROL_CHUNK_STREAM,
        RTMP_CONTROL_MSG_STREAM,
        0,
        buffer,
        sizeof( buffer ) );
}


rtmp_err_t
rtmp_chunk_conn_send_message(
    rtmp_chunk_conn_t conn,
    byte message_type,
    unsigned int chunk_stream,
    unsigned int message_stream,
    unsigned int timestamp,
    byte *data,
    size_t length
){
    rtmp_chunk_stream_message_t msg;
    msg.chunk_stream_id = chunk_stream;
    msg.message_stream_id = message_stream;
    msg.message_length = length;
    msg.timestamp = timestamp;
    msg.message_type = message_type;

    size_t written = 0;
    while( written < length ){
        size_t chunk_len = length - written;
        if( chunk_len > conn->self_chunk_size ){
            chunk_len = conn->self_chunk_size;
        }
        FAIL_IF_ERR(rtmp_chunk_emit_hdr( conn->outflow, &msg, conn->stream_cache_out ));
        if( ors_data_write( conn->outflow, data + written, chunk_len ) < chunk_len ){
            return RTMP_ERR_BAD_WRITE;
        }
        written += chunk_len;
    }
    return RTMP_ERR_NONE;
}

