/*
    ringbuffer.c

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
#include <stdio.h>
#include "ringbuffer.h"

#define READ_FROZEN(a) (a&1)
#define WRITE_FROZEN(a) (a&2)
#define READ_FREEZE(a) (a|=1)
#define WRITE_FREEZE(a) (a|=2)
#define READ_UNFREEZE(a) (a&=~1)
#define WRITE_UNFREEZE(a) (a&=~2)


struct ringbuffer{
    char *data;
    unsigned long read_offset;
    unsigned long write_offset;
    char frozen;
    unsigned long start;
    unsigned long len;
    unsigned long size;
};

ringbuffer_t ringbuffer_create( unsigned long size){
    ringbuffer_t buf = malloc( sizeof( struct ringbuffer ) );
    buf->data = malloc( size );
    buf->start = buf->len = 0;
    buf->size = size;
    buf->read_offset = 0;
    buf->write_offset = 0;
    return buf;
}

void ringbuffer_destroy( ringbuffer_t buffer ){
    free( buffer->data );
    free( buffer );
}

void* ringbuffer_get_write_buf( ringbuffer_t buffer, unsigned long *size ){
    unsigned long offset = (buffer->start + buffer->len + buffer->write_offset) % buffer->size;
    unsigned long length = buffer->size - buffer->len - buffer->write_offset;
    void *buff = buffer->data + offset;
    if( size ){
        //If the length of the buffer doesn't fall off the end, return the length as-is
        if( offset + length < buffer->size ){
            *size = length;
        }
        //Otherwise return the length from the write offset to the end of the allocated buffer
        else{
            *size = buffer->size - offset;
        }
    }
    return buff;
}
const void* ringbuffer_get_read_buf( ringbuffer_t buffer, unsigned long *size ){
    unsigned long offset = (buffer->start + buffer->read_offset) % buffer->size;
    unsigned long length = buffer->len + buffer->write_offset - buffer->read_offset;
    void *buff = buffer->data + offset;
    if( size ){
        //If the length of the read buffer doesn't fall off of the end, return the length as-is
        if( offset + length < buffer->size ){
            *size = length;
        }
        //Otherwise, return the distance from the read offset to the end of the allocated buffer
        else{
            *size = buffer->size - offset;
        }
    }
    return buff;
}
unsigned long ringbuffer_commit_write( ringbuffer_t buffer, unsigned long len ){
    if( WRITE_FROZEN(buffer->frozen) ){
        unsigned long ret = buffer->write_offset;
        if( len + buffer->len + buffer->write_offset > buffer->size ){
            //If writes are frozen and this write would overfill the buffer, move the write offset to the very end
            buffer->write_offset = buffer->size - buffer->len;
        }
        else{
            //otherwise just increment the write offset by len
            buffer->write_offset += len;
        }
        return buffer->write_offset - ret;
    }
    else{
        unsigned long ret = buffer->len;
        if( len + buffer->len > buffer->size ){
            //If the write would extend beyond the limit of the buffer, set the length to the max buffer size
            buffer->len = buffer->size;
        }
        else{
            //Otherwise just increment the buffer length by len
            buffer->len += len;
        }
        return buffer->len - ret;
    }
}

unsigned long ringbuffer_commit_read( ringbuffer_t buffer, unsigned long len ){
    //If the read would extend past the end of the content, resize len to fit.
    if( len + buffer->read_offset > buffer->len + buffer->write_offset ){
        len = buffer->len + buffer->write_offset - buffer->read_offset;
    }
    if( READ_FROZEN(buffer->frozen) ){
        buffer->read_offset += len;
        //len + buffer->read_offset is guaranteed to be less than or equal to buffer->len + buffer->write_offset
    }
    else{
        buffer->start = (buffer->start + len) % buffer->size;
        buffer->len -= len;
    }
    return len;
}

unsigned long ringbuffer_count( ringbuffer_t buffer ){
    return buffer->len + buffer->write_offset - buffer->read_offset;
}

void ringbuffer_expand( ringbuffer_t buffer, unsigned long amount ){
    ringbuffer_resize( buffer, buffer->size + amount );
}

void ringbuffer_shrink( ringbuffer_t buffer, unsigned long amount ){
    if( amount > buffer->size ){
        amount = buffer->size;
    }
    ringbuffer_resize( buffer, buffer->size - amount );
}

void ringbuffer_resize( ringbuffer_t buffer, unsigned long amount ){
    if( amount > buffer->size ){
        //Expand memory then move data
        buffer->data = realloc( buffer->data, amount );
        //Get number of bytes hanging off the end
        unsigned long offset = buffer->len + buffer->start + buffer->write_offset;
        unsigned long overlap = offset % buffer->size;
        //If there are some hanging off, determine how many bytes to move from the start of the buffer
        //to the new space added to the end.
        if( offset > buffer->size ){
            unsigned long new_space = amount - buffer->size;
            if( new_space > overlap ){
                new_space = overlap;
            }
            //Move the first bytes of the buffer to the new area
            memmove( buffer->data + buffer->size, buffer->data, new_space );
            //If there's any data left at the beginning, move it down to the start of the array
            memmove( buffer->data, buffer->data + new_space, overlap - new_space);
        }
    }
    else if( amount < buffer->size ){
        //Move data then shrink memory
        unsigned long commit = 0;
        //Determine number of bytes to trim off the start
        if( amount < buffer->len + buffer->write_offset ){
            commit = buffer->len + buffer->write_offset - amount;
        }
        //Trim them
        ringbuffer_commit_read( buffer, commit );
        //Get number of bytes hanging off the end
        unsigned long offset = buffer->len + buffer->start + buffer->write_offset;
        unsigned long overlap = offset % buffer->size;
        if( offset > buffer->size ){
            //If any, move them to where they would be if start was zero
            memmove( buffer->data + buffer->len + buffer->write_offset - overlap, buffer->data, overlap );
        }
        //Move everything between start and size to the front of the buffer
        //Since the previous move is only done if the data overlaps the end, this will not clobber anything.
        memmove( buffer->data, buffer->data + buffer->start, buffer->size - buffer->start );
        buffer->data = realloc( buffer->data, amount );
    }
    buffer->size = amount;
}

unsigned long ringbuffer_size( ringbuffer_t buffer ){
    return buffer->size;
}

void ringbuffer_freeze_read( ringbuffer_t buffer ){
    READ_FREEZE(buffer->frozen);
}

unsigned long ringbuffer_unfreeze_read( ringbuffer_t buffer, char commit ){
    unsigned long total = buffer->read_offset;
    READ_UNFREEZE(buffer->frozen);
    buffer->read_offset = 0;
    if( commit ){
        ringbuffer_commit_read( buffer, total );
    }
    else{
        total = 0;
    }
    return total;
}

void ringbuffer_freeze_write( ringbuffer_t buffer ){
    WRITE_FREEZE(buffer->frozen);
}

unsigned long ringbuffer_unfreeze_write( ringbuffer_t buffer, char commit ){
    unsigned long total = 0;
    WRITE_UNFREEZE(buffer->frozen);
    if( commit ){
        total = ringbuffer_commit_write( buffer, buffer->write_offset );
    }
    buffer->write_offset = 0;
    return total;
}

unsigned long ringbuffer_copy_read( ringbuffer_t buffer, void *dst, unsigned long length ){
    unsigned long size;
    char *d = dst;
    unsigned long read = 0;
    const void* buff = ringbuffer_get_read_buf( buffer, &size );
    //If the read buffer has more data than was requested, we only need a simple memcpy
    if( size >= length ){
        memcpy( dst, buff, length );
        read = length;
        ringbuffer_commit_read( buffer, read );
    }
    else{
        //Copy the data which was made available
        memcpy( dst, buff, size );
        read = size;
        length -= size;
        //Commit it and try to fetch another buffer
        ringbuffer_commit_read( buffer, read );
        buff = ringbuffer_get_read_buf( buffer, &size );
        //If the new buffer is smaller than the remaining read, trim the remaining length to fit
        if( length > size ){
            length = size;
        }
        //Copy from the read buffer into the dest buffer offset by the amount which has already been read
        memcpy( d + read, buff, length );
        //Increment the amount read and commit the read
        read += length;
        ringbuffer_commit_read( buffer, length );
    }
    return read;
}

unsigned long ringbuffer_copy_write( ringbuffer_t buffer, const void *src, unsigned long length ){
    unsigned long size;
    char *s = (char*)src;
    unsigned long wrote = 0;
    void* buff = ringbuffer_get_write_buf( buffer, &size );
    //If the buffer has enough space to consume the whole write, we just need a simple memcpy
    if( size >= length ){
        memcpy( buff, src, length );
        wrote = length;
        ringbuffer_commit_write( buffer, wrote );
    }
    else{
        //Fill the write buffer with the first size bytes from src
        memcpy( buff, src, size );
        wrote = size;
        length -= size;
        //Commit the write and fetch a new buffer
        ringbuffer_commit_write( buffer, wrote );
        buff = ringbuffer_get_write_buf( buffer, &size );
        //If the write buffer isn't big enough to consume the rest of the write, trim the amount to fit
        if( length > size ){
            length = size;
        }
        //Copy from the source buffer offset by the amount already written into the write buffer
        memcpy( buff, s + wrote, length );
        wrote += length;
        //Commit the amount which was written
        ringbuffer_commit_write( buffer, length );
    }
    return wrote;
}
