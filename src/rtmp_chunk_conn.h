/*
    rtmp_chunk_conn.h

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

#pragma once

#include "rtmp_types.h"
#include "data_stream.h"
#include "rtmp_constants.h"
#include "rtmp_chunk_flow.h"


typedef struct rtmp_chunk_conn *rtmp_chunk_conn_t;

typedef rtmp_cb_status_t (*rtmp_chunk_proc)(
    rtmp_chunk_conn_t conn,
    byte *contents,
    size_t available,
    size_t remaining,
    rtmp_chunk_stream_message_t *msg,
    void *user
);

typedef rtmp_cb_status_t (*rtmp_event_proc)(
    rtmp_chunk_conn_t conn,
    rtmp_event_t event,
    void *user
);

struct rtmp_chunk_conn {
    ringbuffer_t in, out;
    rtmp_chunk_stream_cache_t stream_cache_out;
    rtmp_chunk_stream_cache_t stream_cache_in;

    size_t bytes_out, bytes_in;
    size_t partial_chunk;
    size_t *partial_msg;
    size_t *partial_msg_p;

    void *nonce_c, *nonce_s;
    rtmp_time_t self_time, peer_time, peer_shake_recv_time, self_shake_recv_time;

    unsigned int lag;
    byte control_message_buffer[RTMP_CONTROL_BUFFER_SIZE];
    int control_message_len;

    rtmp_chunk_proc callback_chunk;
    rtmp_event_proc callback_event;
    void *userdata;
    rtmp_chunk_conn_status_t status;

    unsigned int self_chunk_size;
    unsigned int peer_chunk_size;
    unsigned int self_window_size;
    unsigned int peer_window_size;
    unsigned int peer_acknowledged;
    rtmp_limit_t peer_bandwidth_type;
};


rtmp_chunk_conn_t rtmp_chunk_conn_create( bool client );
rtmp_err_t rtmp_chunk_conn_close( rtmp_chunk_conn_t conn );

rtmp_err_t rtmp_chunk_conn_service( rtmp_chunk_conn_t conn );

rtmp_err_t rtmp_chunk_conn_register_callbacks( rtmp_chunk_conn_t conn, rtmp_chunk_proc chunk_cb, rtmp_event_proc event_cb, void *user );

bool rtmp_chunk_conn_connected( rtmp_chunk_conn_t conn );

rtmp_err_t rtmp_chunk_conn_set_chunk_size( rtmp_chunk_conn_t conn, unsigned int size );
rtmp_err_t rtmp_chunk_conn_abort( rtmp_chunk_conn_t conn, unsigned int chunk_stream );
rtmp_err_t rtmp_chunk_conn_acknowledge( rtmp_chunk_conn_t conn );
rtmp_err_t rtmp_chunk_conn_set_window_ack_size( rtmp_chunk_conn_t conn, unsigned int size );
rtmp_err_t rtmp_chunk_conn_set_peer_bwidth( rtmp_chunk_conn_t conn, unsigned int size, rtmp_limit_t limit_type );

rtmp_err_t rtmp_chunk_conn_get_in_buff( rtmp_chunk_conn_t conn, void **buffer, size_t *size );
rtmp_err_t rtmp_chunk_conn_get_out_buff( rtmp_chunk_conn_t conn, const void **buffer, size_t *size );
rtmp_err_t rtmp_chunk_conn_commit_in_buff( rtmp_chunk_conn_t conn, size_t size );
rtmp_err_t rtmp_chunk_conn_commit_out_buff( rtmp_chunk_conn_t conn, size_t size );



rtmp_err_t
rtmp_chunk_conn_send_message(
    rtmp_chunk_conn_t conn,
    byte message_type,
    unsigned int chunk_stream,
    unsigned int message_stream,
    unsigned int timestamp,
    byte *data,
    size_t length,
    size_t *written
);
