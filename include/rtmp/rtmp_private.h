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

#ifndef RTMP_H_PRIVATE_H
#define RTMP_H_PRIVATE_H

#include "rtmp_types.h"
#include "rtmp_constants.h"
#include "chunk/rtmp_chunk_conn.h"
#include "rtmp/rtmp_stream.h"
#include "rtmp_server.h"
#include "rtmp_client.h"
#include "vec.h"

#if defined RTMP_POLLTECH_EPOLL
#   include <sys/epoll.h>
#elif defined RTMP_POLLTECH_SELECT
#   include <sys/select.h>
#elif defined RTMP_POLLTECH_POLL
#   include <sys/poll.h>
#elif defined RTMP_POLLTECH_WSAPOLL
#   include <winsock2.h>
#endif


typedef enum {
    RTMP_T_RTMP_T,
    RTMP_T_SERVER_T,
    RTMP_T_CLIENT_T
} rtmp_t_t;

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

struct rtmp_params{
    char *app;
    char *flashver;
    char *swfUrl;
    char *tcUrl;
    char *pageUrl;
    uint32_t audioCodecs;
    uint32_t videoCodecs;
    uint32_t videoFunction;
    uint32_t objectEncoding;
    bool fpad;
};

void rtmp_params_free( struct rtmp_params * params );


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

typedef struct rtmp_evt_cb{
    rtmp_event_t type;
    rtmp_stream_evt_proc callback;
    void *user;
} rtmp_evt_cb_t;

typedef struct rtmp_log_cb{
    rtmp_log_proc callback;
    void *user;
} rtmp_log_cb_t;


struct rtmp_stream{
    rtmp_chunk_conn_t connection;
    rtmp_chunk_assembler_t assembler;

    size_t chunk_id, message_id;
    void *scratch;
    size_t scratch_len;

    VEC_DECLARE(rtmp_amf_cb_t) amf_callback;

    VEC_DECLARE(rtmp_msg_cb_t) msg_callback;

    VEC_DECLARE(rtmp_usr_cb_t) usr_callback;

    VEC_DECLARE(rtmp_evt_cb_t) event_callback;

    VEC_DECLARE(rtmp_log_cb_t) log_callback;
};


typedef struct rtmp_mgr_svr{
    rtmp_t_t type;
    rtmp_sock_t socket;
    union{
        rtmp_server_t server;
        rtmp_client_t client;
    };
    int flags;
    rtmp_t mgr;
    bool closing;
} *rtmp_mgr_svr_t;

struct rtmp_mgr {
    rtmp_t_t type;
    VEC_DECLARE(rtmp_mgr_svr_t) servers;
    rtmp_time_t last_refresh;

    union{
        #if defined RTMP_POLLTECH_EPOLL
        struct{
            int epollfd;
        } epoll_args;
        #elif defined RTMP_POLLTECH_SELECT
        struct{
            fd_set rfds, wfds, efds;
        } select_args;
        #elif defined RTMP_POLLTECH_POLL
        struct{

        } poll_args;
        #elif defined RTMP_POLLTECH_WSAPOLL
        struct{

        } wsapoll_args;
        #endif
    };
    rtmp_err_t (*service_cb)( rtmp_t mgr );

    rtmp_sock_t listen_socket;
    rtmp_connect_proc callback;
    void *callback_data;

    rtmp_app_list_t applist;
};

#endif
