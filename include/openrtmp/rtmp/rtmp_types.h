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

#ifndef RTMP_H_TYPES_H
#define RTMP_H_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>
#include <stddef.h>

typedef uint8_t byte;
typedef size_t rtmp_time_t;



#ifndef __cplusplus
#include <stdbool.h>
#define nullptr ((void*)0x0)
#else
#define restrict
#endif



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

#ifdef __cplusplus
}
#endif

#endif
