/*
    rtmp_stream.h

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

#include "rtmp/chunk/rtmp_chunk_conn.h"
#include "rtmp/chunk/rtmp_chunk_assembler.h"
#include "amf/amf.h"

typedef struct rtmp_stream * rtmp_stream_t;

typedef rtmp_cb_status_t (*rtmp_stream_amf_proc)(
    rtmp_stream_t stream,
    rtmp_message_type_t message,
    amf_t object,
    void *user
);

typedef rtmp_cb_status_t (*rtmp_stream_msg_proc)(
    rtmp_stream_t stream,
    rtmp_message_type_t message,
    byte *data,
    size_t length,
    size_t remaining,
    void *user
);

typedef rtmp_cb_status_t (*rtmp_stream_usr_proc)(
    rtmp_stream_t stream,
    rtmp_usr_evt_t message,
    uint32_t param1,
    uint32_t param2,
    void *user
);



rtmp_stream_t rtmp_stream_create( bool client );
void rtmp_stream_destroy( rtmp_stream_t stream );

rtmp_err_t rtmp_stream_reg_amf( rtmp_stream_t stream, rtmp_message_type_t type, const char *name, rtmp_stream_amf_proc proc, void *user );
rtmp_err_t rtmp_stream_reg_msg( rtmp_stream_t stream, rtmp_message_type_t type, rtmp_stream_msg_proc proc, void *user );
rtmp_err_t rtmp_stream_reg_usr( rtmp_stream_t stream, rtmp_usr_evt_t type, rtmp_stream_usr_proc proc, void *user );

rtmp_chunk_conn_t rtmp_stream_get_conn( rtmp_stream_t stream );