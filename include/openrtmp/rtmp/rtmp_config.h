/*
    rtmp_config.h

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

#ifndef RTMP_H_CONFIG_H
#define RTMP_H_CONFIG_H

#define RTMP_DEFAULT_PORT                   1935

#define RTMP_FMSVER                         "0,0,0,0"
#define RTMP_FMSVER_STR                     "LibOpenRTMP/" RTMP_FMSVER
#define RTMP_FLASHVER_VERSION               "0.0"
#define RTMP_FLASHVER_STR                   "LibOpenRTMP/" RTMP_FLASHVER_VERSION " (compatible; FMSc/1.0)"

//Protocol defaults. These may all be negotiated at runtime.
#define RTMP_DEFAULT_WINDOW_SIZE            0x00FFFFFF
#define RTMP_DEFAULT_PEER_WINDOW_SIZE       0x0
#define RTMP_DEFAULT_BANDWIDTH_TYPE         RTMP_LIMIT_HARD


//The max chunk size this implementation will allow to be used.
//The peer may still use a larger chunk size than this.
#define RTMP_MAX_CHUNK_SIZE                 0x0004FFFF

//Desired chunk size
#define RTMP_DESIRED_CHUNK_SIZE             0x005FFFFF

//Size of the IO buffers used to feed data in and out of the RTMP object.
//This must be at least 1600 bytes.
#define RTMP_DEFAULT_IO_BUFFER_SIZE         0x00FFFFFF

#define RTMP_DEFAULT_PROXY_V_BUFFER_SIZE    0x05FFFFFF
#define RTMP_DEFAULT_PROXY_A_BUFFER_SIZE    0x05FFFFFF

//Size of static allocation for chunk header cache
#define RTMP_STREAM_STATIC_CACHE_SIZE 10

//Max size of dynamic allocation for chunk header cache
#define RTMP_STREAM_CACHE_MAX 100

//Size of control chunk buffer. This MUST be at least 12 bytes.
#define RTMP_CONTROL_BUFFER_SIZE 16

//Force verification of handshake times and nonces. There is no real reason to turn
//these off, as they ensure both the peer and the client are communicating correctly.
#define RTMP_SPEC_ENFORCE_HANDSHAKE_TIMES
#define RTMP_SPEC_ENFORCE_HANDSHAKE_NONCES

//Determines if and how to log errors internal to the RTMP implementation.
// 0 - No logging
// 1 - Log fatal errors only (Suggested for production)
// 2 - Log most non-fatal errors (Suggested for debugging)
// 3 - Log all errors
// 4 - Log all status codes other than ERR_NONE
#define RTMP_LOG_LEVEL 4

//The max size of a 'safe alloc'
#define RTMP_MAX_ALLOC 10000


//Maximum values
#define RTMP_MAX_CHUNK_CACHE 0xFFFF
#define RTMP_MAX_STREAMS 10

//Max number of buffers per assembler
#define RTMP_MAX_ASM_HARD_BUFFER 20
//Max number of persistent buffers (going over this will cause
//a buffer allocation and deallocation for every message)
#define RTMP_MAX_ASM_SOFT_BUFFER 5


//The technique used to poll connections.
//#define RTMP_POLLTECH_SELECT
//#define RTMP_POLLTECH_WSAPOLL
//#define RTMP_POLLTECH_POLL
#define RTMP_POLLTECH_EPOLL

#define RTMP_EPOLL_MAX 100

#define RTMP_LISTEN_SIZE 1000

#define RTMP_REFRESH_TIME 1000

#define RTMP_CALL_TIMEOUT 10000

#endif
