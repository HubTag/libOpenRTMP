/*
    rtmp_app.h

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

#ifndef RTMP_H_APP_H
#define RTMP_H_APP_H

#ifdef __cplusplus
extern "C" {
#endif


typedef struct rtmp_app * rtmp_app_t;
typedef struct rtmp_app_list * rtmp_app_list_t;


#include <openrtmp/rtmp/rtmp_types.h>
#include <openrtmp/rtmp.h>

typedef rtmp_cb_status_t (*rtmp_app_on_amf_proc)(
    rtmp_stream_t stream,
    rtmp_app_t app,
    amf_t args,
    void *user
);
typedef size_t (*rtmp_app_on_fcpub_proc)(
    rtmp_stream_t stream,
    rtmp_app_t app,
    const char * name,
    char * new_name,
    size_t new_name_len,
    void *user
);
typedef size_t (*rtmp_app_on_pub_proc)(
    rtmp_stream_t stream,
    rtmp_app_t app,
    const char * name,
    const char * type,
    char * new_name,
    size_t new_name_len,
    void *user
);
typedef rtmp_cb_status_t (*rtmp_app_on_video_proc)(
    rtmp_stream_t stream,
    rtmp_app_t app,
    void *user
);
typedef rtmp_cb_status_t (*rtmp_app_on_av_proc)(
    rtmp_stream_t stream,
    rtmp_app_t app,
    size_t streamid,
    rtmp_time_t timestamp,
    const void * av_data,
    size_t av_length,
    bool final_part,
    void *user
);


rtmp_app_list_t rtmp_app_list_create( void );
void rtmp_app_list_destroy( rtmp_app_list_t list );

rtmp_app_t rtmp_app_list_register( rtmp_app_list_t list, const char *appname );
rtmp_app_t rtmp_app_list_get( rtmp_app_list_t list, const char *appname );

rtmp_cb_status_t rtmp_app_set_connect( rtmp_app_t app, rtmp_app_on_amf_proc proc, void *user );
rtmp_cb_status_t rtmp_app_set_disconnect( rtmp_app_t app, rtmp_app_on_amf_proc proc, void *user );
rtmp_cb_status_t rtmp_app_set_release( rtmp_app_t app, rtmp_app_on_amf_proc proc, void *user );
rtmp_cb_status_t rtmp_app_set_fcpublish( rtmp_app_t app, rtmp_app_on_fcpub_proc proc, void *user );
rtmp_cb_status_t rtmp_app_set_publish( rtmp_app_t app, rtmp_app_on_pub_proc proc, void *user );
rtmp_cb_status_t rtmp_app_set_metadata( rtmp_app_t app, rtmp_app_on_amf_proc proc, void *user );
rtmp_cb_status_t rtmp_app_set_video( rtmp_app_t app, rtmp_app_on_av_proc proc, void *user );
rtmp_cb_status_t rtmp_app_set_audio( rtmp_app_t app, rtmp_app_on_av_proc proc, void *user );

rtmp_cb_status_t rtmp_app_connect( rtmp_stream_t stream, rtmp_app_t app, amf_t params );
rtmp_cb_status_t rtmp_app_disconnect( rtmp_stream_t stream, rtmp_app_t app, amf_t params  );
rtmp_cb_status_t rtmp_app_release( rtmp_stream_t stream, rtmp_app_t app, amf_t params  );
size_t rtmp_app_fcpublish( rtmp_stream_t stream, rtmp_app_t app, const char * name, char * new_name, size_t len );
size_t rtmp_app_publish( rtmp_stream_t stream, rtmp_app_t app, const char * name, const char * type, char * new_name, size_t len );
rtmp_cb_status_t rtmp_app_metadata( rtmp_stream_t stream, rtmp_app_t app, amf_t params  );
rtmp_cb_status_t rtmp_app_video( rtmp_stream_t stream, rtmp_app_t app, size_t streamid, rtmp_time_t timestamp, const void * av_data, size_t av_length, bool final_part );
rtmp_cb_status_t rtmp_app_audio( rtmp_stream_t stream, rtmp_app_t app, size_t streamid, rtmp_time_t timestamp, const void * av_data, size_t av_length, bool final_part );

#ifdef __cplusplus
}
#endif

#endif
