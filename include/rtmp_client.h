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


rtmp_client_t rtmp_client_create( const char * url, const char * playpath );
void rtmp_client_destroy( rtmp_client_t client );

rtmp_err_t rtmp_client_disconnect( rtmp_client_t client );
bool rtmp_client_connected( rtmp_client_t client );

rtmp_stream_t rtmp_client_stream( rtmp_client_t client );

rtmp_err_t rtmp_client_get_conninfo( rtmp_client_t client, const char **host, uint16_t * port );
const char * rtmp_client_get_playpath( rtmp_client_t client );

//See NetConnect documentation for usage

rtmp_err_t rtmp_client_connect(
    rtmp_client_t client,
    const char * restrict app,
    const char * swfUrl,
    const char * tcUrl,
    size_t audioCodecs,
    size_t videoCodecs,
    rtmp_stream_amf_proc proc,
    void *userdata
);

rtmp_err_t rtmp_client_releasestream(
    rtmp_client_t client,
    const char * path,
    rtmp_stream_amf_proc proc,
    void *userdata
);

rtmp_err_t rtmp_client_fcpublish(
    rtmp_client_t client,
    const char * path,
    rtmp_stream_amf_proc proc,
    void *userdata
);

rtmp_err_t rtmp_client_createstream(
    rtmp_client_t client,
    rtmp_stream_amf_proc proc,
    void *userdata
);

rtmp_err_t rtmp_client_publish(
    rtmp_client_t client,
    size_t streamid,
    const char * playpath,
    const char * type,
    rtmp_stream_amf_proc proc,
    void *userdata
);

rtmp_err_t rtmp_client_setdataframe(
    rtmp_client_t client,
    size_t streamid,
    const char * frame_name,
    double duration,
    double size,
    uint32_t width,
    uint32_t height,
    const char * vid_codecid,
    double vid_data_rate,
    double framerate,
    const char * aud_codecid,
    double aud_data_rate,
    double aud_sample_rate,
    double aud_sample_size,
    uint32_t aud_channels,
    const char * encoder
);

rtmp_err_t rtmp_client_setdataframe_amf(
    rtmp_client_t client,
    size_t streamid,
    amf_t object
);


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

rtmp_err_t rtmp_client_seek( rtmp_client_t client,
    rtmp_time_t seek_ms);

rtmp_err_t rtmp_client_pause( rtmp_client_t client,
    bool paused,
    rtmp_time_t pause_ms);


#endif
