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
    rtmp_client_t client = ezalloc( struct rtmp_client );

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
        const char * amp = strchr( path, '&' );
        const char * colon = nullptr;
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
        client->playpath = str_dup( path );
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

rtmp_err_t rtmp_client_get_conninfo( rtmp_client_t client, const char **host, uint16_t * port ){
    if( client->url ){
        *host = parseurl_get( client->url, PARSEURL_HOST, nullptr );
        *port = atoi( parseurl_get( client->url, PARSEURL_PORT, STR(RTMP_DEFAULT_PORT) ) );
        if( !*host || !*port ){
            return RTMP_ERR_ERROR;
        }
    }
    return RTMP_ERR_NONE;
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

rtmp_err_t rtmp_client_publish( rtmp_client_t client,
    const char * restrict publish_name,
    const char * restrict publish_type);

rtmp_err_t rtmp_client_seek( rtmp_client_t client,
    rtmp_time_t seek_ms);

rtmp_err_t rtmp_client_pause( rtmp_client_t client,
    bool paused,
    rtmp_time_t pause_ms);
