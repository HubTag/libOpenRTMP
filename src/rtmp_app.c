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

#include "rtmp_app.h"
#include "rtmp/rtmp_types.h"
#include "rtmp/rtmp_stream.h"
#include "rtmp/rtmp_private.h"
#include <stdio.h>
#include <stdlib.h>
#include "memutil.h"

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

    rtmp_app_on_video_proc on_video;
    void * on_video_data;

    rtmp_app_on_audio_proc on_audio;
    void * on_audio_data;
};

struct rtmp_app_list{
    VEC_DECLARE( rtmp_app_t ) apps;
};


rtmp_app_list_t rtmp_app_list_create( void ){
    rtmp_app_list_t applist = malloc( sizeof( struct rtmp_app_list ) );
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

    app = calloc( 1, sizeof( struct rtmp_app ) );
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

rtmp_cb_status_t rtmp_app_set_connect( rtmp_app_t app, rtmp_app_on_amf_proc proc, void *user ){
    app->on_connect = proc;
    app->on_connect_data = user;
    return RTMP_CB_CONTINUE;
}

rtmp_cb_status_t rtmp_app_set_disconnect( rtmp_app_t app, rtmp_app_on_amf_proc proc, void *user ){
    app->on_disconnect = proc;
    app->on_disconnect_data = user;
    return RTMP_CB_CONTINUE;
}

rtmp_cb_status_t rtmp_app_set_release( rtmp_app_t app, rtmp_app_on_amf_proc proc, void *user ){
    app->on_release = proc;
    app->on_release_data = user;
    return RTMP_CB_CONTINUE;
}

rtmp_cb_status_t rtmp_app_set_publish( rtmp_app_t app, rtmp_app_on_pub_proc proc, void *user ){
    app->on_publish = proc;
    app->on_publish_data = user;
    return RTMP_CB_CONTINUE;
}

rtmp_cb_status_t rtmp_app_set_fcpublish( rtmp_app_t app, rtmp_app_on_fcpub_proc proc, void *user ){
    app->on_fcpublish = proc;
    app->on_fcpublish_data = user;
    return RTMP_CB_CONTINUE;
}

rtmp_cb_status_t rtmp_app_set_metadata( rtmp_app_t app, rtmp_app_on_amf_proc proc, void *user ){
    app->on_metadata = proc;
    app->on_metadata_data = user;
    return RTMP_CB_CONTINUE;
}

rtmp_cb_status_t rtmp_app_set_video( rtmp_app_t app, rtmp_app_on_video_proc proc, void *user ){
    app->on_video = proc;
    app->on_video_data = user;
    return RTMP_CB_CONTINUE;
}

rtmp_cb_status_t rtmp_app_set_audio( rtmp_app_t app, rtmp_app_on_audio_proc proc, void *user ){
    app->on_audio = proc;
    app->on_audio_data = user;
    return RTMP_CB_CONTINUE;
}


rtmp_cb_status_t rtmp_app_connect( rtmp_stream_t stream, rtmp_app_t app, amf_value_t value ){
    return app->on_connect ? app->on_connect( stream, app, value, app->on_connect_data ) : RTMP_CB_CONTINUE;
}

rtmp_cb_status_t rtmp_app_disconnect( rtmp_stream_t stream, rtmp_app_t app, amf_value_t value ){
    return app->on_disconnect ? app->on_disconnect( stream, app, value, app->on_disconnect_data ) : RTMP_CB_CONTINUE;
}

rtmp_cb_status_t rtmp_app_release( rtmp_stream_t stream, rtmp_app_t app, amf_value_t value ){
    return app->on_release ? app->on_release( stream, app, value, app->on_release_data ) : RTMP_CB_CONTINUE;
}

size_t rtmp_app_publish( rtmp_stream_t stream, rtmp_app_t app, const char * name, const char * type, char * new_name, size_t len ){
    return app->on_publish ? app->on_publish( stream, app, name, type, new_name, len, app->on_publish_data ) : 0;
}

size_t rtmp_app_fcpublish( rtmp_stream_t stream, rtmp_app_t app, const char * name, char * new_name, size_t len ){
    return app->on_fcpublish ? app->on_fcpublish( stream, app, name, new_name, len, app->on_fcpublish_data ) : 0;
}

rtmp_cb_status_t rtmp_app_metadata( rtmp_stream_t stream, rtmp_app_t app, amf_value_t value ){
    return app->on_metadata ? app->on_metadata( stream, app, value, app->on_metadata_data ) : RTMP_CB_CONTINUE;
}

rtmp_cb_status_t rtmp_app_video( rtmp_stream_t stream, rtmp_app_t app ){
    return app->on_video ? app->on_video( stream, app, app->on_video_data ) : RTMP_CB_CONTINUE;
}

rtmp_cb_status_t rtmp_app_audio( rtmp_stream_t stream, rtmp_app_t app ){
    return app->on_audio ? app->on_audio( stream, app, app->on_audio_data ) : RTMP_CB_CONTINUE;
}
