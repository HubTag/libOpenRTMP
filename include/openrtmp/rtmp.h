/*
    rtmp.h

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

#ifndef RTMP_H_H
#define RTMP_H_H

#ifdef __cplusplus
extern "C" {
#endif

#if RTMP_LOG_LEVEL > 0
#define RTMP_GEN_ERROR(err) rtmp_gen_error(err,__LINE__,__FILE__,"")
#define RTMP_GEN_ERROR_MSG(err,msg) rtmp_gen_error(err,__LINE__,__FILE__,msg)
#else
#define RTMP_GEN_ERROR(err) (err)
#define RTMP_GEN_ERROR_MSG(err,msg) (err)
#endif

typedef struct rtmp_mgr * rtmp_t;
typedef struct rtmp_params *rtmp_params_t;

typedef enum {
    RTMP_APP,
    RTMP_FLASHVER,
    RTMP_TCURL,
    RTMP_PAGEURL,
    RTMP_ACODEC,
    RTMP_VCODEC,
    RTMP_VFUNC,
    RTMP_OENC,
    RTMP_FPAD
} rtmp_param_name_t;

#include <openrtmp/rtmp/rtmp_types.h>
#include <openrtmp/amf/amf.h>
#include <openrtmp/rtmp/rtmp_client.h>
#include <openrtmp/rtmp/rtmp_server.h>
#include <openrtmp/rtmp/rtmp_app.h>


char * rtmp_params_get_s( rtmp_params_t params, rtmp_param_name_t name );
uint32_t rtmp_params_get_d( rtmp_params_t params, rtmp_param_name_t name );

typedef rtmp_cb_status_t (*rtmp_connect_proc)(
    rtmp_server_t connection,
    void *user
);


rtmp_t rtmp_create( void );
void rtmp_destroy( rtmp_t mgr );

void rtmp_set_app_list( rtmp_t mgr, rtmp_app_list_t list );

rtmp_err_t rtmp_service( rtmp_t mgr, int timeout );

rtmp_err_t rtmp_connect( rtmp_t mgr, rtmp_client_t client );

rtmp_err_t rtmp_disconnect( rtmp_t mgr, rtmp_client_t client );

rtmp_err_t rtmp_close_stream( rtmp_t mgr, rtmp_stream_t stream );

rtmp_err_t rtmp_listen( rtmp_t mgr, const char * iface, short port, rtmp_connect_proc cb, void *user );

rtmp_err_t rtmp_amferr( amf_err_t err );

rtmp_err_t rtmp_gen_error(rtmp_err_t err, size_t line, const char *file, const char *msg);

#ifdef __cplusplus
}
#endif

#endif