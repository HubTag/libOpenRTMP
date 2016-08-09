/*
    rtmp_server.c

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

#include "rtmp_server.h"
#include "rtmp/rtmp_types.h"
#include "rtmp/rtmp_stream.h"
#include "rtmp/rtmp_private.h"
#include <stdio.h>
#include <stdlib.h>


struct rtmp_server{
    struct rtmp_stream stream;
    struct rtmp_params params;
    rtmp_app_list_t applist;
    rtmp_app_t app;
};


#define LOAD_ARG_S(a) \
{ \
    amf_value_t val = amf_obj_get_value( args, #a ); \
    if( val ){ \
        size_t len; \
        const char *str = amf_value_get_string(val, &len); \
        self->params. a = str_dupl(str, len); \
    } \
}

#define LOAD_ARG_D(a) \
{ \
    amf_value_t val = amf_obj_get_value( args, #a ); \
    if( val ){ \
        self->params. a = amf_value_get_number( val ); \
    } \
}

rtmp_cb_status_t rtmp_server_shake_done( rtmp_stream_t stream, rtmp_event_t event, void * restrict user ){
    rtmp_err_t err = RTMP_ERR_NONE;
    err = err ? err : rtmp_chunk_conn_set_window_ack_size( rtmp_stream_get_conn( stream ), RTMP_DEFAULT_WINDOW_SIZE );
    err = err ? err : rtmp_chunk_conn_set_peer_bwidth( rtmp_stream_get_conn( stream ), RTMP_DEFAULT_WINDOW_SIZE, RTMP_LIMIT_DYNAMIC );
    err = err ? err : rtmp_chunk_conn_set_chunk_size( rtmp_stream_get_conn( stream ), RTMP_DEFAULT_CHUNK_SIZE );

    if( err != RTMP_ERR_NONE ){
        return RTMP_CB_ABORT;
    }
    return RTMP_CB_CONTINUE;
}

rtmp_cb_status_t rtmp_server_shake_fail( rtmp_stream_t conn, rtmp_event_t event, void * restrict user ){
    return RTMP_CB_ABORT;
}

rtmp_cb_status_t rtmp_server_onconnect(
    rtmp_stream_t stream,
    rtmp_message_type_t message,
    amf_t object,
    void *user
){
    rtmp_server_t self = user;
    if( amf_get_count( object ) < 3 ){
        return RTMP_CB_ABORT;
    }

    if( amf_value_get_number(amf_get_item( object, 1 )) != 1 ){
        return RTMP_CB_ABORT;
    }

    amf_value_t args = amf_get_item( object, 2 );
    if( !amf_value_is( args, AMF0_TYPE_OBJECT ) ){
        return RTMP_CB_ABORT;
    }
    LOAD_ARG_S(app);
    LOAD_ARG_S(flashver);
    LOAD_ARG_S(swfUrl);
    LOAD_ARG_S(tcUrl);
    LOAD_ARG_S(pageUrl);
    LOAD_ARG_D(audioCodecs);
    LOAD_ARG_D(videoCodecs);
    LOAD_ARG_D(videoFunction);
    LOAD_ARG_D(objectEncoding);

    if( self->applist == nullptr || self->app != nullptr ){
        goto fail;
    }
    printf("Got connection for app %s\n", self->params.app );

    self->app = rtmp_app_list_get( self->applist, self->params.app );

    if( !self->app ){
        goto fail;
    }

    rtmp_cb_status_t status = rtmp_app_connect( stream, self->app, &self->params );
    if( status != RTMP_CB_CONTINUE ){
        goto fail;
    }

    rtmp_err_t err = RTMP_ERR_NONE;
    err = err ? err : rtmp_stream_send_stream_begin( stream, 0 );
    err = err ? err : rtmp_stream_respond( stream, "_result", 1,
        AMF(
            AMF_OBJ(
                AMF_STR("fmsVer", RTMP_FMSVER_STR),
                AMF_INT("capabilities", RTMP_CAPABILITIES),
                AMF_INT("mode", RTMP_MODE)
            ),
            AMF_OBJ(
                AMF_STR("level", "status"),
                AMF_STR("code", RTMP_NETCON_ACCEPT),
                AMF_STR("description", "Connection Accepted"),
                AMF_OBJ( "data",
                        AMF_STR("string", RTMP_FMSVER)
                ),
                AMF_INT("objectEncoding", 0)
            )
        )
    );
    if( err != RTMP_ERR_NONE ){
        return RTMP_CB_ABORT;
    }
    return RTMP_CB_CONTINUE;

    fail:
    rtmp_stream_respond( stream, "_result", 1,
        AMF(
            AMF_OBJ(
                AMF_STR("fmsVer", RTMP_FMSVER_STR),
                AMF_INT("capabilities", RTMP_CAPABILITIES),
                AMF_INT("mode", RTMP_MODE)
            ),
            AMF_OBJ(
                AMF_STR("level", "status"),
                AMF_STR("code", RTMP_NETCON_REJECT),
                AMF_STR("description", "Connection Rejected"),
                AMF_OBJ( "data",
                        AMF_STR("string", RTMP_FMSVER)
                ),
                AMF_INT("objectEncoding", 0)
            )
        )
    );
    return RTMP_CB_ABORT;
}


rtmp_server_t rtmp_server_create( void ){
    rtmp_server_t server = calloc( 1, sizeof( struct rtmp_server ) );
    rtmp_stream_create_at( &server->stream, false );
    rtmp_stream_reg_amf( &server->stream, RTMP_MSG_AMF0_CMD, "connect", rtmp_server_onconnect, server );
    rtmp_stream_reg_event( &server->stream, RTMP_EVENT_CONNECT_SUCCESS, rtmp_server_shake_done, server );
    rtmp_stream_reg_event( &server->stream, RTMP_EVENT_CONNECT_FAIL, rtmp_server_shake_fail, server );

    return server;
}

void rtmp_server_destroy( rtmp_server_t server ){
    rtmp_stream_destroy_at( &server->stream );
    rtmp_params_free( &server->params );
    free( server );
}

bool rtmp_server_connected( rtmp_server_t server ){
    return rtmp_chunk_conn_connected( rtmp_stream_get_conn( &server->stream ) );
}

rtmp_stream_t rtmp_server_stream( rtmp_server_t server ){
    return &server->stream;
}

void rtmp_server_set_app_list( rtmp_server_t server, rtmp_app_list_t list ){
    server->applist = list;
}
