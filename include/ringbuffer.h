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

ringbuffer_t ringbuffer_create( unsigned long size);
void ringbuffer_destroy( ringbuffer_t buffer );

void ringbuffer_clear( ringbuffer_t buffer );

//Returns a void* buffer for writing. Max write to buffer is returned in size.
//The buffer returned is not guaranteed to be as long as the total unused
//capacity of the buffer.
void* ringbuffer_get_write_buf( ringbuffer_t buffer, unsigned long *size );

//Returns a void* buffer for reading. Max read from buffer is returned in size.
//The buffer returned is not guaranteed to be as long as the total remaining
//unread content held in the buffer.
const void* ringbuffer_get_read_buf( ringbuffer_t buffer, unsigned long *size );

//Commits a write to the buffer and returns how many bytes were actually committed.
//Len specifies how many bytes were written by the caller.
unsigned long ringbuffer_commit_write( ringbuffer_t buffer, unsigned long len );

//Commits a read of len bytes to the buffer. Returns number of bytes actually committed.
unsigned long ringbuffer_commit_read( ringbuffer_t buffer, unsigned long len );

//Expands the capacity of the buffer by amount bytes.
void ringbuffer_expand( ringbuffer_t buffer, unsigned long amount );

//Shrinks the capacity of the buffer by amount bytes. This might trim data
//from the start of the buffer.
void ringbuffer_shrink( ringbuffer_t buffer, unsigned long amount );

//Changes the capacity of the buffer. This might trim data from the start of the buffer.
void ringbuffer_resize( ringbuffer_t buffer, unsigned long amount );

//Returns the total capacity of the buffer.
unsigned long ringbuffer_size( ringbuffer_t buffer );

//Returns how many bytes are available for reading in the buffer.
unsigned long ringbuffer_count( ringbuffer_t buffer );

//Prepare the buffer for rolling back a series of read operations.
void ringbuffer_freeze_read( ringbuffer_t buffer );

//Prepare the buffer for rolling back a series of write operations.
void ringbuffer_freeze_write( ringbuffer_t buffer );

//Unfreeze future read operations. If commit is false, undo any reads made since
//the last call to freeze. Otherwise, commit all reads since the last call to freeze.
//Returns the total number of bytes committed upon calling.
unsigned long ringbuffer_unfreeze_read( ringbuffer_t buffer, char commit );

//Unfreeze future write operations. If commit is false, undo any writes made since the
//last call to freeze. Otherwise, commit all writes made since the last call to freeze.
//Returns the total number of bytes committed upon calling.
unsigned long ringbuffer_unfreeze_write( ringbuffer_t buffer, char commit );

//Read and copy up to length bytes from the buffer into dst. This is guaranteed to read length
//bytes to dst so long as the buffer has the data available to do so. Any issues with the buffer
//wrapping is handled internally.
unsigned long ringbuffer_copy_read( ringbuffer_t buffer, void *dst, unsigned long length );

//Copy up to length bytes from src to the buffer. This is guaranteed to write length bytes to
//the buffer so long as the buffer has the free space available. Any issues with the buffer
//wrapping is handled internally.
unsigned long ringbuffer_copy_write( ringbuffer_t buffer, const void *src, unsigned long length );
