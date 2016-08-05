/*
    rtmp_types.h

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

#include <stdint.h>

typedef unsigned int rtmp_time_t;
typedef unsigned char byte;
typedef unsigned long size_t;



#ifndef __cplusplus
#include <stdbool.h>
#define nullptr ((void*)0x0)
#endif


typedef enum {
    si_yotta = 24,
    si_zetta = 21,
    si_exa = 18,
    si_peta = 15,
    si_tera = 12,
    si_giga = 9,
    si_mega = 6,
    si_kilo = 3,
    si_none = 0, //Or NULL
    si_milli = -3,
    si_micro = -6,
    si_nano = -9,
    si_pico = -12,
    si_femto = -15,
    si_atto = -18,
    si_zepto = -21,
    si_yocto = -24
} si_prefix;

#ifdef __cplusplus
#define DEFAULT(a) = a
#else
#define DEFAULT(a)
#endif

#ifdef _WIN32
    #include <windows.h>
    #include <winsock2.h>
    typedef SOCKET rtmp_sock_t;
#else
    #include <sys/socket.h>
    #include <netinet/tcp.h>
    #include <netinet/in.h>
    #include <netdb.h>
    typedef int rtmp_sock_t;
#endif
