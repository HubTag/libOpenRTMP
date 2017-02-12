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

/*! \addtogroup rtmp_ref RTMP
    @{
*/

/*! \addtogroup rtmp_config Configurable Constants

    The RTMP config header `rtmp_config.h` contains many definitions that drive a few factors which pertain mostly to
    memory consumption vs performance. Some aren't really meant to be modified, but may be changed to affect various aspects of the library.

    @{
*/

//! \brief   The default port for RTMP.
//! \details This should probably not be changed unless you're planning on breaking RTMP compatibility.
#define RTMP_DEFAULT_PORT                   1935

//! A string indicating the version of "Flash Media Server," or in this case, libOpenRTMP
#define RTMP_FMSVER                         "0,0,0,0"
//! A string indicating the name and version of "Flash Media Server," or in this case, libOpenRTMP
#define RTMP_FMSVER_STR                     "LibOpenRTMP/" RTMP_FMSVER
//! A string indicating the version of "Flash," or in this case, libOpenRTMP
#define RTMP_FLASHVER_VERSION               "0.0"
//! A string indicating the name and version of "Flash," or in this case, libOpenRTMP
#define RTMP_FLASHVER_STR                   "LibOpenRTMP/" RTMP_FLASHVER_VERSION " (compatible; FMSc/1.0)"

//Protocol defaults. These may all be negotiated at runtime.
//! The default requested window size.
//#define RTMP_DEFAULT_WINDOW_SIZE            0x00FFFFFF
#define RTMP_DEFAULT_WINDOW_SIZE            0x00FFFFFF
// Doesn't really mean anything
#define RTMP_DEFAULT_PEER_WINDOW_SIZE       0x0
//! The default strategy setting for changing window size.
#define RTMP_DEFAULT_BANDWIDTH_TYPE         RTMP_LIMIT_HARD


//! The max chunk size this implementation will allow to be used. The peer may still use a larger chunk size than this.
#define RTMP_MAX_CHUNK_SIZE                 10000

//! The desired chunk size.
#define RTMP_DESIRED_CHUNK_SIZE             4096

//! \brief The default number of bytes allocated for input/output buffers.
//!
//! It may be desireable to be able to store a few seconds worth of data in these buffers.
//! At minimum, the buffer must be 1600 bytes to hold handshake messages.
#define RTMP_DEFAULT_IO_BUFFER_SIZE         30000000

#define RTMP_DEFAULT_PROXY_V_BUFFER_SIZE    0x05FFFFFF
#define RTMP_DEFAULT_PROXY_A_BUFFER_SIZE    0x05FFFFFF

//! \brief   The size of the static stream cache allocation.
//! \details If a chunk stream ID exceeds this number, dense dynamic allocation will be used, which does have negative performance implications.
#define RTMP_STREAM_STATIC_CACHE_SIZE 10

//! The max number of stream cache entries.
#define RTMP_STREAM_CACHE_MAX 100

//! \brief   The size of the control message buffer.
//! \details If a control message can't fit in this number of bytes, the message is ignored.
//!          As of writing, the biggest documented control message is 12 bytes.
#define RTMP_CONTROL_BUFFER_SIZE 16

//Force verification of handshake times and nonces.
//! \brief   Enables enforcement of handshake timestamps.
//! \details If defined, connections with handshakes whose timestamps aren't correct are dropped. There is no real reason to turn
//! this off, as it ensures both the peer and the client are communicating correctly.
#define RTMP_SPEC_ENFORCE_HANDSHAKE_TIMES

//! \brief   Enables enforcement of handshake nonces.
//! \details If defined, connections with handshakes whose nonces are incorrect are dropped. There is no real reason to turn
//!          this off, as it ensures both the peer and the client are communicating correctly.
#define RTMP_SPEC_ENFORCE_HANDSHAKE_NONCES

//! \brief   The logging level.
//! \details Currently, there are 5 logging levels:
//! * 0 - No logging
//! * 1 - Log fatal errors only (Suggested for production)
//! * 2 - Log most non-fatal errors (Suggested for debugging)
//! * 3 - Log all errors
//! * 4 - Log all status codes other than ERR_NONE
#define RTMP_LOG_LEVEL 4

//The max size of a 'safe alloc'
//! The maximum amount of memory allowed to be allocated by a stream. Currently unimplemented.
#define RTMP_MAX_ALLOC 10000


//Maximum values
//! \brief   The size of assembler buffers.
//! \details The chunk assembler will allocate this much room for assembly. Chunks larger than this will still be fragmented.
//!          Usually, chunks larger than a few hundred bytes are media frames, which are much easier to deal with as fragments.
#define RTMP_MAX_CHUNK_CACHE 0xFFFF

//! The maximum number of message streams on one connection.
#define RTMP_MAX_STREAMS 10

//Max number of buffers per assembler
//! \brief   The maximum number of assembler buffers.
//! \details The stream will be aborted if there are more streams than this.
#define RTMP_MAX_ASM_HARD_BUFFER 20

//! \brief   The maximum number of pre-allocated assembler buffers.
//! \details If more buffers are needed than this, they will be allocated as needed, and destroyed once the relevant message has been assembled.
#define RTMP_MAX_ASM_SOFT_BUFFER 5


//The technique used to poll connections.
//#define RTMP_POLLTECH_SELECT
//#define RTMP_POLLTECH_WSAPOLL
//#define RTMP_POLLTECH_POLL

//! \brief   If defined, the networking implementation used will be epoll based.
//! \details As of writing this, epoll is the only available networking implementation.
#define RTMP_POLLTECH_EPOLL

//! The maximum number of file descriptors returned by epoll.
#define RTMP_EPOLL_MAX 100

//! The maximum number of connections waiting to be accepted.
#define RTMP_LISTEN_SIZE 1000

//! The interval, in milliseconds, at which the refresh event is fired.
#define RTMP_REFRESH_TIME 1000

//! How long, in milliseconds, the library will wait for a pending RPC to finish.
#define RTMP_CALL_TIMEOUT 10000

/*! @} */
/*! @} */
#endif
