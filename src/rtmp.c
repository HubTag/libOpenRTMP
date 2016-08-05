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



