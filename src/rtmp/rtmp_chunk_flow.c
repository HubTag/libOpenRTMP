/*
    rtmp_chunk_flow.c

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

#include <openrtmp/rtmp/rtmp_private.h>
#include <openrtmp/rtmp/rtmp_chunk_flow.h>
#include <openrtmp/rtmp/rtmp_debug.h>
#include <openrtmp/rtmp/rtmp_chunk_cache.h>
#include <openrtmp/util/memutil.h>
#include <openrtmp/util/algorithm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>




rtmp_err_t rtmp_chunk_emit_shake_0( ringbuffer_t out ){
    byte version = RTMP_VERSION;
    if( ringbuffer_copy_write( out, &version, 1 ) < 1 ){
        return RTMP_GEN_ERROR(RTMP_ERR_AGAIN);
    }
    return RTMP_GEN_ERROR(RTMP_ERR_NONE);
}

rtmp_err_t rtmp_chunk_emit_shake_1( ringbuffer_t output, rtmp_time_t timestamp, const byte * restrict nonce, size_t length){
    byte zero[4] = {0,0,0,0};
    byte timestamp_out[4];
    ntoh_write_ud( timestamp_out, timestamp );
    if( ringbuffer_copy_write( output, timestamp_out, 4 ) < 4){
        return RTMP_GEN_ERROR(RTMP_ERR_AGAIN);
    }
    if( ringbuffer_copy_write( output, zero, 4 ) < 4 ){
        return RTMP_GEN_ERROR(RTMP_ERR_AGAIN);
    }
    if( ringbuffer_copy_write( output, nonce, length ) < length ){
        return RTMP_GEN_ERROR(RTMP_ERR_AGAIN);
    }
    return RTMP_GEN_ERROR(RTMP_ERR_NONE);
}

rtmp_err_t rtmp_chunk_emit_shake_2( ringbuffer_t output, rtmp_time_t timestamp1, rtmp_time_t timestamp2, const byte * restrict nonce, size_t length){
    byte timestamp1_out[4];
    byte timestamp2_out[4];
    ntoh_write_ud( timestamp1_out, timestamp1 );
    ntoh_write_ud( timestamp2_out, timestamp2 );
    if( ringbuffer_copy_write( output, timestamp1_out, 4 ) < 4){
        return RTMP_GEN_ERROR(RTMP_ERR_AGAIN);
    }
    if( ringbuffer_copy_write( output, timestamp2_out, 4 ) < 4 ){
        return RTMP_GEN_ERROR(RTMP_ERR_AGAIN);
    }
    if( ringbuffer_copy_write( output, nonce, length ) < length ){
        return RTMP_GEN_ERROR(RTMP_ERR_AGAIN);
    }
    return RTMP_GEN_ERROR(RTMP_ERR_NONE);
}


rtmp_err_t rtmp_chunk_read_shake_0( ringbuffer_t input ){
    byte version;
    int result = ringbuffer_copy_read( input, &version, 1 );
    if( result < 1){
        return RTMP_GEN_ERROR(RTMP_ERR_AGAIN);
    }
    if( version != 3 ){
        return RTMP_GEN_ERROR(RTMP_ERR_INVALID);
    }
    return RTMP_GEN_ERROR(RTMP_ERR_NONE);
}

rtmp_err_t rtmp_chunk_read_shake_1( ringbuffer_t input, rtmp_time_t * restrict timestamp, byte * restrict nonce, size_t length){
    byte timestamp_in[4];
    byte zero[4];
    if( ringbuffer_copy_read( input, timestamp_in, 4 ) < 4){
        return RTMP_GEN_ERROR(RTMP_ERR_AGAIN);
    }
    if( ringbuffer_copy_read( input, zero, 4 ) < 4 ){
        return RTMP_GEN_ERROR(RTMP_ERR_AGAIN);
    }
    if( ringbuffer_copy_read( input, nonce, length ) < length ){
        return RTMP_GEN_ERROR(RTMP_ERR_AGAIN);
    }
    *timestamp = ntoh_read_ud( timestamp_in );
    return RTMP_GEN_ERROR(RTMP_ERR_NONE);
}

rtmp_err_t rtmp_chunk_read_shake_2( ringbuffer_t input, rtmp_time_t * restrict timestamp1, rtmp_time_t * restrict timestamp2, byte * restrict nonce, size_t length){
    byte timestamp1_in[4];
    byte timestamp2_in[4];
    if( ringbuffer_copy_read( input, timestamp1_in, 4 ) < 4 ){
        return RTMP_GEN_ERROR(RTMP_ERR_AGAIN);
    }
    if( ringbuffer_copy_read( input, timestamp2_in, 4 ) < 4 ){
        return RTMP_GEN_ERROR(RTMP_ERR_AGAIN);
    }
    if( ringbuffer_copy_read( input, nonce, length ) < length ){
        return RTMP_GEN_ERROR(RTMP_ERR_AGAIN);
    }
    *timestamp1 = ntoh_read_ud( timestamp1_in );
    *timestamp2 = ntoh_read_ud( timestamp2_in );
    return RTMP_GEN_ERROR(RTMP_ERR_NONE);
}

rtmp_err_t rtmp_chunk_emit_hdr( ringbuffer_t output, rtmp_chunk_stream_message_t *message, rtmp_chunk_stream_cache_t cache ){
    byte fmt = 0;
    rtmp_time_t timestamp = message->timestamp;
    rtmp_chunk_stream_message_internal_t *previous;

    if( message->chunk_stream_id == 0 ){
        previous = rtmp_cache_find( cache, message->message_stream_id, message->message_length, message->message_type, message->timestamp );
        message->chunk_stream_id = previous->msg.chunk_stream_id;
    }
    else{
        previous = rtmp_cache_get(cache, message->chunk_stream_id);
    }
    if( previous == nullptr ){
        return RTMP_GEN_ERROR(RTMP_ERR_INADEQUATE_CHUNK);
    }
    int32_t delta = timestamp_get_delta( previous->msg.timestamp, timestamp );

    if( previous->initialized ){
        //If zero, force a delta of zero rather than use an absolute timestamp
        if( timestamp == 0 ){
            delta = 0;
            timestamp = previous->msg.timestamp;
        }
        if( previous->msg.message_stream_id == message->message_stream_id ){
            fmt = 1;
            if( previous->msg.message_type == message->message_type &&
                previous->msg.message_length == message->message_length ){
                fmt = 2;
                if( previous->time_delta == (rtmp_time_t)delta ){
                    fmt = 3;
                }
            }
        }
    }
    if( fmt > 0 ){
        timestamp = timestamp_get_delta( previous->msg.timestamp, timestamp );
    }
    memcpy( previous, message, sizeof( rtmp_chunk_stream_message_t) );
    previous->time_delta = delta;
    previous->initialized = true;

    byte buffer[16];
    size_t position = 0;
    rtmp_err_t err;
    if( (err = rtmp_chunk_emit_hdr_basic( output, fmt, message->chunk_stream_id ) ) >= RTMP_ERR_ERROR ){
        return RTMP_GEN_ERROR(err);
    }
    if( fmt <= 2 ){
        if( timestamp >= 0xFFFFFF ){
            ntoh_write_ud3( buffer + position, 0xFFFFFF );
        }
        else{
            ntoh_write_ud3( buffer + position, timestamp );
        }
        position += 3;
    }
    if( fmt <= 1 ){
        ntoh_write_ud3( buffer + position, message->message_length );
        buffer[position + 3] = message->message_type;
        position += 4;
    }
    if( fmt <= 0 ){
        htol_write_ud( buffer + position, message->message_stream_id );
        position += 4;
    }
    if( timestamp >= 0xFFFFFF ){
        ntoh_write_ud( buffer + position, timestamp );
        position += 4;
    }
    if(ringbuffer_copy_write( output, buffer, position ) < position ){
        return RTMP_GEN_ERROR(RTMP_ERR_AGAIN);
    }
    return RTMP_GEN_ERROR(RTMP_ERR_NONE);
}

rtmp_err_t rtmp_chunk_read_hdr( ringbuffer_t input, rtmp_chunk_stream_message_t **message_out, rtmp_chunk_stream_cache_t cache ){
    size_t id;
    byte fmt;
    unsigned int old_time = 0;
    rtmp_err_t err;
    if( (err = rtmp_chunk_read_hdr_basic( input, &fmt, &id )) >= RTMP_ERR_ERROR ){
        return RTMP_GEN_ERROR(err);
    }

    rtmp_chunk_stream_message_internal_t *previous = rtmp_cache_get(cache, id);
    if( previous == nullptr ){
        return RTMP_GEN_ERROR(RTMP_ERR_INADEQUATE_CHUNK);
    }
    old_time = previous->msg.timestamp;

    rtmp_chunk_stream_message_t *message = &previous->msg;
    message->chunk_stream_id = id;
    byte buffer[4];

    rtmp_time_t new_time = 0;
    if( fmt <= 2 ){
        if( ringbuffer_copy_read( input, buffer, 3) < 3 ){
            return RTMP_GEN_ERROR(RTMP_ERR_AGAIN);
        }
        new_time = ntoh_read_ud3( buffer );
    }
    if( fmt <= 1 ){
        if( ringbuffer_copy_read( input, buffer, 4) < 4 ){
            return RTMP_GEN_ERROR(RTMP_ERR_AGAIN);
        }
        message->message_length = ntoh_read_ud3( buffer );
        message->message_type = buffer[3];
    }
    if( fmt <= 0 ){
        if( ringbuffer_copy_read( input, buffer, 4) < 4 ){
            return RTMP_GEN_ERROR(RTMP_ERR_AGAIN);
        }
        message->message_stream_id = ltoh_read_ud( buffer );
    }
    if( ( fmt == 3 && old_time == 0xFFFFFF ) || new_time == 0xFFFFFF ){
        if( ringbuffer_copy_read( input, buffer, 4) < 4 ){
            return RTMP_GEN_ERROR(RTMP_ERR_AGAIN);
        }
        new_time = ntoh_read_ud( buffer );
    }
    if( fmt == 0 ){
        message->timestamp = new_time;
    }
    else{
        message->timestamp += new_time;
    }
    *message_out = message;
    return RTMP_GEN_ERROR(RTMP_ERR_NONE);
}

rtmp_err_t rtmp_chunk_emit_hdr_basic( ringbuffer_t output, byte format, size_t id ){
    byte buffer[3];
    size_t len = 1;
    //Fill the two least significant bits of buffer[0] with format
    buffer[0] = (format & 3) << 6;
    if( id < 2 ){
        //id 0 and 1 are reserved
        return RTMP_GEN_ERROR(RTMP_ERR_INVALID);
    }
    else if( id < 64 ){
        //Use one byte format. Fill the top 6 bits of buffer[0] with the ID.
        buffer[0] |= ( id & 63 );
    }
    else if( id < 256 + 64 ){
        //Use two byte format.
        buffer[1] = id - 64;
        len = 2;
    }
    else if( id < 65535 + 64 ){
        //Use three byte format.
        buffer[0] |= 1;
        buffer[1] = (id - 64) >> 8;
        buffer[2] = id - 64;
        len = 3;
    }
    else{
        return RTMP_GEN_ERROR(RTMP_ERR_INVALID);
    }
    if( ringbuffer_copy_write( output, buffer, len ) < len ){
        return RTMP_GEN_ERROR(RTMP_ERR_AGAIN);
    }
    return RTMP_GEN_ERROR(RTMP_ERR_NONE);
}

rtmp_err_t rtmp_chunk_read_hdr_basic( ringbuffer_t input, byte *format, size_t *id ){
    byte buffer[3];
    if( ringbuffer_copy_read( input, buffer, 1) < 1 ){
        return RTMP_GEN_ERROR(RTMP_ERR_AGAIN);
    }
    *format = (buffer[0] >> 6) & 3;
    buffer[0] = buffer[0] & 63;
    if( buffer[0] > 1 ){
        *id = buffer[0];
        return RTMP_GEN_ERROR(RTMP_ERR_NONE);
    }
    else if( buffer[0] == 0 ){
        if( ringbuffer_copy_read( input, buffer + 1, 1) < 1 ){
            return RTMP_GEN_ERROR(RTMP_ERR_AGAIN);
        }
        *id = buffer[1];
        *id += 64;
        return RTMP_GEN_ERROR(RTMP_ERR_NONE);
    }
    else { //buffer[0] must be 1
        if( ringbuffer_copy_read( input, buffer + 1, 2) < 2 ){
            return RTMP_GEN_ERROR(RTMP_ERR_AGAIN);
        }
        *id = buffer[1];
        *id <<= 8;
        *id |= buffer[2];
        *id += 64;
        return RTMP_GEN_ERROR(RTMP_ERR_NONE);
    }
}



void rtmp_print_message( rtmp_chunk_stream_message_t *msg ){
    printf("Message: \n"
            "\tChunk Stream:   %d\n"
            "\tMessage Stream: %d\n"
            "\tTimestamp:      %lu\n"
            "\tType:           %s\n"
            "\tMessage Length: %lu\n\n",
            msg->chunk_stream_id,
            msg->message_stream_id,
            msg->timestamp,
            rtmp_get_message_type_name((rtmp_message_type_t)msg->message_type),
            msg->message_length );
}
