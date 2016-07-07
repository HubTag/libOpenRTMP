/*
    data_stream.c

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
    size_t cap;
    size_t read;
    size_t written;
    byte peek_buf[PEEK_MAX];
    PEEK_TYPE peek_len;
};

#define C(d) ((struct ors_data*)d)

int dummy_write(void *data, const byte *out, unsigned int out_len){
    return -1;
}
int dummy_read(void *data, byte *out, unsigned int out_len){
    return -1;
}
int dummy_single(void *data){
    return -1;
}
int dummy_seek(void *data, int amount, enum ORS_SEEK whence ){
    return -1;
}

void ors_io_prepare( ors_io_t *io ){
    io->do_read = dummy_read;
    io->do_write = dummy_write;
    io->do_close = dummy_single;
    io->is_eof = dummy_single;
    io->get_remainder = dummy_single;
    io->do_tell = dummy_single;
    io->do_seek = dummy_seek;
}


size_t ors_data_amount_read( ors_data_t descriptor ){
    return C(descriptor)->read;
}

size_t ors_data_amount_written( ors_data_t descriptor ){
    return C(descriptor)->written;
}

void ors_data_reset_read_amount( ors_data_t descriptor, size_t amount ){
    if( C(descriptor)->read > amount ){
        C(descriptor)->read -= amount;
    }
    else{
        C(descriptor)->read = 0;
    }
}

void ors_data_reset_write_amount( ors_data_t descriptor, size_t amount ){
    if( C(descriptor)->written > amount ){
        C(descriptor)->written -= amount;
    }
    else{
        C(descriptor)->written = 0;
    }
}

int ors_data_cap( ors_data_t descriptor, size_t len ){
    C(descriptor)->cap = len;
    return 1;
}

int ors_data_read( ors_data_t descriptor, byte *out, unsigned int out_len ){
    C(descriptor)->cap = -1;
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
        if( C(descriptor)->cap != -1 ){
             if( C(descriptor)->cap > len ){
                 C(descriptor)->cap -= len;
             }
             else{
                 C(descriptor)->cap = 0;
             }
        }
        C(descriptor)->read += len;
        return len;
    }
    if( C(descriptor)->cap != -1 && C(descriptor)->cap < out_len + len ){
        out_len = C(descriptor)->cap;
        if( len > out_len ){
            out_len = 0;
        }
        else{
            out_len -= len;
        }
        int ret = C(descriptor)->vtable->do_read(
            descriptor,
            out,
            out_len
        ) + len;
        if( ret > 0 ){
            C(descriptor)->cap -= ret;
            C(descriptor)->read += ret;
        }
        return ret;
    }
    int ret = C(descriptor)->vtable->do_read(
        descriptor,
        out,
        out_len
    ) + len;
    if( ret > 0 ){
        C(descriptor)->read += ret;
    }
    return ret;
}

int ors_data_write( ors_data_t descriptor, const byte *in, unsigned int in_len ){
    int ret = C(descriptor)->vtable->do_write(
        descriptor,
        in,
        in_len
    );
    if( ret > 0 ){
        C(descriptor)->written += ret;
    }
    return ret;
}

int ors_data_close( ors_data_t descriptor ){
    return C(descriptor)->vtable->do_close(
        descriptor
    );
}

int ors_data_eof( ors_data_t descriptor ){
    if( C(descriptor)->cap == 0 ){
        return 1;
    }
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
    if( C(descriptor)->cap != -1 && C(descriptor)->cap < out_len ){
        out_len = C(descriptor)->cap;
    }
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
    .do_write = dummy_write,
    .do_close = dummy_single,
    .is_eof = ors_data_memsrc_eof,
    .get_remainder = ors_data_memsrc_get_remainder,
    .do_tell = ors_data_memsrc_tell,
    .do_seek = ors_data_memsrc_seek
};


ors_data_t ors_data_create_memsrc(void *memory, unsigned int length){
    struct ors_data_memsrc *data = malloc( sizeof( struct ors_data_memsrc ) );
    data->data.vtable = &ors_memsrc_vtable;
    data->data.peek_len = 0;
    data->data.cap = -1;
    data->ptr = memory;
    data->offset = 0;
    data->length = length;
    return data;
}

static int ors_data_memsnk_write(void *data, const byte *in, unsigned int in_len){
    struct ors_data_memsnk *this = data;
    const int newlen = this->parent.offset + in_len;
    if( newlen >= this->reserve ){
        this->reserve = newlen * 5/3;
        void *result = realloc( this->parent.ptr, this->reserve );
        if( result == nullptr ){
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
    this->parent.ptr = nullptr;
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
    data->parent.data.peek_len = 0;
    data->parent.data.cap = -1;
    data->reserve = max( 1, reserve );
    data->parent.ptr = malloc(reserve * sizeof(byte));
    data->parent.offset = 0;
    data->parent.length = 0;
    return data;
}

#include<stdio.h>

struct ors_data_file{
    struct ors_data data;
    FILE *file;
};

static int ors_data_file_read(void *data, byte *out, unsigned int out_len){
    struct ors_data_file *this = data;
    return fread( out, 1, out_len, this->file );
}

static int ors_data_file_eof(void *data){
    struct ors_data_file *this = data;
    return feof( this->file );
}

static int ors_data_file_tell(void *data){
    struct ors_data_file *this = data;
    return ftell( this->file );
}


static int ors_data_file_seek(void *data, int amount, enum ORS_SEEK whence){
    struct ors_data_file *this = data;
    switch( whence ){
        case ORS_SEEK_START:
            fseek( this->file, amount, SEEK_SET );
            break;
        case ORS_SEEK_OFFSET:
            fseek( this->file, amount, SEEK_CUR );
            break;
        case ORS_SEEK_END:
            fseek( this->file, amount, SEEK_END );
            break;
    }
    return ors_data_file_tell( data );
}
static int ors_data_file_write(void *data, const byte *in, unsigned int in_len){
    struct ors_data_file *this = data;
    return fwrite( in, 1, in_len, this->file );
}

static int ors_data_file_close(void *data){
    struct ors_data_file *this = data;
    fclose( this->file );
    return 0;
}

ors_io_t ors_file_vtable = {
    .do_read = ors_data_file_read,
    .do_write = ors_data_file_write,
    .do_close = ors_data_file_close,
    .is_eof = ors_data_file_eof,
    .get_remainder = dummy_single,
    .do_tell = ors_data_file_tell,
    .do_seek = ors_data_file_seek
};

ors_data_t ors_data_create_file(const char* fname, const char* mode){
    struct ors_data_file *data = malloc( sizeof( struct ors_data_file ) );
    data->data.vtable = &ors_file_vtable;
    data->data.cap = -1;
    data->data.peek_len = 0;
    data->file = fopen(fname, mode);
    return data;
}
