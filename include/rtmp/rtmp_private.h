/*
    rtmp_private.h

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
#include "rtmp_constants.h"
#include "chunk/rtmp_chunk_conn.h"

struct rtmp_chunk_stream_message_internal{
    rtmp_chunk_stream_message_t msg;
    rtmp_time_t time_delta;
    uint32_t processed;
    bool initialized;
};

struct rtmp_chunk_stream_cache{
    rtmp_chunk_stream_message_internal_t static_cache[RTMP_STREAM_STATIC_CACHE_SIZE];
    rtmp_chunk_stream_message_internal_t *dynamic_cache;
    size_t dynamic_cache_size;
};

struct rtmp_chunk_conn {
    ringbuffer_t in, out;
    rtmp_chunk_stream_cache_t stream_cache_out;
    rtmp_chunk_stream_cache_t stream_cache_in;

    size_t bytes_out, bytes_in;
    size_t partial_chunk;
    size_t partial_msg;

    void *nonce_c, *nonce_s;
    rtmp_time_t self_time, peer_time, peer_shake_recv_time, self_shake_recv_time;

    rtmp_time_t lag;
    byte control_message_buffer[RTMP_CONTROL_BUFFER_SIZE];
    int control_message_len;

    rtmp_chunk_proc callback_chunk;
    rtmp_event_proc callback_event;
    rtmp_log_proc callback_log;
    void *userdata;
    rtmp_chunk_conn_status_t status;

    uint32_t self_chunk_size;
    uint32_t peer_chunk_size;
    uint32_t self_window_size;
    uint32_t peer_window_size;
    uint32_t peer_acknowledged;
    rtmp_limit_t peer_bandwidth_type;
};

