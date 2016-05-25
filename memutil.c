#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "memutil.h"

//memcpy that will reverse byte order if the machine is little endian
void ntoh_memcpy(void *dst, const void *src, size_t len){
    #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        byte *output = dst;
        const byte *input = src;
        while( len --> 0 ){
            *(output++) = input[len];
        }
    #elif __BYTE_ORDER__ == __ORDER_PDP_ENDIAN__
        #error "Intentionally unsupported"
    #else
        memcpy( dst, src, len );
    #endif
}

//Read a short from a buffer based on endianess
short ntoh_read_s(const void *src){
    short value;
    ntoh_memcpy( &value, src, 2 );
    return value;
}

//Read an int from a buffer based on endianess
int ntoh_read_d(const void *src){
    int value;
    ntoh_memcpy( &value, src, 4 );
    return value;
}

//Read a short from a buffer based on endianess
unsigned short ntoh_read_us(const void *src){
    unsigned short value;
    ntoh_memcpy( &value, src, 2 );
    return value;
}

//Read an int from a buffer based on endianess
unsigned int ntoh_read_ud(const void *src){
    unsigned int value;
    ntoh_memcpy( &value, src, 4 );
    return value;
}

//Write a short to a buffer based on endianess
void ntoh_write_s(void *src, short value){
    ntoh_memcpy( src, &value, sizeof( value ) );
}

//Write an int to a buffer based on endianess
void ntoh_write_d(void *src, int value){
    ntoh_memcpy( src, &value, sizeof( value ) );
}

//Write a short to a buffer based on endianess
void ntoh_write_us(void *src, unsigned short value){
    ntoh_memcpy( src, &value, sizeof( value ) );
}

//Write an int to a buffer based on endianess
void ntoh_write_ud(void *src, unsigned int value){
    ntoh_memcpy( src, &value, sizeof( value ) );
}


void emit_err(const char* err){
    printf("%s\n", err);
}
