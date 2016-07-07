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

typedef struct rtmp_chunk_stream_message{
    unsigned int chunk_stream_id;
    unsigned int timestamp;
    size_t message_length;
    byte message_type;
    unsigned int message_stream_id;
} rtmp_chunk_stream_message_t;

rtmp_err_t rtmp_chunk_emit_shake_0( ors_data_t output );
rtmp_err_t rtmp_chunk_emit_shake_1( ors_data_t output, unsigned int timestamp, const byte* nonce, size_t length);
rtmp_err_t rtmp_chunk_emit_shake_2( ors_data_t output, unsigned int timestamp1, unsigned int timestamp2, const byte* nonce, size_t length);

rtmp_err_t rtmp_chunk_read_shake_0( ors_data_t input );
rtmp_err_t rtmp_chunk_read_shake_1( ors_data_t input, unsigned int *timestamp, byte* nonce, size_t length);
rtmp_err_t rtmp_chunk_read_shake_2( ors_data_t input, unsigned int *timestamp1, unsigned int *timestamp2, byte* data, size_t length);

rtmp_err_t rtmp_chunk_emit_hdr( ors_data_t output, rtmp_chunk_stream_message_t *message, rtmp_chunk_stream_message_t cache[RTMP_STREAM_CACHE_MAX] );
rtmp_err_t rtmp_chunk_read_hdr( ors_data_t output, rtmp_chunk_stream_message_t **message, rtmp_chunk_stream_message_t cache[RTMP_STREAM_CACHE_MAX] );

rtmp_err_t rtmp_chunk_emit_hdr_basic( ors_data_t output, byte format, size_t id );
rtmp_err_t rtmp_chunk_read_hdr_basic( ors_data_t input, byte *format, size_t *id );


void rtmp_print_message( rtmp_chunk_stream_message_t *msg );
