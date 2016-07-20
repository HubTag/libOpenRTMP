/*
    rtmp_chunk_flow.h

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

#include "../rtmp_types.h"
#include "../../data_stream.h"
#include "../../ringbuffer.h"

//A chunk message header
typedef struct rtmp_chunk_stream_message{
    uint32_t chunk_stream_id;   //ID of the chunk stream
    rtmp_time_t timestamp;         //Absolute timestamp of the message
    size_t message_length;          //Length of the message in bytes
    uint32_t message_stream_id; //ID of the message stream
    byte message_type;              //Message type
} rtmp_chunk_stream_message_t;

typedef struct rtmp_chunk_stream_message_internal rtmp_chunk_stream_message_internal_t;
typedef struct rtmp_chunk_stream_cache *rtmp_chunk_stream_cache_t;

//Create a stream cache, used for decoding partial chunk headers.
rtmp_chunk_stream_cache_t rtmp_cache_create( void );

void rtmp_cache_destroy( rtmp_chunk_stream_cache_t cache );

//Get a cached message header by chunk stream ID
rtmp_chunk_stream_message_internal_t * rtmp_cache_get( rtmp_chunk_stream_cache_t cache, size_t chunk_id );

//Used to emit the various handshakes. Used for both the client and server; the only difference is ordering.
rtmp_err_t rtmp_chunk_emit_shake_0( ringbuffer_t output );
rtmp_err_t rtmp_chunk_emit_shake_1( ringbuffer_t output, rtmp_time_t timestamp, const byte* nonce, size_t length);
rtmp_err_t rtmp_chunk_emit_shake_2( ringbuffer_t output, rtmp_time_t timestamp1, rtmp_time_t timestamp2, const byte* nonce, size_t length);

//Used to read the various handshakes. Used for both the client and server; the only difference is ordering.
rtmp_err_t rtmp_chunk_read_shake_0( ringbuffer_t input );
rtmp_err_t rtmp_chunk_read_shake_1( ringbuffer_t input, rtmp_time_t *timestamp, byte* nonce, size_t length);
rtmp_err_t rtmp_chunk_read_shake_2( ringbuffer_t input, rtmp_time_t *timestamp1, rtmp_time_t *timestamp2, byte* data, size_t length);

//Used to emit a header. Contents of header will be read from message and stored in the cache for future writes.
rtmp_err_t rtmp_chunk_emit_hdr( ringbuffer_t output, rtmp_chunk_stream_message_t *message, rtmp_chunk_stream_cache_t cache );

//Used to read a header. Message will be filled with a pointer to a message in the cache which contains all the information.
rtmp_err_t rtmp_chunk_read_hdr( ringbuffer_t input, rtmp_chunk_stream_message_t **message, rtmp_chunk_stream_cache_t cache );

//Used to emit the initial few bytes of the chunk header, as the length and content is variable based on the id value.
rtmp_err_t rtmp_chunk_emit_hdr_basic( ringbuffer_t output, byte format, size_t id );

//Used to read the initial few bytes of the chunk header, as the length and content is variable based on the id value.
rtmp_err_t rtmp_chunk_read_hdr_basic( ringbuffer_t input, byte *format, size_t *id );

//Print the contents of a chunk header. For debugging.
void rtmp_print_message( rtmp_chunk_stream_message_t *msg );
