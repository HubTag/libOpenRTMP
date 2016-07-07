/*
    data_stream.h

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

#include "memutil.h"

enum ORS_SEEK {
    ORS_SEEK_START = -1,
    ORS_SEEK_OFFSET,
    ORS_SEEK_END
};

typedef struct ors_io{
    int (*do_read)(void *data, byte *out, unsigned int out_len);
    int (*do_write)(void *data, const byte *in, unsigned int in_len);
    int (*do_close)(void *data);
    int (*is_eof)(void *data);
    int (*get_remainder)(void *data);
    int (*do_tell)(void *data);
    int (*do_seek)(void *data, int amount, enum ORS_SEEK whence );
} ors_io_t;

typedef void* ors_data_t;

void ors_io_prepare( ors_io_t *io );

int ors_data_read( ors_data_t descriptor, byte *out, unsigned int out_len );
int ors_data_write( ors_data_t descriptor, const byte *in, unsigned int in_len );
int ors_data_close( ors_data_t descriptor );
int ors_data_eof( ors_data_t descriptor );
int ors_data_get_remainder( ors_data_t descriptor );
int ors_data_tell( ors_data_t descriptor );
int ors_data_seek( ors_data_t descriptor, int amount, enum ORS_SEEK whence );
int ors_data_peek( ors_data_t descriptor, byte *out, unsigned int out_len );
int ors_data_flush_peek( ors_data_t descriptor );
int ors_data_cap( ors_data_t descriptor, size_t len );
size_t ors_data_amount_read( ors_data_t descriptor );
size_t ors_data_amount_written( ors_data_t descriptor );
void ors_data_reset_read_amount( ors_data_t descriptor, size_t amount );
void ors_data_reset_write_amount( ors_data_t descriptor, size_t amount );

void ors_data_destroy(ors_data_t descriptor);

ors_data_t ors_data_create_memsrc(void *memory, unsigned int length);
ors_data_t ors_data_create_memsnk(size_t reserve);
ors_data_t ors_data_create_file(const char* fname, const char* mode);

