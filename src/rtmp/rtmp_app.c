/*
    rtmp_app.c

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

#include <openrtmp/rtmp/rtmp_app.h>
#include <openrtmp/rtmp/rtmp_types.h>
#include <openrtmp/rtmp/rtmp_stream.h>
#include <openrtmp/rtmp/rtmp_private.h>
#include <stdio.h>
#include <stdlib.h>
#include <openrtmp/util/memutil.h>

struct rtmp_app {
    char *name;
    rtmp_app_on_amf_proc on_connect;
    void * on_connect_data;

    rtmp_app_on_amf_proc on_disconnect;
    void * on_disconnect_data;

    rtmp_app_on_amf_proc on_release;
    void * on_release_data;

    rtmp_app_on_fcpub_proc on_fcpublish;
    void * on_fcpublish_data;

    rtmp_app_on_pub_proc on_publish;
    void * on_publish_data;

    rtmp_app_on_amf_proc on_metadata;
    void * on_metadata_data;

    rtmp_app_on_av_proc on_video;
    void * on_video_data;

    rtmp_app_on_av_proc on_audio;
    void * on_audio_data;
};

struct rtmp_app_list{
    VEC_DECLARE( rtmp_app_t ) apps;
};


rtmp_app_list_t rtmp_app_list_create( void ){
    rtmp_app_list_t applist = ezalloc( applist );
    VEC_INIT( applist->apps );
    return applist;
}

void rtmp_app_list_destroy( rtmp_app_list_t list ){
    for( size_t i = 0; i < VEC_SIZE(list->apps); ++i ){
        free( list->apps[i]->name );
        free( list->apps[i] );
    }
    VEC_DESTROY( list->apps );
    free( list );
}

rtmp_app_t rtmp_app_list_register( rtmp_app_list_t list, const char *appname ){
    rtmp_app_t app = rtmp_app_list_get( list, appname );
    if( app ){
        return app;
    }

    app = ezalloc( app );
    if( !app ){
        return nullptr;
    }

    app->name = str_dup( appname );
    if( !app->name ){
        free( app );
        return nullptr;
    }

    rtmp_app_t * app_pos = VEC_PUSH( list->apps );
    if( !app_pos ){
        free( app );
        free( app->name );
        return nullptr;
    }
    *app_pos = app;

    return app;
}

rtmp_app_t rtmp_app_list_get( rtmp_app_list_t list, const char *appname ){
    for( size_t i = 0; i < VEC_SIZE( list->apps ); ++i ){
        if( strcmp( appname, list->apps[i]->name ) == 0 ){
            return list->apps[i];
        }
    }
    return nullptr;
}

void rtmp_app_set_connect( rtmp_app_t app, rtmp_app_on_amf_proc proc, void *user ){
    app->on_connect = proc;
    app->on_connect_data = user;
}

void rtmp_app_set_disconnect( rtmp_app_t app, rtmp_app_on_amf_proc proc, void *user ){
    app->on_disconnect = proc;
    app->on_disconnect_data = user;
}

void rtmp_app_set_release( rtmp_app_t app, rtmp_app_on_amf_proc proc, void *user ){
    app->on_release = proc;
    app->on_release_data = user;
}

void rtmp_app_set_publish( rtmp_app_t app, rtmp_app_on_pub_proc proc, void *user ){
    app->on_publish = proc;
    app->on_publish_data = user;
}

void rtmp_app_set_fcpublish( rtmp_app_t app, rtmp_app_on_fcpub_proc proc, void *user ){
    app->on_fcpublish = proc;
    app->on_fcpublish_data = user;
}

void rtmp_app_set_metadata( rtmp_app_t app, rtmp_app_on_amf_proc proc, void *user ){
    app->on_metadata = proc;
    app->on_metadata_data = user;
}

void rtmp_app_set_video( rtmp_app_t app, rtmp_app_on_av_proc proc, void *user ){
    app->on_video = proc;
    app->on_video_data = user;
}

void rtmp_app_set_audio( rtmp_app_t app, rtmp_app_on_av_proc proc, void *user ){
    app->on_audio = proc;
    app->on_audio_data = user;
}


rtmp_cb_status_t rtmp_app_connect( rtmp_stream_t stream, rtmp_app_t app, amf_t value ){
    return app->on_connect ? app->on_connect( stream, app, value, app->on_connect_data ) : RTMP_CB_CONTINUE;
}

rtmp_cb_status_t rtmp_app_disconnect( rtmp_stream_t stream, rtmp_app_t app, amf_t value ){
    return app->on_disconnect ? app->on_disconnect( stream, app, value, app->on_disconnect_data ) : RTMP_CB_CONTINUE;
}

rtmp_cb_status_t rtmp_app_release( rtmp_stream_t stream, rtmp_app_t app, amf_t value ){
    return app->on_release ? app->on_release( stream, app, value, app->on_release_data ) : RTMP_CB_CONTINUE;
}

rtmp_cb_status_t rtmp_app_publish( rtmp_stream_t stream, rtmp_app_t app, const char * name, const char * type ){
    return app->on_publish ? app->on_publish( stream, app, name, type, app->on_publish_data ) : RTMP_CB_CONTINUE;
}

rtmp_cb_status_t rtmp_app_fcpublish( rtmp_stream_t stream, rtmp_app_t app, const char * name ){
    return app->on_fcpublish ? app->on_fcpublish( stream, app, name, app->on_fcpublish_data ) : RTMP_CB_CONTINUE;
}

rtmp_cb_status_t rtmp_app_metadata( rtmp_stream_t stream, rtmp_app_t app, amf_t value ){
    return app->on_metadata ? app->on_metadata( stream, app, value, app->on_metadata_data ) : RTMP_CB_CONTINUE;
}

rtmp_cb_status_t rtmp_app_video( rtmp_stream_t stream, rtmp_app_t app, size_t streamid, rtmp_time_t timestamp, const void * av_data, size_t av_length, bool final_part ){
    return app->on_video ? app->on_video( stream, app, streamid, timestamp, av_data, av_length, final_part, app->on_video_data ) : RTMP_CB_CONTINUE;
}

rtmp_cb_status_t rtmp_app_audio( rtmp_stream_t stream, rtmp_app_t app, size_t streamid, rtmp_time_t timestamp, const void * av_data, size_t av_length, bool final_part ){
    return app->on_audio ? app->on_audio( stream, app, streamid, timestamp, av_data, av_length, final_part, app->on_audio_data ) : RTMP_CB_CONTINUE;
}
