/*
    test.c

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

#include <signal.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "amf/amf.h"
#include "amf/amf_object.h"
#include "rtmp/chunk/rtmp_chunk_conn.h"
#include "rtmp/rtmp_constants.h"
#include "rtmp.h"
#include "rtmp_app.h"
#include "rtmp/chunk/rtmp_chunk_assembler.h"
#include "ringbuffer.h"
#include <stdarg.h>
#include "parseurl.h"
#include "memutil.h"
#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif

typedef struct app_data{
    rtmp_t rtmp;
    rtmp_client_t client;
    amf_t onMetadata;
    size_t streamid;
    bool ready;
} appdata_t;

rtmp_cb_status_t client_result_connect( rtmp_stream_args_t args, amf_t object, void * user );

static rtmp_cb_status_t connect_proc( rtmp_server_t server, void *user ){
    //printf("Got connection\n");
    static int i = 0;
    printf("%d\r", ++i );
    fflush(stdout);
    return RTMP_CB_CONTINUE;
}

static rtmp_cb_status_t app_connect( rtmp_stream_t stream, rtmp_app_t app, amf_t value, void *user ){
    printf("Create streamer connection\n");
    return RTMP_CB_CONTINUE;
}

static rtmp_cb_status_t app_connect2( rtmp_stream_t stream, rtmp_app_t app, amf_t value, void *user ){
    printf("Deny streamer2 connection\n");
    return RTMP_CB_ABORT;
}

static size_t app_fcpublish( rtmp_stream_t stream, rtmp_app_t app, const char * target, char * newname, size_t len, void *user ){
    size_t inlen = strlen( target );
    size_t cplen = MIN(inlen+1, len);
    memcpy( newname, target, cplen );
    newname[cplen] = 0;
    return inlen;
}

static rtmp_cb_status_t on_connect(rtmp_stream_t stream, rtmp_event_t event, void * user ){
    ALIAS( user, appdata_t *, data );
    return rtmp_client_connect( data->client, "streamer", nullptr, nullptr, RTMP_SUPPORT_SND_AAC, RTMP_SUPPORT_VID_H264, client_result_connect, user ) >= RTMP_ERR_ERROR ? RTMP_CB_ERROR : RTMP_CB_CONTINUE;
}

static size_t app_publish( rtmp_stream_t stream, rtmp_app_t app, const char * target, const char * type, char * newname, size_t len, void *user ){
    size_t inlen = strlen( target );
    size_t cplen = MIN(inlen+1, len);
    memcpy( newname, target, cplen );
    newname[cplen] = 0;
    ALIAS( user, appdata_t *, data );
    if( strcmp(target, "TEST") == 0 ){
        return inlen;
    }
    //data->client = rtmp_client_create( "rtmp://live-sea.twitch.tv/app", target );
    data->client = rtmp_client_create( "rtmp://localhost/streamer2", "TEST" );
    rtmp_stream_reg_event( rtmp_client_stream( data->client ), RTMP_EVENT_CONNECT_SUCCESS, on_connect, user );

    return inlen;
}

static rtmp_cb_status_t app_metadata( rtmp_stream_t stream, rtmp_app_t app, amf_t value, void *user ){
    ALIAS( user, appdata_t *, data );
    data->onMetadata = amf_reference( value );
    rtmp_connect( data->rtmp, data->client );
    return RTMP_CB_CONTINUE;
}

static rtmp_cb_status_t app_video( rtmp_stream_t stream, rtmp_app_t app, size_t streamid, rtmp_time_t timestamp, void * av_data, size_t av_length, void *user ){
    ALIAS( user, appdata_t *, data );
    if( !data->ready ){
        return RTMP_CB_CONTINUE;
    }
    return rtmp_stream_send_video2( rtmp_client_stream(data->client), 0, streamid, timestamp, av_data, av_length, nullptr ) == RTMP_ERR_NONE ? RTMP_CB_CONTINUE : RTMP_CB_ERROR;
}
static rtmp_cb_status_t app_audio( rtmp_stream_t stream, rtmp_app_t app, size_t streamid, rtmp_time_t timestamp, void * av_data, size_t av_length, void *user ){
    ALIAS( user, appdata_t *, data );
    if( !data->ready ){
        return RTMP_CB_CONTINUE;
    }
    return rtmp_stream_send_audio2( rtmp_client_stream(data->client), 0, streamid, timestamp, av_data, av_length, nullptr ) == RTMP_ERR_NONE ? RTMP_CB_CONTINUE : RTMP_CB_ERROR;
}

void rtmp_perror( rtmp_err_t err ){
    if( err ) printf("%s\n", rtmp_get_err_name( err ));
}

bool processing = true;

void sig_terminate(int foo){
    if (processing == false){
        //Something is probably blocking, force exit.
        printf("Forcing termination.\n");
        exit(1);
    }
    printf("Terminating...\n");
    processing = false;
}

#include "vec.h"
rtmp_cb_status_t client_result_publish( rtmp_stream_args_t args, amf_t object, void * user ){
    ALIAS( user, appdata_t *, data );
    rtmp_client_t client = data->client;
    if( amf_get_count( object ) >= 1 ){
        amf_value_t result = amf_get_item( object, 0 );
        if( amf_value_is( result, AMF_TYPE_STRING ) ){
            size_t len;
            const char * str = amf_value_get_string( result, &len );
            if( strncasecmp( str, "onStatus", len ) == 0 ){
                if( rtmp_client_setdataframe(   client, data->streamid, "onMetaData",
                                                0, 0, 1920, 1080,
                                                "avc1", 1000, 30,
                                                "mp4a", 128, 48000, 16, 2,
                                                "OpenRTMP")
                    /*rtmp_client_setdataframe_amf( client, data->streamid, data->onMetadata )*/ > RTMP_ERR_NONE ){
                    return RTMP_CB_ERROR;
                }
                data->ready = true;
                printf("Initialized\n");
                return RTMP_CB_CONTINUE;
            }
        }
    }
    return RTMP_CB_ABORT;
}
rtmp_cb_status_t client_result_createstream( rtmp_stream_args_t args, amf_t object, void * user ){
    ALIAS( user, appdata_t *, data );
    rtmp_client_t client = data->client;
    if( amf_get_count( object ) >= 4 ){
        amf_value_t result = amf_get_item( object, 0 );
        if( amf_value_is( result, AMF_TYPE_STRING ) ){
            size_t len;
            const char * str = amf_value_get_string( result, &len );
            if( strncasecmp( str, "_result", len ) == 0 ){
                data->streamid = amf_value_get_integer( amf_get_item( object, 3 ) );
                if( rtmp_client_publish( client, data->streamid, nullptr, "live", client_result_publish, user ) > RTMP_ERR_NONE ){
                    return RTMP_CB_ERROR;
                }
                return RTMP_CB_CONTINUE;
            }
        }
    }
    return RTMP_CB_ABORT;
}

rtmp_cb_status_t client_result_connect( rtmp_stream_args_t args, amf_t object, void * user ){
    ALIAS( user, appdata_t *, data );
    rtmp_client_t client = data->client;
    if( amf_get_count( object ) > 0 ){
        amf_value_t result = amf_get_item( object, 0 );
        if( amf_value_is( result, AMF_TYPE_STRING ) ){
            size_t len;
            const char * str = amf_value_get_string( result, &len );
            if( strncasecmp( str, "_result", len ) == 0 ){
                if( rtmp_client_releasestream( client, nullptr, nullptr, nullptr ) > RTMP_ERR_NONE ){
                    return RTMP_CB_ERROR;
                }
                if( rtmp_client_fcpublish( client, nullptr, nullptr, nullptr ) > RTMP_ERR_NONE ){
                    return RTMP_CB_ERROR;
                }
                if( rtmp_client_createstream( client, client_result_createstream, user ) > RTMP_ERR_NONE ){
                    return RTMP_CB_ERROR;
                }
                return RTMP_CB_CONTINUE;
            }
        }
    }
    return RTMP_CB_ABORT;
}

int main(){
    signal(SIGINT, sig_terminate);
	signal(SIGTERM, sig_terminate);

    rtmp_t rtmp = rtmp_create();
    rtmp_app_list_t list = rtmp_app_list_create();

    //Create "streamer" app
    appdata_t appdata;
    appdata.client = nullptr;
    appdata.onMetadata = nullptr;
    appdata.rtmp = rtmp;
    appdata.streamid = 0;
    appdata.ready = false;

    rtmp_app_t app = rtmp_app_list_register( list, "streamer" );
    rtmp_app_set_connect( app, app_connect, &appdata );
    rtmp_app_set_fcpublish( app, app_fcpublish, &appdata );
    rtmp_app_set_publish( app, app_publish, &appdata );
    rtmp_app_set_video( app, app_video, &appdata );
    rtmp_app_set_audio( app, app_audio, &appdata );
    rtmp_app_set_metadata( app, app_metadata, &appdata );

    //Create "streamer2" app
    app = rtmp_app_list_register( list, "streamer2" );
    rtmp_app_set_connect( app, app_connect2, nullptr );

    //Assign app list
    rtmp_set_app_list( rtmp, list );


    rtmp_perror(rtmp_listen( rtmp, RTMP_ADDR_ANY, RTMP_DEFAULT_PORT, connect_proc, nullptr ));


    while( processing ){
        rtmp_perror(rtmp_service( rtmp, RTMP_REFRESH_TIME ));
    }
    rtmp_destroy( rtmp );
    rtmp_app_list_destroy( list );
    printf("Closed\n");
    return 0;
}

