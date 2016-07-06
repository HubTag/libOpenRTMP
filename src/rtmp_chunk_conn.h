/*
    rtmp_chunk_conn.h

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

#pragma once

#include "rtmp_types.h"
#include "data_stream.h"
#include "rtmp_constants.h"
#include "rtmp_chunk_flow.h"

#define RTMP_STREAM_CACHE_MAX 10

typedef struct rtmp_chunk_conn *rtmp_chunk_conn_t;

typedef rtmp_cb_status_t (*rtmp_chunk_proc)(
    rtmp_chunk_conn_t conn,
    ors_data_t contents,
    unsigned int chunk_stream_id,
    unsigned int message_stream_id,
    unsigned int timestamp,
    byte message_type,
    void *user
);

typedef rtmp_cb_status_t (*rtmp_event_proc)(
    rtmp_chunk_conn_t conn,
    rtmp_event_t event,
    void *user
);

struct rtmp_chunk_conn {
    ors_data_t inflow, outflow;
    rtmp_chunk_stream_message_t stream_cache[RTMP_STREAM_CACHE_MAX];

    void *nonce_c, *nonce_s;
    rtmp_time_t self_time, peer_time, peer_shake_recv_time, self_shake_recv_time;
    unsigned int lag;

    rtmp_chunk_proc callback_chunk;
    rtmp_event_proc callback_event;
    void *userdata;
    rtmp_chunk_conn_status_t status;

    unsigned int self_chunk_size;
    unsigned int peer_chunk_size;
    unsigned int self_window_size;
    unsigned int peer_window_size;
    rtmp_limit_t peer_bandwidth_type;

    unsigned int bytes_recvd;
};


rtmp_chunk_conn_t rtmp_chunk_conn_create( bool client, ors_data_t inflow, ors_data_t outflow );
rtmp_err_t rtmp_chunk_conn_close( rtmp_chunk_conn_t conn, bool close_pipes );

rtmp_err_t rtmp_chunk_conn_service( rtmp_chunk_conn_t conn, rtmp_io_t io_status );

rtmp_err_t rtmp_chunk_conn_register_callbacks( rtmp_chunk_conn_t conn, rtmp_chunk_proc chunk_cb, rtmp_event_proc event_cb, void *user );



rtmp_err_t rtmp_chunk_conn_set_chunk_size( rtmp_chunk_conn_t conn, unsigned int size );
rtmp_err_t rtmp_chunk_conn_abort( rtmp_chunk_conn_t conn, unsigned int chunk_stream );
rtmp_err_t rtmp_chunk_conn_acknowledge( rtmp_chunk_conn_t conn );
rtmp_err_t rtmp_chunk_conn_set_window_ack_size( rtmp_chunk_conn_t conn, unsigned int size );
rtmp_err_t rtmp_chunk_conn_set_peer_bwidth( rtmp_chunk_conn_t conn, unsigned int size, rtmp_limit_t limit_type );
