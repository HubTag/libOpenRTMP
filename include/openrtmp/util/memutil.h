/*
    memutil.h

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

#ifndef RTMP_H_MEMUTIL_H
#define RTMP_H_MEMUTIL_H

#ifdef __cplusplus
extern "C" {
#endif


#include <openrtmp/rtmp/rtmp_types.h>
#include <openrtmp/rtmp/rtmp_constants.h>

#ifdef __cplusplus
#include <type_traits>
#define ezalloc(a) ((decltype(a))calloc(1,sizeof(std::remove_pointer<decltype(a)>::type)))
#else
#define ezalloc(a) (calloc(1,sizeof (*a)))
#endif
#define ALIAS(a,b,c) b c = (b)a;

//memcpy that will reverse byte order if the machine is little endian
void ntoh_memcpy(void * restrict dst, const void * restrict src, size_t len);

//Read a short from a buffer based on endianess
short ntoh_read_s(const void *src);

//Read an int from a buffer based on endianess
int ntoh_read_d(const void *src);

//Read a short from a buffer based on endianess
uint16_t ntoh_read_us(const void *src);

//Read an int from a buffer based on endianess
uint32_t ntoh_read_ud(const void *src);

//Write a short to a buffer based on endianess
void ntoh_write_s(void *dst, short value);

//Write an int to a buffer based on endianess
void ntoh_write_d(void *dst, int value);

//Write a short to a buffer based on endianess
void ntoh_write_us(void *dst, uint16_t value);

//Write an int to a buffer based on endianess
void ntoh_write_ud(void *dst, uint32_t value);

void htol_write_ud(void *dst, uint32_t value);
uint32_t ltoh_read_ud(void *src);


//Write a 3 byte int to a buffer based on endianess
void ntoh_write_ud3(void *dst, uint32_t value);
//Read a 3 byte int from a buffer based on endianess
uint32_t ntoh_read_ud3(void *dst);

//strlen which considers nullptr to be a zero-length string.
size_t strlen_check( const char *str );

//strcpy which returns str + strlen( appending ).
char *strwrite( char * restrict str, const char * restrict appending );

void emit_err(const char* err);

int32_t timestamp_get_delta( rtmp_time_t stamp1, rtmp_time_t stamp2 );

//If generating of allocating a nonce, it MUST be initialized to null!
rtmp_err_t rtmp_nonce_gen(void **nonce, size_t length);
rtmp_err_t rtmp_nonce_alloc(void **nonce, size_t length);
rtmp_err_t rtmp_nonce_del(void **nonce);

rtmp_time_t rtmp_get_time();

typedef enum {
    si_yotta = 24,
    si_zetta = 21,
    si_exa = 18,
    si_peta = 15,
    si_tera = 12,
    si_giga = 9,
    si_mega = 6,
    si_kilo = 3,
    si_none = 0, //Or NULL
    si_milli = -3,
    si_micro = -6,
    si_nano = -9,
    si_pico = -12,
    si_femto = -15,
    si_atto = -18,
    si_zepto = -21,
    si_yocto = -24
} si_prefix;

unsigned long long si_convert_ull(unsigned long long number, const si_prefix from, const si_prefix to);

byte* safe_alloc(size_t amount);


char * str_dupl( const char * input, size_t len );
char * str_dup( const char * input );

#ifndef RTMP_HAS_STRNCASECMP
int strncasecmp( const char * a, const char * b, size_t len );
#endif

#ifdef __cplusplus
}
#endif

#endif
