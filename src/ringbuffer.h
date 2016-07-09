/*
    ringbuffer.h

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

typedef struct ringbuffer * ringbuffer_t;

ringbuffer_t ringbuffer_create( unsigned int size);
void ringbuffer_destroy( ringbuffer_t buffer );
void* ringbuffer_get_write_buf( ringbuffer_t buffer, unsigned int *size );
const void* ringbuffer_get_read_buf( ringbuffer_t buffer, unsigned int *size );
unsigned int ringbuffer_commit_write( ringbuffer_t buffer, unsigned int len );
unsigned int ringbuffer_commit_read( ringbuffer_t buffer, unsigned int len );
void ringbuffer_expand( ringbuffer_t buffer, unsigned int amount );
void ringbuffer_shrink( ringbuffer_t buffer, unsigned int amount );
void ringbuffer_resize( ringbuffer_t buffer, unsigned int amount );
unsigned int ringbuffer_size( ringbuffer_t buffer );
unsigned int ringbuffer_count( ringbuffer_t buffer );
void ringbuffer_freeze_read( ringbuffer_t buffer );
void ringbuffer_freeze_write( ringbuffer_t buffer );
unsigned int ringbuffer_unfreeze_read( ringbuffer_t buffer, char commit );
unsigned int ringbuffer_unfreeze_write( ringbuffer_t buffer, char commit );
unsigned int ringbuffer_copy_read( ringbuffer_t buffer, void *dst, unsigned int length );
unsigned int ringbuffer_copy_write( ringbuffer_t buffer, const void *src, unsigned int length );
