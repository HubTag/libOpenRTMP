/*
    rtmp_client.h

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

#ifndef RTMP_H_CLIENT_H
#define RTMP_H_CLIENT_H

typedef struct rtmp_client * rtmp_client_t;

#include "rtmp/rtmp_types.h"
#include "rtmp/rtmp_stream.h"
#include "rtmp.h"


rtmp_client_t rtmp_client_create( void );
void rtmp_client_destroy( rtmp_client_t client );

rtmp_err_t rtmp_client_disconnect( rtmp_client_t client );
bool rtmp_client_connected( rtmp_client_t client );

rtmp_stream_t rtmp_client_stream( rtmp_client_t client );


//See NetConnect documentation for usage


rtmp_err_t rtmp_client_play( rtmp_client_t client,
    const char *stream_name,
    double start DEFAULT(-1),
    double duration DEFAULT(-1),
    bool reset DEFAULT(false) );

rtmp_err_t rtmp_client_play2( rtmp_client_t client,
    const char * old_stream_name,
    const char * new_stream_name,
    double start_time,
    double offset,
    double len,
    const char *restrict transition );

rtmp_err_t rtmp_client_deletestream( rtmp_client_t client,
    double stream_id);

rtmp_err_t rtmp_client_recv_audio( rtmp_client_t client,
    bool want_audio);

rtmp_err_t rtmp_client_recv_video( rtmp_client_t client,
    bool want_video);

rtmp_err_t rtmp_client_publish( rtmp_client_t client,
    const char * restrict publish_name,
    const char * restrict publish_type);

rtmp_err_t rtmp_client_seek( rtmp_client_t client,
    rtmp_time_t seek_ms);

rtmp_err_t rtmp_client_pause( rtmp_client_t client,
    bool paused,
    rtmp_time_t pause_ms);


#endif
