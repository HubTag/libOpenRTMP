/*
    memutil.h

    Copyright (C) 2016 Hubtag LLC.

    ----------------------------------------

    This file is part of libOpenRTMP.

    libOpenRTMP is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    libOpenRTMP is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with libOpenRTMP.  If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once

typedef unsigned char byte;
typedef unsigned long size_t;

//memcpy that will reverse byte order if the machine is little endian
void ntoh_memcpy(void *dst, const void *src, size_t len);

//Read a short from a buffer based on endianess
short ntoh_read_s(const void *src);

//Read an int from a buffer based on endianess
int ntoh_read_d(const void *src);

//Read a short from a buffer based on endianess
unsigned short ntoh_read_us(const void *src);

//Read an int from a buffer based on endianess
unsigned int ntoh_read_ud(const void *src);

//Write a short to a buffer based on endianess
void ntoh_write_s(void *dst, short value);

//Write an int to a buffer based on endianess
void ntoh_write_d(void *dst, int value);

//Write a short to a buffer based on endianess
void ntoh_write_us(void *dst, unsigned short value);

//Write an int to a buffer based on endianess
void ntoh_write_ud(void *dst, unsigned int value);

void htol_write_ud(void *dst, unsigned int value);
unsigned int ltoh_read_ud(void *src);


//Write a 3 byte int to a buffer based on endianess
void ntoh_write_ud3(void *dst, unsigned int value);
//Read a 3 byte int from a buffer based on endianess
unsigned int ntoh_read_ud3(void *dst);

void emit_err(const char* err);

int timestamp_get_delta( unsigned int stamp1, unsigned int stamp2 );

