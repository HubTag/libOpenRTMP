/*
    rtmp.c

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

#include "rtmp_config.h"
#include "rtmp.h"
#include "rtmp/rtmp_private.h"
#include<stdlib.h>

void rtmp_params_free( struct rtmp_params * params ){
    free( params->app );
    free( params->flashver );
    free( params->pageUrl );
    free( params->swfUrl );
    free( params->tcUrl );
}

rtmp_err_t rtmp_amferr( amf_err_t err ){
    switch(err){
        case AMF_ERR_OOM: return RTMP_ERR_OOM;
        case AMF_ERR_NONE: return RTMP_ERR_NONE;
        default: return RTMP_ERR_ERROR;
    }
}



char * rtmp_params_get_s( rtmp_params_t params, rtmp_param_name_t name ){
    switch( name ){
        case RTMP_APP: return params->app;
        case RTMP_FLASHVER: return params->flashver;
        case RTMP_TCURL: return params->tcUrl;
        case RTMP_PAGEURL: return params->pageUrl;
        default: return nullptr;
    }
}

uint32_t rtmp_params_get_d( rtmp_params_t params, rtmp_param_name_t name ){
    switch( name ){
        case RTMP_ACODEC: return params->audioCodecs;
        case RTMP_VCODEC: return params->videoCodecs;
        case RTMP_VFUNC: return params->videoFunction;
        case RTMP_OENC: return params->objectEncoding;
        case RTMP_FPAD: return params->fpad;
        default: return 0;
    }
}

void rtmp_set_app_list( rtmp_t mgr, rtmp_app_list_t list ){
    mgr->applist = list;
}
