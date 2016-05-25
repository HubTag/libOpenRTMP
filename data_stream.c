/*
    data_stream.c

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

#include <stdlib.h>
#include <string.h>
#include "data_stream.h"
#include <stdio.h>

#define min(a, b) ((a)<(b)?(a):(b))
#define max(a, b) ((a)>(b)?(a):(b))
#define abs(a) ((a)<0?-(a):(a))

#define PEEK_MAX 20
#define PEEK_TYPE char

struct ors_data {
    ors_io_t *vtable;
    byte peek_buf[PEEK_MAX];
    PEEK_TYPE peek_len;
};

#define C(d) ((struct ors_data*)d)

int dummy_read_write(void *data, byte *out, unsigned int out_len){
    return -1;
}
int dummy_single(void *data){
    return -1;
}
int dummy_seek(void *data, int amount, enum ORS_SEEK whence ){
    return -1;
}

void ors_io_prepare( ors_io_t *io ){
    io->do_read = dummy_read_write;
    io->do_write = dummy_read_write;
    io->do_close = dummy_single;
    io->is_eof = dummy_single;
    io->get_remainder = dummy_single;
    io->do_tell = dummy_single;
    io->do_seek = dummy_seek;
}

int ors_data_read( ors_data_t descriptor, byte *out, unsigned int out_len ){
    int len = 0;
    if( C(descriptor)->peek_len > 0 ){
        len = min(C(descriptor)->peek_len, out_len);
        memmove(
            out,
            C(descriptor)->peek_buf,
            len
        );
        memmove(
            C(descriptor)->peek_buf,
            C(descriptor)->peek_buf + len,
            C(descriptor)->peek_len - len);
        out_len -= len;
        out += len;
        C(descriptor)->peek_len -= len;
    }
    if( out_len == 0 ){
        return 0;
    }
    return C(descriptor)->vtable->do_read(
        descriptor,
        out,
        out_len
    ) + len;
}

int ors_data_write( ors_data_t descriptor, byte *in, unsigned int in_len ){
    return C(descriptor)->vtable->do_write(
        descriptor,
        in,
        in_len
    );
}

int ors_data_close( ors_data_t descriptor ){
    return C(descriptor)->vtable->do_close(
        descriptor
    );
}

int ors_data_eof( ors_data_t descriptor ){
    return C(descriptor)->vtable->is_eof(
        descriptor
    );
}

int ors_data_get_remainder( ors_data_t descriptor ){
    return C(descriptor)->vtable->get_remainder(
        descriptor
    );
}

int ors_data_tell( ors_data_t descriptor ){
    return C(descriptor)->vtable->do_tell(
        descriptor
    );
}

int ors_data_seek( ors_data_t descriptor, int amount, enum ORS_SEEK whence ){
    return C(descriptor)->vtable->do_seek(
        descriptor,
        amount,
        whence
    );
}

void ors_data_destroy(ors_data_t descriptor){
    free( descriptor );
}

int ors_data_peek( ors_data_t descriptor, byte *out, unsigned int out_len ){
    out_len = min( PEEK_MAX, out_len );
    int amt = ors_data_read( descriptor, C(descriptor)->peek_buf, out_len);
    C(descriptor)->peek_len = max( 0, amt );
    memcpy( out, C(descriptor)->peek_buf, C(descriptor)->peek_len);
    return C(descriptor)->peek_len;
}

int ors_data_flush_peek( ors_data_t descriptor ){
    C(descriptor)->peek_len = 0;
    return C(descriptor)->peek_len;
}


struct ors_data_memsrc{
    struct ors_data data;
    void *ptr;
    unsigned int offset;
    unsigned int length;
};

struct ors_data_memsnk{
    struct ors_data_memsrc parent;
    unsigned int reserve;
};

static int ors_data_memsrc_read(void *data, byte *out, unsigned int out_len){
    struct ors_data_memsrc *this = data;
    if( this->offset + out_len >= this->length ){
        out_len = this->length - this->offset;
    }
    memcpy( out, this->ptr + this->offset, out_len );
    this->offset += out_len;
    return out_len;
}

static int ors_data_memsrc_eof(void *data){
    struct ors_data_memsrc *this = data;
    return this->offset == this->length;
}

static int ors_data_memsrc_get_remainder(void *data){
    struct ors_data_memsrc *this = data;
    return this->length - this->offset;
}

static int ors_data_memsrc_tell(void *data){
    struct ors_data_memsrc *this = data;
    return this->offset;
}


static int ors_data_memsrc_seek(void *data, int amount, enum ORS_SEEK whence){
    struct ors_data_memsrc *this = data;
    switch( whence ){
        case ORS_SEEK_START:
            this->offset = min(this->length, abs(amount));
            break;
        case ORS_SEEK_OFFSET:
            this->offset = min(this->length, max(this->offset + amount, 0));
            break;
        case ORS_SEEK_END:
            this->offset = min(this->length, max(this->length - abs(amount), 0 ));
            break;
    }
    return this->offset;
}


ors_io_t ors_memsrc_vtable = {
    .do_read = ors_data_memsrc_read,
    .do_write = dummy_read_write,
    .do_close = dummy_single,
    .is_eof = ors_data_memsrc_eof,
    .get_remainder = ors_data_memsrc_get_remainder,
    .do_tell = ors_data_memsrc_tell,
    .do_seek = ors_data_memsrc_seek
};


ors_data_t ors_data_create_memsrc(void *memory, unsigned int length){
    struct ors_data_memsrc *data = malloc( sizeof( struct ors_data_memsrc ) );
    data->data.vtable = &ors_memsrc_vtable;
    data->ptr = memory;
    data->offset = 0;
    data->length = length;
    return data;
}

static int ors_data_memsnk_write(void *data, byte *in, unsigned int in_len){
    struct ors_data_memsnk *this = data;
    const int newlen = this->parent.offset + in_len;
    if( newlen >= this->reserve ){
        this->reserve = newlen * 5/3;
        void *result = realloc( this->parent.ptr, this->reserve );
        if( result == NULL ){
            return -1;
        }
        this->parent.ptr = result;
    }
    memcpy( this->parent.ptr + this->parent.offset, in, in_len );
    this->parent.offset += in_len;
    this->parent.length = max(this->parent.length, newlen);
    return in_len;
}

static int ors_data_memsnk_close(void *data){
    struct ors_data_memsnk *this = data;
    this->parent.offset = 0;
    this->parent.length = 0;
    this->reserve = 0;
    free( this->parent.ptr );
    this->parent.ptr = NULL;
    return 0;
}

ors_io_t ors_memsnk_vtable = {
    .do_read = ors_data_memsrc_read,
    .do_write = ors_data_memsnk_write,
    .do_close = ors_data_memsnk_close,
    .is_eof = ors_data_memsrc_eof,
    .get_remainder = ors_data_memsrc_get_remainder,
    .do_tell = ors_data_memsrc_tell,
    .do_seek = ors_data_memsrc_seek
};

ors_data_t ors_data_create_memsnk(size_t reserve){
    struct ors_data_memsnk *data = malloc( sizeof( struct ors_data_memsnk ) );
    data->parent.data.vtable = &ors_memsnk_vtable;
    data->reserve = max( 1, reserve );
    data->parent.ptr = malloc(reserve * sizeof(byte));
    data->parent.offset = 0;
    data->parent.length = 0;
    return data;
}
