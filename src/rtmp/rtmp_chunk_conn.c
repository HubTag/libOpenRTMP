/*
    rtmp_chunk_conn.c

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
#include "rtmp/rtmp_private.h"
#include "rtmp/chunk/rtmp_chunk_conn.h"
#include "rtmp_debug.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if RTMP_LOG_LEVEL == 0
#define RTMP_GEN_ERROR(c,err) rtmp_chunk_conn_gen_error(c,err,__LINE__,__FILE__,"")
#define RTMP_GEN_ERROR_MSG(c,err,msg) rtmp_chunk_conn_gen_error(c,err,__LINE__,__FILE__,msg)
#else
#define RTMP_GEN_ERROR(c,err) (err)
#define RTMP_GEN_ERROR_MSG(c,err,msg) (err)
#endif

//This is wrapped around all error return values in order to handle logging.
rtmp_err_t rtmp_chunk_conn_gen_error(rtmp_chunk_conn_t conn, rtmp_err_t err, size_t line, const char *file, const char *msg);


rtmp_chunk_conn_t rtmp_chunk_conn_create( bool is_client ){
    rtmp_chunk_conn_t ret = calloc( 1, sizeof( struct rtmp_chunk_conn ) );

    ret->in = ringbuffer_create( RTMP_DEFAULT_IO_BUFFER_SIZE );
    ret->out = ringbuffer_create( RTMP_DEFAULT_IO_BUFFER_SIZE );

    ret->stream_cache_in = rtmp_cache_create();
    ret->stream_cache_out = rtmp_cache_create();

    ret->status = RTMP_STATUS_UNINIT | (is_client ? RTMP_STATUS_IS_CLIENT : 0 );

    ret->self_chunk_size = RTMP_DEFAULT_CHUNK_SIZE;
    ret->peer_chunk_size = RTMP_DEFAULT_CHUNK_SIZE;

    ret->self_window_size = RTMP_DEFAULT_WINDOW_SIZE;
    ret->peer_window_size = RTMP_DEFAULT_WINDOW_SIZE;

    ret->peer_bandwidth_type = RTMP_DEFAULT_BANDWIDTH_TYPE;
    return ret;
}

rtmp_err_t rtmp_chunk_conn_close( rtmp_chunk_conn_t conn ){
    ringbuffer_destroy( conn->in );
    ringbuffer_destroy( conn->out );
    rtmp_nonce_del( &conn->nonce_c );
    rtmp_nonce_del( &conn->nonce_s );
    rtmp_cache_destroy( conn->stream_cache_in );
    rtmp_cache_destroy( conn->stream_cache_out );

    free( conn );
    return RTMP_GEN_ERROR(conn, RTMP_ERR_NONE);
}

bool rtmp_chunk_conn_connected( rtmp_chunk_conn_t conn ){
    return ( conn->status & RTMP_STATUS_SHAKING_DONE ) == RTMP_STATUS_SHAKING_DONE;
}


rtmp_err_t rtmp_chunk_conn_call_event( rtmp_chunk_conn_t conn, rtmp_event_t event ){
    rtmp_err_t err = RTMP_ERR_NONE;
    if( conn->callback_event ){
        switch( conn->callback_event( conn, event, conn->userdata ) ){
            case RTMP_CB_CONTINUE:
                break;
            case RTMP_CB_ERROR:
            case RTMP_CB_ABORT:
                err = RTMP_ERR_ERROR;
                break;
        }
    }
    return RTMP_GEN_ERROR(conn, err);
}

rtmp_err_t rtmp_chunk_conn_call_chunk( rtmp_chunk_conn_t conn, const void *input, size_t available, size_t remaining, rtmp_chunk_stream_message_t *msg ){
    rtmp_err_t err = RTMP_ERR_NONE;
    if( conn->callback_chunk ){
        switch( conn->callback_chunk( conn, input, available, remaining, msg, conn->userdata ) ){
            case RTMP_CB_CONTINUE:
                break;
            case RTMP_CB_ERROR:
            case RTMP_CB_ABORT:
                err = RTMP_ERR_ERROR;
                break;
        }
    }
    return RTMP_GEN_ERROR(conn, err);
}


static void rtmp_chunk_conn_service_shake_tryfinalize( rtmp_chunk_conn_t conn ){
    //If shaking is done
    if( ( conn->status & RTMP_STATUS_SHAKING_DONE ) == RTMP_STATUS_SHAKING_DONE ){
        //Free the nonces used in the handshake
        rtmp_nonce_del( &conn->nonce_c );
        rtmp_nonce_del( &conn->nonce_s );
        //Fire the connection success event
        rtmp_chunk_conn_call_event( conn, RTMP_EVENT_CONNECT_SUCCESS );
        conn->lag = 0;
        //Calculate latency
        if( conn->peer_shake_recv_time > conn->self_time ){
            conn->lag = conn->peer_shake_recv_time - conn->self_time;
        }
        else{
            conn->lag = conn->self_time - conn->peer_shake_recv_time;
        }
    }
}

#define FAIL_IF_ERR(a) {rtmp_err_t err; if((err=(a))>=RTMP_ERR_ERROR){ if( err != RTMP_ERR_AGAIN ){rtmp_chunk_conn_call_event( conn, RTMP_EVENT_CONNECT_FAIL );} return RTMP_GEN_ERROR(conn, err);}}

static rtmp_err_t rtmp_chunk_conn_service_shake_client_emit( rtmp_chunk_conn_t conn ){
    //Emit C0 if we haven't already
    if( !( conn->status & RTMP_STATUS_SHAKING_C0 ) ){
        FAIL_IF_ERR( rtmp_chunk_emit_shake_0( conn->out ) );
        conn->status |= RTMP_STATUS_SHAKING_C0;
    }
    //Emit C1 if we haven't already
    else if( !( conn->status & RTMP_STATUS_SHAKING_C1 ) ){
        FAIL_IF_ERR( rtmp_nonce_gen( &(conn->nonce_c), RTMP_NONCE_SIZE ) );
        conn->self_time = rtmp_get_time();
        FAIL_IF_ERR( rtmp_chunk_emit_shake_1( conn->out, conn->self_time, conn->nonce_c, RTMP_NONCE_SIZE ) );
        conn->status |= RTMP_STATUS_SHAKING_C1;
    }
    //Emit C2 if we have S0 and S1, and C2 hasn't yet been emitted.
    else if(!( conn->status & RTMP_STATUS_SHAKING_C2 )   &&
             ( conn->status & RTMP_STATUS_SHAKING_S0 )   &&
             ( conn->status & RTMP_STATUS_SHAKING_S1 )       ){
        FAIL_IF_ERR( rtmp_chunk_emit_shake_2( conn->out, conn->peer_time, conn->peer_shake_recv_time, conn->nonce_s, RTMP_NONCE_SIZE ) );
        conn->status |= RTMP_STATUS_SHAKING_C2;
        rtmp_chunk_conn_service_shake_tryfinalize( conn );
    }
    return RTMP_GEN_ERROR(conn, RTMP_ERR_NONE);
}

static rtmp_err_t rtmp_chunk_conn_service_shake_client_ingest( rtmp_chunk_conn_t conn ){
    //Grab S0 if we haven't already
    if( !( conn->status & RTMP_STATUS_SHAKING_S0 ) ){
        //Server MUST wait until C0 before sending S0.
        if( !( conn->status & RTMP_STATUS_SHAKING_C0 ) ){
            return RTMP_GEN_ERROR(conn, RTMP_ERR_NOT_READY);
        }
        FAIL_IF_ERR( rtmp_chunk_read_shake_0( conn->in ) );
        conn->status |= RTMP_STATUS_SHAKING_S0;
    }
    //Grab S1 if we haven't already
    else if( !( conn->status & RTMP_STATUS_SHAKING_S1 ) ){
        //Server MUST wait until C0 before sending S1.
        if( !( conn->status & RTMP_STATUS_SHAKING_C0 ) ){
            return RTMP_GEN_ERROR(conn, RTMP_ERR_NOT_READY);
        }
        FAIL_IF_ERR( rtmp_nonce_alloc( &conn->nonce_s, RTMP_NONCE_SIZE ) );
        conn->peer_shake_recv_time = rtmp_get_time();
        FAIL_IF_ERR( rtmp_chunk_read_shake_1( conn->in, &conn->peer_time, conn->nonce_s, RTMP_NONCE_SIZE ) );
        conn->status |= RTMP_STATUS_SHAKING_S1;
    }
    //Grab S2 if we have already emitted C0 and C1.
    else if(!( conn->status & RTMP_STATUS_SHAKING_S2 )   &&
             ( conn->status & RTMP_STATUS_SHAKING_C0 )   &&
             ( conn->status & RTMP_STATUS_SHAKING_C1 )       ){
        rtmp_time_t verify_time;
        void *verify_nonce = nullptr;
        FAIL_IF_ERR( rtmp_nonce_alloc( &verify_nonce, RTMP_NONCE_SIZE ) );

        FAIL_IF_ERR( rtmp_chunk_read_shake_2( conn->in, &verify_time, &conn->self_shake_recv_time, verify_nonce, RTMP_NONCE_SIZE ) );
        #ifdef RTMP_SPEC_ENFORCE_HANDSHAKE_TIMES
        if( verify_time != conn->self_time ){
            rtmp_nonce_del( &verify_nonce );
            return RTMP_GEN_ERROR(conn, RTMP_ERR_INVALID);
        }
        #endif
        #ifdef RTMP_SPEC_ENFORCE_HANDSHAKE_NONCES
        if( memcmp( conn->nonce_c, verify_nonce, RTMP_NONCE_SIZE ) != 0 ){
            rtmp_nonce_del( &verify_nonce );
            return RTMP_GEN_ERROR(conn, RTMP_ERR_INVALID);
        }
        #endif
        conn->status |= RTMP_STATUS_SHAKING_S2;
        rtmp_chunk_conn_service_shake_tryfinalize( conn );
        rtmp_nonce_del( &verify_nonce );
    }
    return RTMP_GEN_ERROR(conn, RTMP_ERR_NONE);
}

static rtmp_err_t rtmp_chunk_conn_service_shake_server_emit( rtmp_chunk_conn_t conn ){
    //Emit S0 if we haven't already
    if( !( conn->status & RTMP_STATUS_SHAKING_S0 ) ){
        //Server MUST wait to emit S0 until C0 is received.
        if( !( conn->status & RTMP_STATUS_SHAKING_C0 ) ){
            return RTMP_GEN_ERROR(conn, RTMP_ERR_NOT_READY);
        }
        FAIL_IF_ERR( rtmp_chunk_emit_shake_0( conn->out ) );
        conn->status |= RTMP_STATUS_SHAKING_S0;
    }
    //Emit S1 if we haven't already
    else if( !( conn->status & RTMP_STATUS_SHAKING_S1 ) ){
        //Server MUST wait to emit S0 until C0 is received.
        //Condition implied in previous block.
        FAIL_IF_ERR( rtmp_nonce_gen( &(conn->nonce_s), RTMP_NONCE_SIZE ) );
        conn->self_time = rtmp_get_time();
        FAIL_IF_ERR( rtmp_chunk_emit_shake_1( conn->out, conn->self_time, conn->nonce_s, RTMP_NONCE_SIZE ) );
        conn->status |= RTMP_STATUS_SHAKING_S1;
    }
    //Emit S2 if we have C0 and C1, and S2 hasn't yet been emitted.
    else if(!( conn->status & RTMP_STATUS_SHAKING_S2 )   &&
             ( conn->status & RTMP_STATUS_SHAKING_C0 )   &&
             ( conn->status & RTMP_STATUS_SHAKING_C1 )       ){
        FAIL_IF_ERR( rtmp_chunk_emit_shake_2( conn->out, conn->peer_time, conn->peer_shake_recv_time, conn->nonce_c, RTMP_NONCE_SIZE ) );
        conn->status |= RTMP_STATUS_SHAKING_S2;
        rtmp_chunk_conn_service_shake_tryfinalize( conn );
    }
    return RTMP_GEN_ERROR(conn, RTMP_ERR_NONE);
}

static rtmp_err_t rtmp_chunk_conn_service_shake_server_ingest( rtmp_chunk_conn_t conn ){
    //Grab C0 if we haven't already
    if( !( conn->status & RTMP_STATUS_SHAKING_C0 ) ){
        FAIL_IF_ERR( rtmp_chunk_read_shake_0( conn->in ) );
        conn->status |= RTMP_STATUS_SHAKING_C0;
    }
    //Grab C1 if we haven't already
    else if( !( conn->status & RTMP_STATUS_SHAKING_C1 ) ){
        FAIL_IF_ERR( rtmp_nonce_alloc( &conn->nonce_c, RTMP_NONCE_SIZE ) );
        conn->peer_shake_recv_time = rtmp_get_time();
        FAIL_IF_ERR( rtmp_chunk_read_shake_1( conn->in, &conn->peer_time, conn->nonce_c, RTMP_NONCE_SIZE ) );
        conn->status |= RTMP_STATUS_SHAKING_C1;
    }
    //Grab C2 if we have already emitted S0 and S1.
    else if(!( conn->status & RTMP_STATUS_SHAKING_C2 )   &&
             ( conn->status & RTMP_STATUS_SHAKING_S0 )   &&
             ( conn->status & RTMP_STATUS_SHAKING_S1 )       ){
        rtmp_time_t verify_time;
        void *verify_nonce = nullptr;
        FAIL_IF_ERR( rtmp_nonce_alloc( &verify_nonce, RTMP_NONCE_SIZE ) );
        FAIL_IF_ERR( rtmp_chunk_read_shake_2( conn->in, &verify_time, &conn->self_shake_recv_time, verify_nonce, RTMP_NONCE_SIZE ) );
        #ifdef RTMP_SPEC_ENFORCE_HANDSHAKE_TIMES
        if( verify_time != conn->self_time ){
            rtmp_nonce_del( &verify_nonce );
            return RTMP_GEN_ERROR(conn, RTMP_ERR_INVALID);
        }
        #endif
        #ifdef RTMP_SPEC_ENFORCE_HANDSHAKE_NONCES
        if( memcmp( verify_nonce, conn->nonce_s, RTMP_NONCE_SIZE ) != 0 ){
            rtmp_nonce_del( &verify_nonce );
            return RTMP_GEN_ERROR(conn, RTMP_ERR_INVALID);
        }
        #endif
        conn->status |= RTMP_STATUS_SHAKING_C2;
        rtmp_chunk_conn_service_shake_tryfinalize( conn );
        rtmp_nonce_del( &verify_nonce );
    }
    return RTMP_GEN_ERROR(conn, RTMP_ERR_NONE);
}

static rtmp_err_t rtmp_chunk_conn_service_shake( rtmp_chunk_conn_t conn, rtmp_io_t io_status ){
    //Check if we're a client
    rtmp_err_t err = RTMP_GEN_ERROR(conn, RTMP_ERR_NONE);
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
#define FAIL_IF_ERR(a) {rtmp_err_t err; if((err=(a))>=RTMP_ERR_ERROR){ if( err != RTMP_ERR_AGAIN ){rtmp_chunk_conn_call_event( conn, RTMP_EVENT_FAILED );} return RTMP_GEN_ERROR(conn, err);}}

static rtmp_err_t rtmp_chunk_conn_service_recv_set_chunk_size(rtmp_chunk_conn_t conn){
    if( conn->control_message_len >= 4 ){
        conn->peer_chunk_size = ntoh_read_ud( conn->control_message_buffer );
        if( conn->peer_chunk_size < RTMP_MIN_PEER_CHUNK_SIZE ){
            conn->peer_chunk_size = RTMP_MIN_PEER_CHUNK_SIZE;
        }
        return RTMP_GEN_ERROR(conn, RTMP_ERR_NONE);
    }
    return RTMP_GEN_ERROR(conn, RTMP_ERR_INVALID);
}

static rtmp_err_t rtmp_chunk_conn_service_recv_abort(rtmp_chunk_conn_t conn){
    if( conn->control_message_len >= 4 ){
        uint32_t chunk_stream = ntoh_read_ud( conn->control_message_buffer );

        rtmp_chunk_stream_message_t msg;
        rtmp_chunk_stream_message_internal_t *cached = rtmp_cache_get( conn->stream_cache_in, chunk_stream);
        rtmp_chunk_stream_message_internal_t *agmsg = rtmp_cache_get( conn->stream_cache_in, RTMP_CACHE_AGGREGATE );
        if( cached == nullptr || agmsg == nullptr ){
            return RTMP_GEN_ERROR(conn, RTMP_ERR_INADEQUATE_CHUNK);
        }
        memcpy( &msg, cached, sizeof( rtmp_chunk_stream_message_t ) );
        msg.chunk_stream_id = chunk_stream;

        rtmp_chunk_conn_call_chunk( conn, nullptr, 0, 0, &msg );
        cached->processed = 0;
        agmsg->processed = 0;
        agmsg->initialized = false;
        return RTMP_GEN_ERROR(conn, RTMP_ERR_NONE);
    }
    return RTMP_GEN_ERROR(conn, RTMP_ERR_INVALID);
}

static rtmp_err_t rtmp_chunk_conn_service_recv_set_peer_bwidth(rtmp_chunk_conn_t conn){
    if( conn->control_message_len >= 5 ){
        rtmp_limit_t limit = conn->control_message_buffer[4];
        uint32_t new_size = ntoh_read_ud( conn->control_message_buffer );
        uint32_t old_size = conn->self_window_size;
        if( new_size < RTMP_MIN_WINDOW_SIZE ){
            new_size = RTMP_MIN_WINDOW_SIZE;
        }
        switch( limit ){
            case RTMP_LIMIT_DYNAMIC:
                //If previous attempt was hard, treat as hard. Otherwise, ignore.
                if( conn->peer_bandwidth_type == RTMP_LIMIT_HARD ){
                    limit = RTMP_LIMIT_HARD;
                }
            case RTMP_LIMIT_HARD:
                //don't change new size
                break;
            case RTMP_LIMIT_SOFT:
                //Limit to whichever is smaller
                if( old_size < new_size ){
                    new_size = old_size;
                }
                break;
            default: return RTMP_GEN_ERROR(conn, RTMP_ERR_INVALID);
        }
        conn->peer_bandwidth_type = limit;
        if( new_size == old_size ){
            //Since the size hasn't changed, there's no need to alert the peer
            return RTMP_GEN_ERROR(conn, RTMP_ERR_NONE);
        }
        //Alert the peer to the new size
        return rtmp_chunk_conn_set_window_ack_size( conn, new_size );
    }
    return RTMP_GEN_ERROR(conn, RTMP_ERR_INVALID);
}

static rtmp_err_t rtmp_chunk_conn_service_recv_ack(rtmp_chunk_conn_t conn ){
    if( conn->control_message_len >= 4 ){
        size_t amount = ntoh_read_ud( conn->control_message_buffer );
        if( amount > conn->bytes_out ){
            //Realistically this should never happen.
            //This implies that the peer has received more bytes than we have sent.
            //This shouldn't cause any issues though, so emit only a soft error.
            conn->bytes_out = 0;
            return RTMP_GEN_ERROR(conn, RTMP_ERR_DIVERGENCE_METER_ERROR);
        }
        conn->bytes_out -= amount;
        return RTMP_GEN_ERROR(conn, RTMP_ERR_NONE);
    }
    return RTMP_GEN_ERROR(conn, RTMP_ERR_INVALID);
}

static rtmp_err_t rtmp_chunk_conn_service_recv_win_ack_size(rtmp_chunk_conn_t conn ){
    if( conn->control_message_len >= 4 ){
        conn->peer_window_size = ntoh_read_ud( conn->control_message_buffer );
        if( conn->peer_window_size < RTMP_MIN_PEER_WINDOW_SIZE ){
            conn->peer_window_size = RTMP_MIN_PEER_WINDOW_SIZE;
        }
        return rtmp_chunk_conn_acknowledge( conn );
    }
    return RTMP_GEN_ERROR(conn, RTMP_ERR_INVALID);
}


//This is the wrong place for this...
//TODO: Move into RTMP stream handler
/*static rtmp_err_t rtmp_chunk_conn_service_recv_aggregate( rtmp_chunk_conn_t conn, const void *input, size_t available, size_t remaining, rtmp_chunk_stream_message_t *msg ){
    //Fetch our aggregate message cache
    rtmp_chunk_stream_message_internal_t *agmsg = rtmp_cache_get( conn->stream_cache_in, RTMP_CACHE_AGGREGATE );
    if( agmsg == nullptr ){
        return RTMP_ERR_INADEQUATE_CHUNK;
    }
    size_t offset = 0;
    if( agmsg->processed == 0 ){
        //This is a new message, try fetching the header.
        if( available + conn->control_message_len < RTMP_MESSAGE_HEADER_SIZE ){
            //The number of available bytes isn't enough to fill the buffer
            memcpy( conn->control_message_buffer + conn->control_message_len, input, available );
            conn->control_message_len += available;
            return RTMP_ERR_AGAIN;
        }
        else if( conn->control_message_len < RTMP_MESSAGE_HEADER_SIZE ){
            //The number of available bytes is enough to fill the buffer.
            size_t amount = RTMP_MESSAGE_HEADER_SIZE - conn->control_message_len;
            memcpy( conn->control_message_buffer + conn->control_message_len, input, amount );
            conn->control_message_len += amount;
            offset = amount;
        }
        //With the filled buffer, parse it into the cache
        agmsg->msg.message_type = conn->control_message_buffer[0];
        agmsg->msg.message_length = ntoh_read_ud3( conn->control_message_buffer + 1 );
        agmsg->msg.timestamp = ntoh_read_ud( conn->control_message_buffer + 4 );
        agmsg->msg.message_stream_id = ntoh_read_ud3( conn->control_message_buffer + 8 );
        agmsg->msg.chunk_stream_id = msg->chunk_stream_id;
        agmsg->initialized = true;
        agmsg->processed = agmsg->msg.message_length;

    }
    if( offset < available ){

    }
}*/


static rtmp_err_t rtmp_chunk_conn_service_recv_cmd( rtmp_chunk_conn_t conn, const void *input, size_t available, size_t remaining, rtmp_chunk_stream_message_t *msg ){
    if( available == remaining && remaining == 0 ){
        //The partial command has been aborted.
        return RTMP_GEN_ERROR(conn, RTMP_ERR_NONE);
    }

    //Clamp the length to the size of our control message buffer.
    if( available + conn->control_message_len >= RTMP_CONTROL_BUFFER_SIZE ){
        available = RTMP_CONTROL_BUFFER_SIZE - conn->control_message_len;
    }

    memcpy( conn->control_message_buffer + conn->control_message_len, input, available );
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
    else if( available == 0 ){
        return RTMP_GEN_ERROR(conn, RTMP_ERR_INVALID);
    }
    return RTMP_GEN_ERROR(conn, RTMP_ERR_NONE);
}

static rtmp_err_t rtmp_chunk_conn_service_recv_issue(
    rtmp_chunk_conn_t conn,
    const void *input,
    rtmp_chunk_stream_message_internal_t *previous,
    rtmp_chunk_stream_message_t *msg,
    size_t available ){

    rtmp_err_t ret = RTMP_ERR_NONE;

    //Remove what we're about to process from the pending amount
    previous->processed -= available;
    size_t remaining = previous->processed;

    if( msg->chunk_stream_id == RTMP_CONTROL_CHUNK_STREAM && msg->message_stream_id == RTMP_CONTROL_MSG_STREAM ){
        //This is a control message; use our internal handler
        ret = rtmp_chunk_conn_service_recv_cmd( conn, input, available, remaining, msg );
    }
    else{
        //Relegate processing to the user provided callback
        ret = rtmp_chunk_conn_call_chunk( conn, input, available, remaining, msg );
    }

    ringbuffer_commit_read( conn->in, available );

    return RTMP_GEN_ERROR(conn, ret);
}

static rtmp_err_t rtmp_chunk_conn_service_recv_partial( rtmp_chunk_conn_t conn, rtmp_io_t io_status ){
    //Continue processing a chunk that didn't completely transfer.
    //Fetch the message headers from the previous attempt to process this message.
    rtmp_chunk_stream_message_internal_t *previous = rtmp_cache_get( conn->stream_cache_in, conn->partial_msg );
    rtmp_chunk_stream_message_t *msg = &previous->msg;

    //If for whatever reason we failed to acquire them, abort.
    if( msg == nullptr || previous == nullptr ){
        return RTMP_GEN_ERROR(conn, RTMP_ERR_INADEQUATE_CHUNK);
    }

    size_t length;
    const void *input = ringbuffer_get_read_buf( conn->in, &length );
    //If the remaining bits of the chunk exceed the size of our buffer, simply trim off what we can.
    if( conn->partial_chunk > length ){
        conn->partial_chunk -= length;
    }
    //Otherwise, prepare to read the rest.
    else{
        length = conn->partial_chunk;
        conn->partial_chunk = 0;
    }

    //Issue callbacks
    return rtmp_chunk_conn_service_recv_issue( conn, input, previous, msg, length );
}

static rtmp_err_t rtmp_chunk_conn_service_recv( rtmp_chunk_conn_t conn, rtmp_io_t io_status ){
    //If we have a partial chunk continuation, go process it as such.
    if( conn->partial_chunk > 0 ){
        return rtmp_chunk_conn_service_recv_partial( conn, io_status );
    }

    rtmp_chunk_stream_message_t *msg;
    FAIL_IF_ERR( rtmp_chunk_read_hdr( conn->in, &msg, conn->stream_cache_in ) );

    //Get the previous header from the cache, and fail if we can't.
    rtmp_chunk_stream_message_internal_t *previous = rtmp_cache_get( conn->stream_cache_in, msg->chunk_stream_id );
    if( previous == nullptr ){
        return RTMP_GEN_ERROR(conn, RTMP_ERR_INADEQUATE_CHUNK);
    }

    size_t available = msg->message_length;
    size_t remaining = previous->processed;
    if( remaining == 0 ){
        //This is the first chunk of a message, set processing to the number of remaining bytes
        previous->processed = available;
    }
    else{
        //This is not the first chunk of a message. Change the bytes available to reflect this.
        available = remaining;
    }
    if( available > conn->peer_chunk_size ){
        //The number of bytes available exceeds the chunk size. Trim available to fit.
        available = conn->peer_chunk_size;
    }

    //If we don't have the whole chunk buffered, store the amount still needed for continuing the chunk later
    size_t length;
    const void *input = ringbuffer_get_read_buf( conn->in, &length );
    if( available > length ){
        conn->partial_chunk = available - length;
        available = length;
        //Store the message ID
        conn->partial_msg = msg->chunk_stream_id;
    }

    return rtmp_chunk_conn_service_recv_issue( conn, input, previous, msg, available );
}

rtmp_err_t rtmp_chunk_conn_service( rtmp_chunk_conn_t conn ){
    size_t committed = 0;
    rtmp_err_t ret = RTMP_ERR_NONE;
    rtmp_io_t io_status = 0;
    //Repeatedly service the connection until the buffers don't change
    do{
        bool shaking = ( conn->status & RTMP_STATUS_SHAKING_DONE ) != RTMP_STATUS_SHAKING_DONE;
        //If the in buffer has data, service input
        if( ringbuffer_count( conn->in ) > 0 ){
            io_status = RTMP_IO_IN;
        }
        //Otherwise if the out buffer has data or if we're shaking, service output
        else if( ringbuffer_count( conn->out ) > 0 || shaking ){
            io_status = RTMP_IO_OUT;
        }
        else{
            return RTMP_GEN_ERROR(conn, RTMP_ERR_NONE);
        }

        //Freeze our relevant I/O
        if( io_status == RTMP_IO_IN ){
            ringbuffer_freeze_read( conn->in );
        }
        if( io_status == RTMP_IO_OUT ){
            ringbuffer_freeze_write( conn->out );
        }

        if( shaking ){
            ret = rtmp_chunk_conn_service_shake( conn, io_status );
        }
        else if( io_status & RTMP_IO_IN ){
            ret = rtmp_chunk_conn_service_recv(conn, io_status );
        }

        bool commit = true;
        if( ret >= RTMP_ERR_ERROR ){
            commit = false;
        }

        //Unfreeze I/O and advance our byte counters by the amount the unfreeze states we committed
        if( io_status == RTMP_IO_IN ){
            committed = ringbuffer_unfreeze_read( conn->in, commit );
            conn->bytes_in += committed;
        }
        if( io_status == RTMP_IO_OUT ){
            committed = ringbuffer_unfreeze_write( conn->out, commit );
            conn->bytes_out += committed;
        }

        //Repeat if we didn't error and we've committed something
    } while( committed > 0 && ret == RTMP_ERR_NONE );
    return RTMP_GEN_ERROR(conn, ret);
}


rtmp_err_t rtmp_chunk_conn_register_callbacks( rtmp_chunk_conn_t conn, rtmp_chunk_proc chunk_cb, rtmp_event_proc event_cb, rtmp_log_proc log_cb, void *user ){
    conn->callback_chunk = chunk_cb;
    conn->callback_event = event_cb;
    conn->callback_log = log_cb;
    conn->userdata = user;
    return RTMP_GEN_ERROR(conn, RTMP_ERR_NONE);
}

rtmp_err_t rtmp_chunk_conn_set_chunk_size( rtmp_chunk_conn_t conn, uint32_t size ){
    size &= ~(1 << 31);
    if( size < RTMP_MIN_CHUNK_SIZE ){
        size = RTMP_MIN_CHUNK_SIZE;
    }
    if( size > RTMP_MAX_CHUNK_SIZE ){
        size = RTMP_MAX_CHUNK_SIZE;
    }
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
            sizeof( buffer ),
            nullptr );

    if( err < RTMP_ERR_ERROR ){
        conn->self_chunk_size = size;
    }

    return err;
}

rtmp_err_t rtmp_chunk_conn_abort( rtmp_chunk_conn_t conn, uint32_t chunk_stream ){
    byte buffer[4];
    ntoh_write_ud( buffer, chunk_stream );

    return rtmp_chunk_conn_send_message(
        conn,
        RTMP_MSG_ABORT,
        RTMP_CONTROL_CHUNK_STREAM,
        RTMP_CONTROL_MSG_STREAM,
        0,
        buffer,
        sizeof( buffer ),
        nullptr );
}

rtmp_err_t rtmp_chunk_conn_acknowledge( rtmp_chunk_conn_t conn ){
    byte buffer[4];
    ntoh_write_ud( buffer, conn->bytes_in );

    rtmp_err_t ret = rtmp_chunk_conn_send_message(
        conn,
        RTMP_MSG_ACK,
        RTMP_CONTROL_CHUNK_STREAM,
        RTMP_CONTROL_MSG_STREAM,
        0,
        buffer,
        sizeof( buffer ),
        nullptr );

    if( ret < RTMP_ERR_ERROR ){
        conn->bytes_in = 0;
    }
    return RTMP_GEN_ERROR(conn, ret);
}

rtmp_err_t rtmp_chunk_conn_set_window_ack_size( rtmp_chunk_conn_t conn, uint32_t size ){
    if( size < RTMP_MIN_WINDOW_ACK_SIZE ){
        size = RTMP_MIN_WINDOW_ACK_SIZE;
    }

    byte buffer[4];
    ntoh_write_ud( buffer, size );

    rtmp_err_t ret = rtmp_chunk_conn_send_message(
        conn,
        RTMP_MSG_WIN_ACK_SIZE,
        RTMP_CONTROL_CHUNK_STREAM,
        RTMP_CONTROL_MSG_STREAM,
        0,
        buffer,
        sizeof( buffer ),
        nullptr );

    if( ret < RTMP_ERR_ERROR ){
        conn->self_window_size = size;
    }
    return RTMP_GEN_ERROR(conn, ret);
}

rtmp_err_t rtmp_chunk_conn_set_peer_bwidth( rtmp_chunk_conn_t conn, uint32_t size, rtmp_limit_t limit_type ){
    if( size < RTMP_MIN_PEER_WINDOW_SIZE ){
        size = RTMP_MIN_PEER_WINDOW_SIZE;
    }
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
        sizeof( buffer ),
        nullptr );
}


rtmp_err_t
rtmp_chunk_conn_send_message(
    rtmp_chunk_conn_t conn,
    byte message_type,
    uint32_t chunk_stream,
    uint32_t message_stream,
    uint32_t timestamp,
    const byte * restrict data,
    size_t length,
    size_t *written_out
){
    rtmp_err_t ret = RTMP_ERR_NONE;
    rtmp_chunk_stream_message_t msg;
    msg.chunk_stream_id = chunk_stream;
    msg.message_stream_id = message_stream;
    msg.message_length = length;
    msg.timestamp = timestamp;
    msg.message_type = message_type;

    //We absolutely must have enough space in the output buffer for a whole chunk
    if( conn->self_chunk_size + 20 > ringbuffer_size( conn->out ) ){
        ringbuffer_resize( conn->out, conn->self_chunk_size + 20 );
    }

    //If written_out should contain a number which indicates how far into a write we are.
    size_t written = 0;
    if( written_out ){
        written = *written_out;
    }
    else{
        //We need to be able to roll back the whole loop
        ringbuffer_freeze_write( conn->out );
    }
    while( written < length ){
        size_t chunk_len = length - written;
        //Make sure we only write one chunk at a time
        if( chunk_len > conn->self_chunk_size ){
            chunk_len = conn->self_chunk_size;
        }
        if( written_out ){
            ringbuffer_freeze_write( conn->out );
        }
        ret = rtmp_chunk_emit_hdr( conn->out, &msg, conn->stream_cache_out );
        if( ret >= RTMP_ERR_ERROR ){
            //Rollback on fail
            break;
        }
        if( ringbuffer_copy_write( conn->out, data + written, chunk_len ) < chunk_len ){
            //Rollback on fail
            ret = RTMP_ERR_AGAIN;
            break;
        }
        //Commit the write
        if( written_out ){
            conn->bytes_out += ringbuffer_unfreeze_write( conn->out, true );
        }
        written += chunk_len;
    }
    if( ret >= RTMP_ERR_ERROR ){
        //Don't commit since we failed the write operation.
        ringbuffer_unfreeze_write( conn->out, false );
    }
    if( written_out ){
        *written_out = written;
    }
    else {
        //If we errored, the previous unfreeze should make this unfreeze return 0
        conn->bytes_out += ringbuffer_unfreeze_write( conn->out, true );
    }
    return RTMP_GEN_ERROR(conn, ret);
}

rtmp_err_t rtmp_chunk_conn_get_in_buff( rtmp_chunk_conn_t conn, void **buffer, size_t *size ){
    *buffer = ringbuffer_get_write_buf( conn->in, size );
    return RTMP_GEN_ERROR(conn, RTMP_ERR_NONE);
}

rtmp_err_t rtmp_chunk_conn_get_out_buff( rtmp_chunk_conn_t conn, const void **buffer, size_t *size ){
    *buffer = ringbuffer_get_read_buf( conn->out, size );
    return RTMP_GEN_ERROR(conn, RTMP_ERR_NONE);
}

rtmp_err_t rtmp_chunk_conn_commit_in_buff( rtmp_chunk_conn_t conn, size_t size ){
    ringbuffer_commit_write( conn->in, size );
    return RTMP_GEN_ERROR(conn, RTMP_ERR_NONE);
}

rtmp_err_t rtmp_chunk_conn_commit_out_buff( rtmp_chunk_conn_t conn, size_t size ){
    ringbuffer_commit_read( conn->out, size );
    return RTMP_GEN_ERROR(conn, RTMP_ERR_NONE);
}

rtmp_err_t rtmp_chunk_conn_gen_error(rtmp_chunk_conn_t conn, rtmp_err_t err, size_t line, const char *file, const char *msg){
    #if RTMP_LOG_LEVEL >= 1
    if( !conn->callback_log ){
        return err;
    }
    switch( err ){
        case RTMP_ERR_INADEQUATE_CHUNK:
        case RTMP_ERR_INVALID:
        case RTMP_ERR_OOM:
            conn->callback_log( err, line, file, msg, conn->userdata );
            return err;
        #if RTMP_LOG_LEVEL >= 2
        case RTMP_ERR_BAD_READ:
        case RTMP_ERR_BAD_WRITE:
        case RTMP_ERR_ERROR:
        case RTMP_ERR_DIVERGENCE_METER_ERROR:
        #endif
        #if RTMP_LOG_LEVEL >= 3
        case RTMP_ERR_AGAIN:
        #endif
        #if RTMP_LOG_LEVEL >= 4
        case RTMP_ERR_NOT_READY:
        #endif
            conn->callback_log( err, line, file, msg, conn->userdata );
        default:
            break;
    }
    #endif
    return err;
}
