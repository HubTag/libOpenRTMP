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

#include "rtmp_types.h"
#include "data_stream.h"
#include "ringbuffer.h"

typedef struct rtmp_chunk_stream_message{
    unsigned int chunk_stream_id;
    unsigned int timestamp;
    size_t message_length;
    unsigned int message_stream_id;
    byte message_type;
} rtmp_chunk_stream_message_t;

typedef struct rtmp_chunk_stream_message_internal{
    rtmp_chunk_stream_message_t msg;
    unsigned int time_delta;
    unsigned int processed;
    bool initialized;
} rtmp_chunk_stream_message_internal_t;

typedef struct rtmp_chunk_stream_cache{
    rtmp_chunk_stream_message_internal_t static_cache[RTMP_STREAM_STATIC_CACHE_SIZE];
    rtmp_chunk_stream_message_internal_t *dynamic_cache;
    size_t dynamic_cache_size;
} *rtmp_chunk_stream_cache_t;

rtmp_chunk_stream_cache_t rtmp_cache_create();
void rtmp_cache_destroy( rtmp_chunk_stream_cache_t cache );
rtmp_chunk_stream_message_internal_t * rtmp_cache_get( rtmp_chunk_stream_cache_t cache, size_t chunk_id );

rtmp_err_t rtmp_chunk_emit_shake_0( ringbuffer_t output );
rtmp_err_t rtmp_chunk_emit_shake_1( ringbuffer_t output, unsigned int timestamp, const byte* nonce, size_t length);
rtmp_err_t rtmp_chunk_emit_shake_2( ringbuffer_t output, unsigned int timestamp1, unsigned int timestamp2, const byte* nonce, size_t length);

rtmp_err_t rtmp_chunk_read_shake_0( ringbuffer_t input );
rtmp_err_t rtmp_chunk_read_shake_1( ringbuffer_t input, unsigned int *timestamp, byte* nonce, size_t length);
rtmp_err_t rtmp_chunk_read_shake_2( ringbuffer_t input, unsigned int *timestamp1, unsigned int *timestamp2, byte* data, size_t length);

rtmp_err_t rtmp_chunk_emit_hdr( ringbuffer_t output, rtmp_chunk_stream_message_t *message, rtmp_chunk_stream_cache_t cache );
rtmp_err_t rtmp_chunk_read_hdr( ringbuffer_t output, rtmp_chunk_stream_message_t **message, rtmp_chunk_stream_cache_t cache );

rtmp_err_t rtmp_chunk_emit_hdr_basic( ringbuffer_t output, byte format, size_t id );
rtmp_err_t rtmp_chunk_read_hdr_basic( ringbuffer_t input, byte *format, size_t *id );


void rtmp_print_message( rtmp_chunk_stream_message_t *msg );
