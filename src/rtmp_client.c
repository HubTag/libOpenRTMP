/*
    rtmp_client.c

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


#include "rtmp_client.h"
#include "parseurl.h"
#include "rtmp/rtmp_private.h"
#include "rtmp/rtmp_stream.h"
#include "memutil.h"

typedef enum{
    RTMP_FMT_FLV,
    RTMP_FMT_MP4,
} rtmp_fmt_t;

struct rtmp_client{
    struct rtmp_stream stream;
    parseurl_t url;
    char * playpath;
    rtmp_fmt_t fmt;
};

#include "rtmp/rtmp_types.h"
#include "rtmp/rtmp_stream.h"
#include "rtmp.h"

static rtmp_fmt_t mem_to_fmt( const char * start, const char * end ){
    size_t len = end - start;
    if( strncasecmp( start, "flv", len ) == 0 ){
        return RTMP_FMT_FLV;
    }
    if( strncasecmp( start, "mp4", len ) == 0 ){
        return RTMP_FMT_MP4;
    }
    return RTMP_FMT_FLV;
}

rtmp_client_t rtmp_client_create( const char * url, const char * playpath ){
    rtmp_client_t client = ezalloc( client );

    client->url = parseurl_create();
    parseurl_set( client->url, PARSEURL_SCHEME, "rtmp" );
    #define STR(a) STR2(a)
    #define STR2(a) # a
    parseurl_set( client->url, PARSEURL_PORT, STR(RTMP_DEFAULT_PORT) );
    if( !parseurl_set( client->url, PARSEURL_URL_FORGIVING, url ) ){
        goto fail;
    }
    if( strncasecmp("rtmp", parseurl_get(client->url, PARSEURL_SCHEME, "rtmp"), -1 ) != 0 ){
        goto fail;
    }

    if( playpath == nullptr ){
        const char * path = parseurl_get( client->url, PARSEURL_PATH, "/" );
        const char * old_path = path;
        const char * amp = strchr( path, '&' );
        const char * colon = nullptr;
        char * new_path = str_dup( path );

        if( amp ){
            path = amp;
            colon = strchr( path, ':' );
            if( colon ){
                path = colon;
                client->fmt = mem_to_fmt( amp + 1, colon );
            }
        }
        else{
            path = strrchr( path, '/' );
            if( !path ){
                path = "";
            }
        }
        if( path ){
            new_path[path - old_path] = 0;
            parseurl_set( client->url, PARSEURL_PATH, new_path );
        }
        free( new_path );
        client->playpath = str_dup( path + 1 );
    }
    else{
        client->playpath = str_dup( playpath );
    }

    rtmp_stream_create_at( &client->stream, true );
    return client;
    fail:
    parseurl_destroy( client->url );
    free( client );
    return nullptr;
}
void rtmp_client_destroy( rtmp_client_t client ){
    rtmp_stream_destroy_at( &client->stream );
    free( client->playpath);
    parseurl_destroy( client->url );
    free( client );
}

rtmp_err_t rtmp_client_disconnect( rtmp_client_t client );
bool rtmp_client_connected( rtmp_client_t client );

rtmp_stream_t rtmp_client_stream( rtmp_client_t client ){
    return &client->stream;
}

const char * rtmp_client_get_playpath( rtmp_client_t client ){
    return client->playpath;
}

rtmp_err_t rtmp_client_get_conninfo( rtmp_client_t client, const char **host, uint16_t * port ){
    if( client->url ){
        *host = parseurl_get( client->url, PARSEURL_HOST, nullptr );
        *port = atoi( parseurl_get( client->url, PARSEURL_PORT, STR(RTMP_DEFAULT_PORT) ) );
        if( !*host || !*port ){
            return RTMP_GEN_ERROR(RTMP_ERR_ERROR);
        }
    }
    return RTMP_GEN_ERROR(RTMP_ERR_NONE);
}

rtmp_err_t rtmp_client_connect(
    rtmp_client_t client,
    const char * restrict app,
    const char * swfUrl,
    const char * tcUrl,
    size_t audioCodecs,
    size_t videoCodecs,
    rtmp_stream_amf_proc proc,
    void *userdata
){
    if( swfUrl == nullptr ){
        swfUrl = parseurl_get( client->url, PARSEURL_URL, "" );
    }
    if( tcUrl == nullptr ){
        tcUrl = parseurl_get( client->url, PARSEURL_URL, "" );
    }
    rtmp_err_t ret = rtmp_chunk_conn_set_chunk_size( rtmp_stream_get_conn( rtmp_client_stream( client ) ), RTMP_DESIRED_CHUNK_SIZE );

    return ret >= RTMP_ERR_ERROR ? ret : rtmp_stream_call(
        rtmp_client_stream( client ),
        "connect", proc, userdata, AMF(
            AMF_OBJ(
                AMF_STR( "app", app ),
                AMF_STR( "type", "nonprivate" ),
                //AMF_STR( "flashVer", RTMP_FLASHVER_STR ),
                //AMF_STR( "swfUrl", swfUrl ),
                AMF_STR( "tcUrl", tcUrl )//,
                //AMF_INT( "audioCodecs", audioCodecs ),
                //AMF_INT( "videoCodecs", videoCodecs )
            )
    ));
}

rtmp_err_t rtmp_client_releasestream(
    rtmp_client_t client,
    const char * path,
    rtmp_stream_amf_proc proc,
    void *userdata
){
    if( path == nullptr ){
        path = client->playpath;
    }
    return rtmp_stream_call(
        rtmp_client_stream( client ),
        "releaseStream", proc, userdata, AMF(
            AMF_NULL(),
            AMF_STR(path)
    ));
}

rtmp_err_t rtmp_client_fcpublish(
    rtmp_client_t client,
    const char * path,
    rtmp_stream_amf_proc proc,
    void *userdata
){
    if( path == nullptr ){
        path = client->playpath;
    }
    return rtmp_stream_call(
        rtmp_client_stream( client ),
        "FCPublish", proc, userdata, AMF(
            AMF_NULL(),
            AMF_STR(path)
    ));
}

rtmp_err_t rtmp_client_createstream(
    rtmp_client_t client,
    rtmp_stream_amf_proc proc,
    void *userdata
){
    return rtmp_stream_call2(
        rtmp_client_stream( client ), 3, 0,
        "createStream", proc, userdata, AMF(
            AMF_NULL()
    ));
}

rtmp_err_t rtmp_client_publish(
    rtmp_client_t client,
    size_t streamid,
    const char * playpath,
    const char * type,
    rtmp_stream_amf_proc proc,
    void *userdata
){
    if( playpath == nullptr ){
        playpath = client->playpath;
    }
    return rtmp_stream_call2(
        rtmp_client_stream( client ), 0, streamid,
        "publish", proc, userdata, AMF(
            AMF_NULL(),
            AMF_STR( playpath ),
            AMF_STR( type )
    ));
}

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
){
    amf_t amf = amf_create(0);
    if( !amf ){
        return RTMP_GEN_ERROR(RTMP_ERR_OOM);
    }
    amf_err_t err = amf_push_simple( amf, AMF(
            AMF_STR( "@setDataFrame" ),
            AMF_STR( frame_name ),
            AMF_OBJ(
                AMF_DBL("duration", duration ),
                AMF_DBL("fileSize", size ),
                AMF_INT("width", width ),
                AMF_INT("height", height ),
                AMF_STR("videocodecid", vid_codecid ),
                AMF_DBL("videodatarate", vid_data_rate ),
                AMF_DBL("framerate", framerate ),
                AMF_STR("audiocodecid", aud_codecid ),
                AMF_DBL("audiodatarate", aud_data_rate ),
                AMF_DBL("audiosamplerate", aud_sample_rate ),
                AMF_DBL("audiosamplesize", aud_sample_size ),
                AMF_INT("audiochannels", aud_channels ),
                AMF_BOOL("stereo", aud_channels == 2 ),
                AMF_STR("encoder", encoder )
            )
    ));
    amf_print( amf );

    rtmp_err_t ret =    err ? rtmp_amferr( err ) :
                        rtmp_stream_send_dat2( rtmp_client_stream( client ), 3, streamid, 0, amf, nullptr );

    amf_destroy( amf );
    return ret;
}
rtmp_err_t rtmp_client_setdataframe_amf(
    rtmp_client_t client,
    size_t streamid,
    amf_t object
){
    return rtmp_stream_send_dat2( rtmp_client_stream( client ), 0, streamid, 0, object, nullptr );
}


//See NetConnect documentation for usage


rtmp_err_t rtmp_client_play( rtmp_client_t client,
    const char *stream_name,
    double start,
    double duration,
    bool reset );

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

rtmp_err_t rtmp_client_publish(
    rtmp_client_t client,
    size_t streamid,
    const char * playpath,
    const char * type,
    rtmp_stream_amf_proc proc,
    void *userdata
);

rtmp_err_t rtmp_client_seek( rtmp_client_t client,
    rtmp_time_t seek_ms);

rtmp_err_t rtmp_client_pause( rtmp_client_t client,
    bool paused,
    rtmp_time_t pause_ms);
