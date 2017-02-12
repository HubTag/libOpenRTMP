/*
    rtmp_chunk_cache.c

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
#include <openrtmp/rtmp/rtmp_chunk_cache.h>
#include <openrtmp/util/memutil.h>
#include <openrtmp/util/algorithm.h>
#include <stdlib.h>

rtmp_chunk_stream_cache_t rtmp_cache_create( void ){
    struct rtmp_chunk_stream_cache *ret = ezalloc( ret );
    for( size_t i = 0; i < RTMP_STREAM_STATIC_CACHE_SIZE; ++i ){
        ret->static_cache[i].msg.chunk_stream_id = i;
    }
    return ret;
}

void rtmp_cache_destroy( rtmp_chunk_stream_cache_t cache ){
    free( cache->dynamic_cache );
    free( cache );
}

void rtmp_cache_reset( rtmp_chunk_stream_cache_t cache ){
    memset( cache->static_cache, 0, sizeof( cache->static_cache ) );
    for( size_t i = 0; i < RTMP_STREAM_STATIC_CACHE_SIZE; ++i ){
        cache->static_cache[i].msg.chunk_stream_id = i;
    }
    for( size_t i = 0; i < cache->dynamic_cache_size; ++i ){
        size_t id = cache->dynamic_cache[i].msg.chunk_stream_id;
        memset( &cache->dynamic_cache[i], 0, sizeof( cache->dynamic_cache[i] ) );
        cache->dynamic_cache[i].msg.chunk_stream_id = id;
    }
}

//Compare the chunk stream IDs for ordering
static bool rtmp_chunk_stream_message_less_than( const void * restrict a, const void * restrict b ){
    return *(const size_t*)a < ((const rtmp_chunk_stream_message_internal_t*)b)->msg.chunk_stream_id;
}

static rtmp_chunk_stream_message_internal_t * rtmp_cache_insert( rtmp_chunk_stream_cache_t cache, size_t index, size_t chunk_id ){
    rtmp_chunk_stream_message_internal_t *ret = nullptr;
    if( cache->dynamic_cache_size >= RTMP_STREAM_CACHE_MAX ){
        //Max stream cache size reached
        return ret;
    }
    size_t remainder = cache->dynamic_cache_size - index;
    cache->dynamic_cache_size ++;
    //Make room for the new element
    void* newptr = realloc( cache->dynamic_cache, sizeof(rtmp_chunk_stream_message_internal_t) * cache->dynamic_cache_size );
    if( newptr == nullptr ){
        //Out of memory
        return ret;
    }
    cache->dynamic_cache = (rtmp_chunk_stream_message_internal_t*)newptr;
    ret = cache->dynamic_cache + index;
    if( remainder > 0 ){
        //If there's data after our index, move it down by one element
        memmove( ret + 1, ret, remainder );
    }
    //Clear the memory for our new element and set its ID
    memset( ret, 0, sizeof( rtmp_chunk_stream_message_internal_t ) );
    ret->msg.chunk_stream_id = chunk_id;
    return ret;
}

rtmp_chunk_stream_message_internal_t * rtmp_cache_get( rtmp_chunk_stream_cache_t cache, size_t chunk_id ){
    if( chunk_id < RTMP_STREAM_STATIC_CACHE_SIZE ){
        //Happy path! If the ID falls in the range of the static allocation, just index into the static array.
        return &cache->static_cache[chunk_id];
    }
    //Search the dynamic array for our element
    size_t idx = alg_search_bin(
            &chunk_id,
            cache->dynamic_cache,
            sizeof(rtmp_chunk_stream_message_internal_t),
            cache->dynamic_cache_size,
            rtmp_chunk_stream_message_less_than
    );
    //If the index is equal to the size of the array, we need to append a new element
    if( idx == cache->dynamic_cache_size ){
        return rtmp_cache_insert( cache, idx, chunk_id );
    }
    else{
        rtmp_chunk_stream_message_internal_t *target = cache->dynamic_cache + idx;
        //If the element at the index we found shares the ID we're looking for, return it
        if( target->msg.chunk_stream_id == chunk_id ){
            return target;
        }
        //Otherwise, insert a new element there
        else{
            return rtmp_cache_insert( cache, idx, chunk_id );
        }
    }
}

static int rtmp_cache_match( rtmp_chunk_stream_message_internal_t * cached, size_t stream_id, size_t msg_size, byte msg_type, rtmp_time_t timestamp ){
    int current_type = 0;
    if( cached->initialized ) {
        if( cached->msg.message_stream_id == stream_id ){
            current_type = 1;
            if( cached->msg.message_length == msg_size && cached->msg.message_type == msg_type ){
                current_type = 2;
                if( cached->time_delta == (rtmp_time_t)timestamp_get_delta( cached->msg.timestamp, timestamp ) ){
                    current_type = 3;
                }
            }
        }
    }
    return current_type;
}

rtmp_chunk_stream_message_internal_t * rtmp_cache_find( rtmp_chunk_stream_cache_t cache, size_t stream_id, size_t msg_size, byte msg_type, rtmp_time_t timestamp ){
    rtmp_chunk_stream_message_internal_t * best = &cache->static_cache[4];
    memset( best, 0, sizeof (rtmp_chunk_stream_message_internal_t) );
    best->msg.chunk_stream_id = 4;
    return best;
    int best_type = 0;
    int current_type = 0;
    for( size_t i = 4; i < RTMP_STREAM_STATIC_CACHE_SIZE; ++i ){
        current_type = rtmp_cache_match( &cache->static_cache[i], stream_id, msg_size, msg_type, timestamp );
        if( current_type > best_type && cache->static_cache[i].initialized ){
            best = &cache->static_cache[i];
            best_type = current_type;
        }
        if( best_type == 3 ){
            return best;
        }
    }
    for( size_t i = 0; i < cache->dynamic_cache_size; ++i ){
        current_type = rtmp_cache_match( &cache->dynamic_cache[i], stream_id, msg_size, msg_type, timestamp );
        if( current_type > best_type && cache->dynamic_cache[i].initialized ){
            best = &cache->dynamic_cache[i];
            best_type = current_type;
        }
        if( best_type == 3 ){
            return best;
        }
    }
    return best;
}
