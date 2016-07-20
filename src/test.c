/*
    test.c

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

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "../include/amf/amf.h"
#include "../include/rtmp/chunk/rtmp_chunk_conn.h"
#include "../include/rtmp/rtmp_constants.h"
#include "../include/ringbuffer.h"
#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif



rtmp_cb_status_t data_callback(
    rtmp_chunk_conn_t conn,
    const byte *contents,
    size_t available,
    size_t remaining,
    rtmp_chunk_stream_message_t *msg,
    void *user
){
    printf("Avail: %lu\tRemain: %lu\n", available, remaining );
    for( int i = 0; i < available; ++i ){
        printf("%c", contents[i]);
    }
    printf("\n");
    return RTMP_CB_CONTINUE;
}

int check( int value ){
    if( value < 0 ){
        printf("Failed!\n");
    }
    return value;
}
int check_chunk( int value, rtmp_chunk_stream_message_t **message ){
    rtmp_chunk_stream_message_t *msg = *message;
    if( value < 0 ){
        printf("Failed!\n");
        return -1;
    }
    printf( "Chunk Stream: %d\n"
            "Timestamp: %d\n"
            "Message Length: %lu\n"
            "Message Type: %s (%d)\n"
            "Steam ID: %u\n\n",
        msg->chunk_stream_id,
        msg->timestamp,
        msg->message_length,
        rtmp_get_message_type_name(msg->message_type), msg->message_type,
        msg->message_stream_id);
    return value;
}

void check_conn_err( rtmp_err_t err ){
    if( err != RTMP_ERR_NONE ){
        printf("Error - %s\n", rtmp_get_err_name( err ) );
    }
}


void logger( rtmp_err_t err, size_t line, const char* file, const char* msg, void* usr ){
    printf("Logged: [%s:%lu] %s\n", file, line, rtmp_get_err_name( err ) );
}

void service( rtmp_chunk_conn_t client, rtmp_chunk_conn_t server ){
    const void *from;
    size_t from_size;
    void *to;
    size_t to_size;
    size_t transferring;


    //xmit from client to server
    rtmp_chunk_conn_get_out_buff( client, &from, &from_size );
    rtmp_chunk_conn_get_in_buff( server, &to, &to_size );
    transferring = MIN( from_size, to_size );
    memcpy( to, from, transferring );
    rtmp_chunk_conn_commit_out_buff( client, transferring );
    rtmp_chunk_conn_commit_in_buff( server, transferring );

    //xmit from server to client
    rtmp_chunk_conn_get_out_buff( server, &from, &from_size );
    rtmp_chunk_conn_get_in_buff( client, &to, &to_size );
    transferring = MIN( from_size, to_size );
    memcpy( to, from, transferring );
    rtmp_chunk_conn_commit_out_buff( server, transferring );
    rtmp_chunk_conn_commit_in_buff( client, transferring );

    check_conn_err( rtmp_chunk_conn_service( client ) );
    check_conn_err( rtmp_chunk_conn_service( server ) );
}

#include "../include/amf/amf_object.h"
#define do_alloc(c) \
    amf_push_string_alloc( amf, &target, strlen(c) ); memcpy(target, c, strlen(c));

void printhex(byte* p, int len){
    while( len --> 0 ){
        for( int i = 0; i < 8; ++i ){
            printf("%d", ((*p) >> (7-i)) & 1 );
        }
        printf("\t%02X\n", *(p++));
    }
}

int main(){

    void* target;
    amf_t amf = amf_create( 3 );
    amf_t amf2 = amf_create( 3 );
    amf_push_number( amf, 1337.0 );
    amf_push_date( amf, 123, 10 );
    amf_push_boolean( amf, true );
    do_alloc("Hello there!");
    amf_push_long_string( amf, target);
    amf_push_null( amf );
    amf_push_object_start( amf );
        do_alloc( "Foo" );
            amf_push_member( amf, target );
            amf_push_number( amf, 45141 );
        do_alloc( "bar" );
            amf_push_member( amf, target );
            amf_push_object_start( amf );
                do_alloc( "Rawr" );
                amf_push_member( amf, target );
                amf_push_number( amf, 6667 );
                amf_push_object_end( amf );
        do_alloc( "Meow" );
            amf_push_member( amf, target );
            amf_push_number( amf, 6697 );
        amf_push_object_end( amf );
    amf_push_reference( amf, 0 );
    amf_push_undefined( amf );
    amf_push_object_start( amf );
        do_alloc( "Thing" );

            amf_push_member( amf, target );
            amf_push_number( amf, 45141 );
        do_alloc( "Meow" );
            amf_push_member( amf, target );
            amf_push_number( amf, 6697 );
        amf_push_object_end( amf );
    amf_push_undefined( amf );
    do_alloc( "The end!" );
    amf_push_string( amf, target );

    size_t len = amf_write( amf, nullptr, 0, nullptr );
    byte *buff = malloc( len );
    amf_write( amf, buff, len, nullptr );
    printf("amf_print!\n\n");
    amf_print( amf );
    printf("\n\namf0_print!\n\n");
    amf0_print( buff, len, rtmp_default_printer );
    amf_read( amf2, buff, len, nullptr );
    printf("\n\namf_print!\n\n");
    amf_print( amf2 );
    printf( "\n\nTest access: %f", amf_value_get_number( amf_obj_get_value( amf_obj_get_value( amf_get_item(amf, 5), "bar" ), "Rawr" ) ) );
    amf_destroy( amf );
    amf_destroy( amf2 );
    free(buff);


    return 0;
}

