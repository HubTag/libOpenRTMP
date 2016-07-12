/*
    memutil.c

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
#define posix1993
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "memutil.h"


//memcpy that will reverse byte order if the machine is little endian
void ntoh_memcpy(void *dst, const void *src, size_t len){
    #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        byte *output = dst;
        const byte *input = src;
        while( len --> 0 ){
            *(output++) = input[len];
        }
    #elif __BYTE_ORDER__ == __ORDER_PDP_ENDIAN__
        #error "Intentionally unsupported"
    #else
        memcpy( dst, src, len );
    #endif
}

//Read a short from a buffer based on endianess
short ntoh_read_s(const void *src){
    short value;
    ntoh_memcpy( &value, src, 2 );
    return value;
}

//Read an int from a buffer based on endianess
int ntoh_read_d(const void *src){
    int value;
    ntoh_memcpy( &value, src, 4 );
    return value;
}

//Read a short from a buffer based on endianess
uint16_t ntoh_read_us(const void *src){
    uint16_t value;
    ntoh_memcpy( &value, src, 2 );
    return value;
}

//Read an int from a buffer based on endianess
uint32_t ntoh_read_ud(const void *src){
    uint32_t value;
    ntoh_memcpy( &value, src, sizeof( uint32_t ) );
    return value;
}

//Write a short to a buffer based on endianess
void ntoh_write_s(void *src, short value){
    ntoh_memcpy( src, &value, sizeof( value ) );
}

//Write an int to a buffer based on endianess
void ntoh_write_d(void *src, int value){
    ntoh_memcpy( src, &value, sizeof( value ) );
}

//Write a short to a buffer based on endianess
void ntoh_write_us(void *src, uint16_t value){
    ntoh_memcpy( src, &value, sizeof( value ) );
}

//Write an int to a buffer based on endianess
void ntoh_write_ud(void *src, uint32_t value){
    ntoh_memcpy( src, &value, sizeof( value ) );
}

void htol_write_ud(void *dst, uint32_t value){
    memcpy( dst, &value, sizeof( value ));
}
uint32_t ltoh_read_ud(void *src){
    uint32_t value;
    memcpy( &value, src, sizeof( uint32_t ));
    return value;
}

//Write an int to a buffer based on endianess
void ntoh_write_ud3(void *dst, uint32_t value){
    byte *dstb = dst;
    dstb[0] = (value >> 16 ) & 255;
    dstb[1] = (value >> 8 ) & 255;
    dstb[2] = (value ) & 255;
}
//Read a 3 byte int from a buffer based on endianess
uint32_t ntoh_read_ud3(void *src){
    uint32_t ret;
    byte *srcb = src;
    ret = srcb[0];
    ret <<= 8;
    ret |= srcb[1];
    ret <<= 8;
    ret |= srcb[2];
    return ret;
}


void emit_err(const char* err){
    printf("%s\n", err);
}

//Get the difference between timestamps. This is dependent on wrapping behavior of ints
//Such that 0xFFFF0000 to 0x0000FFFF evaluates to 0x0001FFFF
int32_t timestamp_get_delta( rtmp_time_t stamp1, rtmp_time_t stamp2 ){
    return (int32_t) (stamp2 - stamp1);
}

//Generate a nonce of size length, and fill nonce with a pointer to the new memory.
//nonce must either be a valid nonce or nullptr.
rtmp_err_t rtmp_nonce_gen(void **nonce, size_t length){
    if( rtmp_nonce_alloc( nonce, length ) >= RTMP_ERR_ERROR ){
        return RTMP_ERR_OOM;
    }
    char* dst = *nonce;
    while( length --> 0 ){
        *dst = rand() % 256;
        ++dst;
    }
    return RTMP_ERR_NONE;
}

//Delete a nonce.
rtmp_err_t rtmp_nonce_del(void **nonce){
    if( *nonce != nullptr ){
        free(*nonce);
        *nonce = nullptr;
    }
    return RTMP_ERR_NONE;
}

//Allocate memory for a nonce. Nonce must be either a valid nonce or nullptr.
rtmp_err_t rtmp_nonce_alloc(void **nonce, size_t length){
    rtmp_nonce_del(nonce);
    *nonce = malloc(length * sizeof(char));
    if( *nonce == nullptr ){
        return RTMP_ERR_OOM;
    }
    return RTMP_ERR_NONE;
}

//Allocate memory provided the requested amount is under a predetermined amount.
byte* safe_alloc(size_t amount){
    if( amount <= RTMP_MAX_ALLOC ){
        return malloc( amount );
    }
    return nullptr;
}

//Return a millisecond timestamp
rtmp_time_t rtmp_get_time( void ){
    //Using POSIX stuff here.
    //Unfortunately there's no C99 way to get subsecond timestamps...
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    unsigned long long s = t.tv_sec;
    s = si_convert_ull(s, si_none, si_nano);
    s += t.tv_nsec;
    s = si_convert_ull(s, si_nano, si_milli);
    printf("%llu\n", s);
    return s;
}

//Static array of powers of ten for use in si_convert
static const unsigned long long pow10[] = {
    1ull,
    10ull,
    100ull,
    1000ull,
    10000ull,
    100000ull,
    1000000ull,
    10000000ull,
    100000000ull,
    1000000000ull,
    10000000000ull,
    100000000000ull,
    1000000000000ull,
    10000000000000ull,
    100000000000000ull,
    1000000000000000ull,
    10000000000000000ull,
    100000000000000000ull,
    1000000000000000000ull,
    10000000000000000000ull
};
unsigned long long si_convert_ull(unsigned long long number, const si_prefix from, const si_prefix to){
    //Get change in magnitude
    int change = from - to;
    if( change < 0 ){
        //If the change is negative
        if( change < -20 ){
            //Prevent overflow
            change = -20;
        }
        //Divide by 10^-change
        return number / pow10[-change];
    }
    if( change > 20 ){
        //prevent overflow
        change = 20;
    }
    //Multiply by 10^change
    return number * pow10[change];
}
