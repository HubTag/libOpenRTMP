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

#ifndef RTMP_H_STREAM_H
#define RTMP_H_STREAM_H

#include "rtmp/chunk/rtmp_chunk_conn.h"
#include "rtmp/chunk/rtmp_chunk_assembler.h"
#include "amf/amf.h"
#include "amf/amf_constants.h"
#include "amf/amf_object.h"

typedef struct rtmp_stream * rtmp_stream_t;

typedef struct rtmp_stream_args{
    rtmp_stream_t stream;
    rtmp_message_type_t message;
    rtmp_time_t timestamp;
    size_t message_stream;
} *rtmp_stream_args_t;

typedef rtmp_cb_status_t (*rtmp_stream_amf_proc)(
    rtmp_stream_args_t args,
    amf_t object,
    void *user
);

typedef rtmp_cb_status_t (*rtmp_stream_msg_proc)(
    rtmp_stream_args_t args,
    const byte *data,
    size_t length,
    size_t remaining,
    void *user
);

typedef rtmp_cb_status_t (*rtmp_stream_usr_proc)(
    rtmp_stream_args_t args,
    uint32_t param1,
    uint32_t param2,
    void *user
);

typedef rtmp_cb_status_t (*rtmp_stream_evt_proc)(
    rtmp_stream_t stream,
    rtmp_event_t event,
    void * user
);


rtmp_stream_t rtmp_stream_create( bool client );
void rtmp_stream_create_at( rtmp_stream_t location, bool client );
void rtmp_stream_destroy( rtmp_stream_t stream );
void rtmp_stream_destroy_at( rtmp_stream_t stream );

rtmp_err_t rtmp_stream_reg_amf( rtmp_stream_t stream, rtmp_message_type_t type, const char * restrict name, rtmp_stream_amf_proc proc, void * restrict user );
rtmp_err_t rtmp_stream_reg_msg( rtmp_stream_t stream, rtmp_message_type_t type, rtmp_stream_msg_proc proc, void * restrict user );
rtmp_err_t rtmp_stream_reg_usr( rtmp_stream_t stream, rtmp_usr_evt_t type, rtmp_stream_usr_proc proc, void * restrict user );

rtmp_err_t rtmp_stream_reg_event( rtmp_stream_t stream, rtmp_event_t type, rtmp_stream_evt_proc proc, void * restrict user );
rtmp_err_t rtmp_stream_reg_log( rtmp_stream_t stream, rtmp_log_proc proc, void * restrict user );

rtmp_chunk_conn_t rtmp_stream_get_conn( rtmp_stream_t stream );



void rtmp_stream_set_chunk_stream(          rtmp_stream_t stream, size_t chunk_id );
void rtmp_stream_set_msg_stream(            rtmp_stream_t stream, size_t msg_id );

rtmp_err_t rtmp_stream_send_audio(          rtmp_stream_t stream, rtmp_time_t timestamp, const byte * restrict data, size_t len, size_t *written );
rtmp_err_t rtmp_stream_send_video(          rtmp_stream_t stream, rtmp_time_t timestamp, const byte * restrict data, size_t len, size_t *written );
rtmp_err_t rtmp_stream_send_cmd(            rtmp_stream_t stream, rtmp_time_t timestamp, amf_t amf, size_t *written  );
rtmp_err_t rtmp_stream_send_so(             rtmp_stream_t stream, rtmp_time_t timestamp, amf_t amf, size_t *written  );
rtmp_err_t rtmp_stream_send_dat(            rtmp_stream_t stream, rtmp_time_t timestamp, amf_t amf, size_t *written  );

rtmp_err_t rtmp_stream_send_stream_begin(   rtmp_stream_t stream, uint32_t stream_id );
rtmp_err_t rtmp_stream_send_stream_eof(     rtmp_stream_t stream, uint32_t stream_id );
rtmp_err_t rtmp_stream_send_stream_dry(     rtmp_stream_t stream, uint32_t stream_id );
rtmp_err_t rtmp_stream_send_set_buf_len(    rtmp_stream_t stream, uint32_t stream_id, uint32_t len_ms );
rtmp_err_t rtmp_stream_send_is_recorded(    rtmp_stream_t stream, uint32_t stream_id );
rtmp_err_t rtmp_stream_send_ping_req(       rtmp_stream_t stream, uint32_t ping_time );
rtmp_err_t rtmp_stream_send_ping_res(       rtmp_stream_t stream, uint32_t ping_time );



rtmp_err_t rtmp_stream_send_audio2(         rtmp_stream_t stream, size_t chunk_id, size_t msg_id, rtmp_time_t timestamp, const byte * restrict data, size_t len, size_t *written );
rtmp_err_t rtmp_stream_send_video2(         rtmp_stream_t stream, size_t chunk_id, size_t msg_id, rtmp_time_t timestamp, const byte * restrict data, size_t len, size_t *written );
rtmp_err_t rtmp_stream_send_cmd2(           rtmp_stream_t stream, size_t chunk_id, size_t msg_id, rtmp_time_t timestamp, amf_t amf, size_t *written  );
rtmp_err_t rtmp_stream_send_so2(            rtmp_stream_t stream, size_t chunk_id, size_t msg_id, rtmp_time_t timestamp, amf_t amf, size_t *written  );
rtmp_err_t rtmp_stream_send_dat2(           rtmp_stream_t stream, size_t chunk_id, size_t msg_id, rtmp_time_t timestamp, amf_t amf, size_t *written  );

rtmp_err_t rtmp_stream_call(                rtmp_stream_t stream, const char *name, rtmp_stream_amf_proc callback, void * userdata, ... );
rtmp_err_t rtmp_stream_respond(             rtmp_stream_t stream, const char *name, double id, ... );

rtmp_err_t rtmp_stream_call2(               rtmp_stream_t stream, size_t chunk_id, size_t msg_id, const char *name, rtmp_stream_amf_proc callback, void * userdata, ... );
rtmp_err_t rtmp_stream_respond2(            rtmp_stream_t stream, size_t chunk_id, size_t msg_id, const char *name, double id, ... );

rtmp_err_t rtmp_stream_call2_va(            rtmp_stream_t stream, size_t chunk_id, size_t msg_id, const char *name, rtmp_stream_amf_proc callback, void * userdata, va_list list );

#endif
