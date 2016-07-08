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

struct ringbuffer{
    char *data;
    unsigned int start;
    unsigned int len;
    unsigned int size;
};

ringbuffer_t ringbuffer_create( unsigned int size){
    ringbuffer_t buf = malloc( sizeof( struct ringbuffer ) );
    buf->data = malloc( size );
    buf->start = buf->len = 0;
    buf->size = size;
    return buf;
}

void ringbuffer_destroy( ringbuffer_t buffer ){
    free( buffer->data );
    free( buffer );
}

void* ringbuffer_get_write_buf( ringbuffer_t buffer, unsigned int *size ){
    unsigned int offset = (buffer->start + buffer->len) % buffer->size;
    unsigned int length = buffer->size - buffer->len;
    void *buff = buffer->data + offset;
    if( size ){
        if( offset + length < buffer->size ){
            *size = length;
        }
        else{
            *size = buffer->size - offset;
        }
    }
    return buff;
}
const void* ringbuffer_get_read_buf( ringbuffer_t buffer, unsigned int *size ){
    unsigned int offset = buffer->start;
    unsigned int length = buffer->len;
    void *buff = buffer->data + offset;
    if( size ){
        if( offset + length < buffer->size ){
            *size = length;
        }
        else{
            *size = buffer->size - offset;
        }
    }
    return buff;
}
void ringbuffer_commit_write( ringbuffer_t buffer, unsigned int len ){
    if( len + buffer->len > buffer->size ){
        buffer->len = buffer->size;
    }
    else{
        buffer->len += len;
    }
}

void ringbuffer_commit_read( ringbuffer_t buffer, unsigned int len ){
    if( len > buffer->len ){
        len = buffer->len;
    }
    buffer->start = (buffer->start + len) % buffer->size;
    buffer->len -= len;
}

unsigned int ringbuffer_count( ringbuffer_t buffer ){
    return buffer->len;
}

void ringbuffer_expand( ringbuffer_t buffer, unsigned int amount ){
    ringbuffer_resize( buffer, buffer->size + amount );
}

void ringbuffer_shrink( ringbuffer_t buffer, unsigned int amount ){
    if( amount > buffer->size ){
        amount = buffer->size;
    }
    ringbuffer_resize( buffer, buffer->size - amount );
}

void ringbuffer_resize( ringbuffer_t buffer, unsigned int amount ){
    if( amount > buffer->size ){
        //Expand memory then move data
        buffer->data = realloc( buffer->data, amount );
        //Get number of bytes hanging off the end
        unsigned int offset = buffer->len + buffer->start;
        unsigned int overlap = offset % buffer->size;
        //If there are some hanging off, determine how many bytes to move from the start of the buffer
        //to the new space added to the end.
        if( offset > buffer->size ){
            unsigned int new_space = amount - buffer->size;
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
        unsigned int commit = 0;
        //Determine number of bytes to trim off the start
        if( amount < buffer->len ){
            commit = buffer->len - amount;
        }
        //Trim them
        ringbuffer_commit_read( buffer, commit );
        //Get number of bytes hanging off the end
        unsigned int offset = buffer->len + buffer->start;
        unsigned int overlap = offset % buffer->size;
        if( offset > buffer->size ){
            //If any, move them to where they would be if start was zero
            memmove( buffer->data + buffer->len - overlap, buffer->data, overlap );
        }
        //Move everything between start and size to the front of the buffer
        //Since the previous move is only done if the data overlaps the end, this will not clobber anything.
        memmove( buffer->data, buffer->data + buffer->start, buffer->size - buffer->start );
        buffer->data = realloc( buffer->data, amount );
    }
    buffer->size = amount;
}

unsigned int ringbuffer_size( ringbuffer_t buffer ){
    return buffer->size;
}
