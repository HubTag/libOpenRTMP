/*
    rtmp.h

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

typedef struct rtmp_mgr * rtmp_t;

#include "rtmp/rtmp_types.h"
#include "amf/amf.h"
#include "rtmp_client.h"
#include "rtmp_server.h"


typedef rtmp_cb_status_t (*rtmp_connect_proc)(
    rtmp_server_t connection,
    void *user;
);

typedef enum {
    RTMP_EPOLL,
    RTMP_POLL,
    RTMP_SELECT
} rtmp_loop_t;


rtmp_t rtmp_create( rtmp_loop_t loop_type );
void rtmp_destroy( rtmp_t mgr );

rtmp_err_t rtmp_service( rtmp_t mgr );

rtmp_client_t rtmp_connect( rtmp_t mgr, const char * url, const char * playpath );

rtmp_err_t rtmp_listen( rtmp_t mgr, unsigned long long iface, short port, rtmp_connect_proc cb, void *user );

rtmp_err_t rtmp_amferr( amf_err_t err );
