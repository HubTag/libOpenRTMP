/*
    memutil.c

    Copyright (C) 2016 Hubtag LLC.

    ----------------------------------------

    This file is part of libOpenRTMP.

    libOpenRTMP is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    libOpenRTMP is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with libOpenRTMP.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "rtmp_chunk_conn.h"
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

    ret->bytes_recvd = 0;

    ret->self_chunk_size = RTMP_DEFAULT_CHUNK_SIZE;
    ret->peer_chunk_size = RTMP_DEFAULT_CHUNK_SIZE;
    ret->self_window_size = RTMP_DEFAULT_WINDOW_SIZE;
    ret->peer_window_size = RTMP_DEFAULT_WINDOW_SIZE;
    ret->peer_bandwidth_type = RTMP_DEFAULT_BANDWIDTH_TYPE;

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
    free( conn );
    return RTMP_ERR_NONE;
}



rtmp_err_t rtmp_chunk_conn_call_event( rtmp_chunk_conn_t conn, rtmp_event_t event ){
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

static rtmp_err_t rtmp_chunk_conn_service_client_emit( rtmp_chunk_conn_t conn ){
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

static rtmp_err_t rtmp_chunk_conn_service_client_ingest( rtmp_chunk_conn_t conn ){
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

static rtmp_err_t rtmp_chunk_conn_service_server_emit( rtmp_chunk_conn_t conn ){
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

static rtmp_err_t rtmp_chunk_conn_service_server_ingest( rtmp_chunk_conn_t conn ){
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
        if( (io_status & RTMP_IO_OUT) && (err = rtmp_chunk_conn_service_client_emit( conn ) ) >= RTMP_ERR_ERROR ){
            return err;
        }
        if( (io_status & RTMP_IO_IN) && (err = rtmp_chunk_conn_service_client_ingest( conn ) ) >= RTMP_ERR_ERROR ){
            return err;
        }
    }
    else{
        if( (io_status & RTMP_IO_OUT) && (err = rtmp_chunk_conn_service_server_emit( conn ) ) >= RTMP_ERR_ERROR ){
            return err;
        }
        if( (io_status & RTMP_IO_IN) && (err = rtmp_chunk_conn_service_server_ingest( conn ) ) >= RTMP_ERR_ERROR ){
            return err;
        }
    }
    return err;
}

rtmp_err_t rtmp_chunk_conn_service( rtmp_chunk_conn_t conn, rtmp_io_t io_status ){
    if( ( conn->status & RTMP_STATUS_SHAKING_DONE ) != RTMP_STATUS_SHAKING_DONE ){
        return rtmp_chunk_conn_service_shake( conn, io_status );
    }

    return RTMP_ERR_NONE;
}


rtmp_err_t rtmp_chunk_conn_register_callbacks( rtmp_chunk_conn_t conn, rtmp_chunk_proc chunk_cb, rtmp_event_proc event_cb, void *user ){
    conn->callback_chunk = chunk_cb;
    conn->callback_event = event_cb;
    conn->userdata = user;
    return RTMP_ERR_NONE;
}

#undef FAIL_IF_ERR
#define FAIL_IF_ERR(a) {rtmp_err_t err; if((err=(a))>=RTMP_ERR_ERROR){ rtmp_chunk_conn_call_event( conn, RTMP_EVENT_FAILED ); return err;}}

static rtmp_err_t rtmp_chunk_conn_emit_number(rtmp_chunk_conn_t conn, rtmp_message_type_t msg_type, unsigned int value){
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
}

rtmp_err_t rtmp_chunk_conn_set_chunk_size( rtmp_chunk_conn_t conn, unsigned int size ){
    size &= ~(1 << 31);
    FAIL_IF_ERR(rtmp_chunk_conn_emit_number( conn, RTMP_MSG_SET_CHUNK_SIZE, size ) );
    conn->self_chunk_size = size;
    return RTMP_ERR_NONE;
}

rtmp_err_t rtmp_chunk_conn_abort( rtmp_chunk_conn_t conn, unsigned int chunk_stream ){
    FAIL_IF_ERR(rtmp_chunk_conn_emit_number( conn, RTMP_MSG_ABORT, chunk_stream ) );
    return RTMP_ERR_NONE;
}

rtmp_err_t rtmp_chunk_conn_acknowledge( rtmp_chunk_conn_t conn ){
    FAIL_IF_ERR(rtmp_chunk_conn_emit_number( conn, RTMP_MSG_ACK, conn->bytes_recvd ) );
    return RTMP_ERR_NONE;
}

rtmp_err_t rtmp_chunk_conn_set_window_ack_size( rtmp_chunk_conn_t conn, unsigned int size ){
    FAIL_IF_ERR(rtmp_chunk_conn_emit_number( conn, RTMP_MSG_WIN_ACK_SIZE, size ) );
    return RTMP_ERR_NONE;
}

rtmp_err_t rtmp_chunk_conn_set_peer_bwidth( rtmp_chunk_conn_t conn, unsigned int size, rtmp_limit_t limit_type ){
    rtmp_chunk_stream_message_t msg;
    msg.chunk_stream_id = RTMP_CONTROL_CHUNK_STREAM;
    msg.message_stream_id = RTMP_CONTROL_MSG_STREAM;
    msg.message_length = 5;
    msg.timestamp = 0;
    msg.message_type = RTMP_MSG_SET_PEER_BWIDTH;
    FAIL_IF_ERR(rtmp_chunk_emit_hdr( conn->outflow, &msg, conn->stream_cache ));
    byte buffer[5];
    ntoh_write_ud( buffer, size );
    buffer[4] = limit_type;
    if( ors_data_write( conn->outflow, buffer, 5 ) < 5 ){
        return RTMP_ERR_BAD_WRITE;
    }
    return RTMP_ERR_NONE;
}
