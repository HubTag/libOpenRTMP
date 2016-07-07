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

#include "rtmp_chunk_flow.h"
#include "rtmp_debug.h"
#include "data_stream.h"
#include <stdio.h>

rtmp_err_t rtmp_chunk_emit_shake_0( ors_data_t output ){
    byte version = 3;
    if( ors_data_write( output, &version, 1 ) < 0 ){
        return RTMP_ERR_BAD_WRITE;
    }
    return RTMP_ERR_NONE;
}

rtmp_err_t rtmp_chunk_emit_shake_1( ors_data_t output, unsigned int timestamp, const byte* nonce, size_t length){
    byte zero[4] = {0,0,0,0};
    byte timestamp_out[4];
    ntoh_write_ud( timestamp_out, timestamp );
    if( ors_data_write( output, timestamp_out, 4 ) < 0){
        return RTMP_ERR_BAD_WRITE;
    }
    if( ors_data_write( output, zero, 4 ) < 0 ){
        return RTMP_ERR_BAD_WRITE;
    }
    if( ors_data_write( output, nonce, length ) < 0 ){
        return RTMP_ERR_BAD_WRITE;
    }
    return RTMP_ERR_NONE;
}

rtmp_err_t rtmp_chunk_emit_shake_2( ors_data_t output, unsigned int timestamp1, unsigned int timestamp2, const byte* nonce, size_t length){
    byte timestamp1_out[4];
    byte timestamp2_out[4];
    ntoh_write_ud( timestamp1_out, timestamp1 );
    ntoh_write_ud( timestamp2_out, timestamp2 );
    if( ors_data_write( output, timestamp1_out, 4 ) < 0){
        return RTMP_ERR_BAD_WRITE;
    }
    if( ors_data_write( output, timestamp2_out, 4 ) < 0 ){
        return RTMP_ERR_BAD_WRITE;
    }
    if( ors_data_write( output, nonce, length ) < 0 ){
        return RTMP_ERR_BAD_WRITE;
    }
    return RTMP_ERR_NONE;
}


rtmp_err_t rtmp_chunk_read_shake_0( ors_data_t input ){
    byte version;
    int result = ors_data_read( input, &version, 1 );
    if( result < 0 || version != 3 ){
        return RTMP_ERR_BAD_READ;
    }
    return RTMP_ERR_NONE;
}

rtmp_err_t rtmp_chunk_read_shake_1( ors_data_t input, unsigned int *timestamp, byte* nonce, size_t length){
    byte timestamp_in[4];
    byte zero[4];
    if( ors_data_read( input, timestamp_in, 4 ) < 0){
        return RTMP_ERR_BAD_READ;
    }
    if( ors_data_read( input, zero, 4 ) < 0 ){
        return RTMP_ERR_BAD_READ;
    }
    if( ors_data_read( input, nonce, length ) < 0 ){
        return RTMP_ERR_BAD_READ;
    }
    *timestamp = ntoh_read_ud( timestamp_in );
    return RTMP_ERR_NONE;
}

rtmp_err_t rtmp_chunk_read_shake_2( ors_data_t input, unsigned int *timestamp1, unsigned int *timestamp2, byte* nonce, size_t length){
    byte timestamp1_in[4];
    byte timestamp2_in[4];
    if( ors_data_read( input, timestamp1_in, 4 ) < 0){
        return RTMP_ERR_BAD_READ;
    }
    if( ors_data_read( input, timestamp2_in, 4 ) < 0 ){
        return RTMP_ERR_BAD_READ;
    }
    if( ors_data_read( input, nonce, length ) < 0 ){
        return RTMP_ERR_BAD_READ;
    }
    *timestamp1 = ntoh_read_ud( timestamp1_in );
    *timestamp2 = ntoh_read_ud( timestamp2_in );
    return RTMP_ERR_NONE;
}

rtmp_err_t rtmp_chunk_emit_hdr( ors_data_t output, rtmp_chunk_stream_message_t *message, rtmp_chunk_stream_message_t cache[RTMP_STREAM_CACHE_MAX] ){
    byte fmt = 0;
    unsigned int timestamp = message->timestamp;
    if( message->chunk_stream_id < RTMP_STREAM_CACHE_MAX ){
        rtmp_chunk_stream_message_t *previous = &cache[message->chunk_stream_id];
        if( previous->message_stream_id == message->message_stream_id ){
            fmt = 1;
            if( previous->message_type == message->message_type &&
                previous->message_length == message->message_length ){
                fmt = 2;
                if( previous->timestamp == message->timestamp ){
                    fmt = 3;
                }
            }
        }
        if( fmt > 0 ){
            timestamp = timestamp_get_delta( previous->timestamp, timestamp );
        }
    }
    else{
        printf("Trying to use more chunk streams than have been allocated for caching. (Using %d / %d)\n", message->chunk_stream_id, RTMP_STREAM_CACHE_MAX );
        return RTMP_ERR_INADEQUATE_CHUNK;
    }
    byte buffer[16];
    size_t position = 0;
    if( rtmp_chunk_emit_hdr_basic( output, fmt, message->chunk_stream_id ) != RTMP_ERR_NONE ){
        return RTMP_ERR_BAD_WRITE;
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
    if(ors_data_write( output, buffer, position ) < 0 ){
        return RTMP_ERR_BAD_WRITE;
    }
    return RTMP_ERR_NONE;
}

rtmp_err_t rtmp_chunk_read_hdr( ors_data_t input, rtmp_chunk_stream_message_t **message_out, rtmp_chunk_stream_message_t cache[RTMP_STREAM_CACHE_MAX] ){
    size_t id;
    byte fmt;
    unsigned int old_time = 0;
    if( rtmp_chunk_read_hdr_basic( input, &fmt, &id ) >= RTMP_ERR_ERROR ){
        return RTMP_ERR_BAD_READ;
    }
    if( id < RTMP_STREAM_CACHE_MAX ){
        old_time = cache[id].timestamp;
    }
    else{
        printf("Trying to use more chunk streams than have been allocated for caching. (Using %lu / %d)\n", id, RTMP_STREAM_CACHE_MAX );
        return RTMP_ERR_INADEQUATE_CHUNK;
    }
    rtmp_chunk_stream_message_t *message = &cache[id];
    message->chunk_stream_id = id;
    byte buffer[4];
    unsigned int new_time = 0;
    if( fmt <= 2 ){
        if( ors_data_read( input, buffer, 3) < 0 ){
            return RTMP_ERR_BAD_READ;
        }
        new_time = ntoh_read_ud3( buffer );
    }
    if( fmt <= 1 ){
        if( ors_data_read( input, buffer, 4) < 0 ){
            return RTMP_ERR_BAD_READ;
        }
        message->message_length = ntoh_read_ud3( buffer );
        message->message_type = buffer[3];
    }
    if( fmt <= 0 ){
        if( ors_data_read( input, buffer, 4) < 0 ){
            return RTMP_ERR_BAD_READ;
        }
        message->message_stream_id = ltoh_read_ud( buffer );
    }
    if( ( fmt == 3 && old_time == 0xFFFFFF ) || new_time == 0xFFFFFF ){
        if( ors_data_read( input, buffer, 4) < 0 ){
            return RTMP_ERR_BAD_READ;
        }
        new_time = ntoh_read_ud( buffer );
    }
    if( fmt == 0 ){
        message->timestamp = new_time;
    }
    else{
        message->timestamp = cache[id].timestamp + new_time;
    }
    *message_out = message;
    return RTMP_ERR_NONE;
}

rtmp_err_t rtmp_chunk_emit_hdr_basic( ors_data_t output, byte format, size_t id ){
    byte buffer[3];
    int len = 1;
    //Fill the two least significant bits of buffer[0] with format
    buffer[0] = (format & 3) << 6;
    if( id < 2 ){
        //id 0 and 1 are reserved
        return RTMP_ERR_INVALID;
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
        return RTMP_ERR_INVALID;
    }
    if( ors_data_write( output, buffer, len ) < 0 ){
        return RTMP_ERR_BAD_WRITE;
    }
    return RTMP_ERR_NONE;
}

rtmp_err_t rtmp_chunk_read_hdr_basic( ors_data_t input, byte *format, size_t *id ){
    byte buffer[3];
    if( ors_data_read( input, buffer, 1) < 0 ){
        return RTMP_ERR_BAD_READ;
    }
    *format = (buffer[0] >> 6) & 3;
    buffer[0] = buffer[0] & 63;
    if( buffer[0] > 1 ){
        *id = buffer[0];
        return RTMP_ERR_NONE;
    }
    else if( buffer[0] == 0 ){
        if( ors_data_read( input, buffer + 1, 1) < 0 ){
            return RTMP_ERR_BAD_READ;
        }
        *id = buffer[1];
        *id += 64;
        return RTMP_ERR_NONE;
    }
    else { //buffer[0] must be 1
        if( ors_data_read( input, buffer + 1, 2) < 0 ){
            return RTMP_ERR_BAD_READ;
        }
        *id = buffer[1];
        *id <<= 8;
        *id |= buffer[2];
        *id += 64;
        return RTMP_ERR_NONE;
    }
}



void rtmp_print_message( rtmp_chunk_stream_message_t *msg ){
    printf("Message: \n"
            "\tChunk Stream:   %d\n"
            "\tMessage Stream: %d\n"
            "\tTimestamp:      %d\n"
            "\tType:           %s\n"
            "\tMessage Length: %lu\n\n",
            msg->chunk_stream_id,
            msg->message_stream_id,
            msg->timestamp,
            rtmp_get_message_type_name(msg->message_type),
            msg->message_length );
}
