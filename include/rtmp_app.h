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

#pragma once

typedef struct rtmp_app * rtmp_app_t;
typedef struct rtmp_app_list * rtmp_app_list_t;


#include "rtmp/rtmp_types.h"
#include "rtmp.h"

typedef rtmp_cb_status_t (*rtmp_app_on_connect_proc)(
    rtmp_stream_t stream,
    rtmp_app_t app,
    rtmp_params_t params,
    void *user
);
typedef rtmp_cb_status_t (*rtmp_app_on_disconnect_proc)(
    rtmp_stream_t stream,
    rtmp_app_t app,
    void *user
);
typedef rtmp_cb_status_t (*rtmp_app_on_release_proc)(
    rtmp_stream_t stream,
    rtmp_app_t app,
    void *user
);
typedef rtmp_cb_status_t (*rtmp_app_on_publish_proc)(
    rtmp_stream_t stream,
    rtmp_app_t app,
    void *user
);
typedef rtmp_cb_status_t (*rtmp_app_on_metadata_proc)(
    rtmp_stream_t stream,
    rtmp_app_t app,
    void *user
);
typedef rtmp_cb_status_t (*rtmp_app_on_video_proc)(
    rtmp_stream_t stream,
    rtmp_app_t app,
    void *user
);
typedef rtmp_cb_status_t (*rtmp_app_on_audio_proc)(
    rtmp_stream_t stream,
    rtmp_app_t app,
    void *user
);

typedef enum{
    RTMP_APP_ON_CONNECT,
    RTMP_APP_ON_DISCONNECT,
    RTMP_APP_ON_RELEASE,
    RTMP_APP_ON_PUBLISH,
    RTMP_APP_ON_METADATA,
    RTMP_APP_ON_VIDEO,
    RTMP_APP_ON_AUDIO
} rtmp_app_field_t;

rtmp_app_list_t rtmp_app_list_create( void );
void rtmp_app_list_destroy( rtmp_app_list_t list );

rtmp_app_t rtmp_app_list_register( rtmp_app_list_t list, const char *appname );
rtmp_app_t rtmp_app_list_get( rtmp_app_list_t list, const char *appname );

rtmp_err_t rtmp_app_set( rtmp_app_t app, rtmp_app_field_t field, void * proc, void * userdata );

rtmp_cb_status_t rtmp_app_set_connect( rtmp_app_t app, rtmp_app_on_connect_proc proc, void *user );
rtmp_cb_status_t rtmp_app_set_disconnect( rtmp_app_t app, rtmp_app_on_disconnect_proc proc, void *user );
rtmp_cb_status_t rtmp_app_set_release( rtmp_app_t app, rtmp_app_on_release_proc proc, void *user );
rtmp_cb_status_t rtmp_app_set_publish( rtmp_app_t app, rtmp_app_on_publish_proc proc, void *user );
rtmp_cb_status_t rtmp_app_set_metadata( rtmp_app_t app, rtmp_app_on_metadata_proc proc, void *user );
rtmp_cb_status_t rtmp_app_set_video( rtmp_app_t app, rtmp_app_on_video_proc proc, void *user );
rtmp_cb_status_t rtmp_app_set_audio( rtmp_app_t app, rtmp_app_on_audio_proc proc, void *user );

rtmp_cb_status_t rtmp_app_connect( rtmp_stream_t stream, rtmp_app_t app, rtmp_params_t params );
rtmp_cb_status_t rtmp_app_disconnect( rtmp_stream_t stream, rtmp_app_t app );
rtmp_cb_status_t rtmp_app_release( rtmp_stream_t stream, rtmp_app_t app );
rtmp_cb_status_t rtmp_app_publish( rtmp_stream_t stream, rtmp_app_t app );
rtmp_cb_status_t rtmp_app_metadata( rtmp_stream_t stream, rtmp_app_t app );
rtmp_cb_status_t rtmp_app_video( rtmp_stream_t stream, rtmp_app_t app );
rtmp_cb_status_t rtmp_app_audio( rtmp_stream_t stream, rtmp_app_t app );
