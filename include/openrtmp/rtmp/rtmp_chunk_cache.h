/*
    rtmp_chunk_cache.h

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

#ifndef RTMP_H_CHUNK_CACHE_H
#define RTMP_H_CHUNK_CACHE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <openrtmp/rtmp/rtmp_types.h>
#include <openrtmp/util/ringbuffer.h>

//A chunk message header
typedef struct rtmp_chunk_stream_message{
    uint32_t chunk_stream_id;       //ID of the chunk stream
    rtmp_time_t timestamp;          //Absolute timestamp of the message
    size_t message_length;          //Length of the message in bytes
    uint32_t message_stream_id;     //ID of the message stream
    byte message_type;              //Message type
} rtmp_chunk_stream_message_t;

typedef struct rtmp_chunk_stream_message_internal rtmp_chunk_stream_message_internal_t;
typedef struct rtmp_chunk_stream_cache *rtmp_chunk_stream_cache_t;

//Create a stream cache, used for decoding partial chunk headers.
rtmp_chunk_stream_cache_t rtmp_cache_create( void );

void rtmp_cache_destroy( rtmp_chunk_stream_cache_t cache );
void rtmp_cache_reset( rtmp_chunk_stream_cache_t cache );

//Get a cached message header by chunk stream ID
rtmp_chunk_stream_message_internal_t * rtmp_cache_get( rtmp_chunk_stream_cache_t cache, size_t chunk_id );
rtmp_chunk_stream_message_internal_t * rtmp_cache_find( rtmp_chunk_stream_cache_t cache, size_t stream_id, size_t msg_size, byte msg_type, rtmp_time_t timestamp );


#ifdef __cplusplus
}
#endif

#endif
