/*
    rtmp_server.h

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

#pragma once

typedef struct rtmp_server * rtmp_server_t;

#include "rtmp/rtmp_types.h"
#include "rtmp.h"


rtmp_server_t rtmp_server_create( void );
void rtmp_server_destroy( rtmp_server_t server );

bool rtmp_server_connected( rtmp_server_t server );

rtmp_stream_t rtmp_server_stream( rtmp_server_t server );

