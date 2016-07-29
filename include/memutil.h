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

#pragma once

#include "rtmp/rtmp_types.h"
#include "rtmp/rtmp_constants.h"

typedef struct{
    char *scheme;
    char *user;
    char *password;
    char *host;
    char *port;
    char *path;
    char *query;
    char *fragment;
    char *allocated;
} url_t;

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

void emit_err(const char* err);

int32_t timestamp_get_delta( rtmp_time_t stamp1, rtmp_time_t stamp2 );

//If generating of allocating a nonce, it MUST be initialized to null!
rtmp_err_t rtmp_nonce_gen(void **nonce, size_t length);
rtmp_err_t rtmp_nonce_alloc(void **nonce, size_t length);
rtmp_err_t rtmp_nonce_del(void **nonce);

rtmp_time_t rtmp_get_time();

unsigned long long si_convert_ull(unsigned long long number, const si_prefix from, const si_prefix to);

byte* safe_alloc(size_t amount);

double read_double_ieee(const void *ptr);
void write_double_ieee(void *ptr, double value);

bool parse_url( const char *url, url_t *out, bool force_host );
