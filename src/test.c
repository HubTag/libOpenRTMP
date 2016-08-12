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
#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif

static rtmp_cb_status_t connect_proc( rtmp_server_t server, void *user ){
    printf("Got connection\n");
    return RTMP_CB_CONTINUE;
}

static rtmp_cb_status_t app_connect( rtmp_stream_t stream, rtmp_app_t app, amf_value_t value, void *user ){
    printf("Create streamer connection\n");
    return RTMP_CB_CONTINUE;
}

static rtmp_cb_status_t app_connect2( rtmp_stream_t stream, rtmp_app_t app, amf_value_t value, void *user ){
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

static size_t app_publish( rtmp_stream_t stream, rtmp_app_t app, const char * target, const char * type, char * newname, size_t len, void *user ){
    size_t inlen = strlen( target );
    size_t cplen = MIN(inlen+1, len);
    memcpy( newname, target, cplen );
    newname[cplen] = 0;
    return inlen;
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

int main(){
    signal(SIGINT, sig_terminate);
	signal(SIGTERM, sig_terminate);

    rtmp_t rtmp = rtmp_create();
    rtmp_app_list_t list = rtmp_app_list_create();

    //Create "streamer" app
    rtmp_app_t app = rtmp_app_list_register( list, "streamer" );
    rtmp_app_set_connect( app, app_connect, nullptr );
    rtmp_app_set_fcpublish( app, app_fcpublish, nullptr );
    rtmp_app_set_publish( app, app_publish, nullptr );

    //Create "streamer2" app
    app = rtmp_app_list_register( list, "streamer2" );
    rtmp_app_set_connect( app, app_connect2, nullptr );

    //Assign app list
    rtmp_set_app_list( rtmp, list );


    rtmp_listen( rtmp, RTMP_ADDR_ANY, RTMP_DEFAULT_PORT, connect_proc, nullptr );
    while( processing ){
        rtmp_service( rtmp, RTMP_REFRESH_TIME );
    }
    rtmp_destroy( rtmp );
    rtmp_app_list_destroy( list );
    printf("Closed\n");
    return 0;
}

