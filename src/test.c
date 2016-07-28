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
#include "amf/amf.h"
#include "rtmp/chunk/rtmp_chunk_conn.h"
#include "rtmp/rtmp_constants.h"
#include "rtmp/chunk/rtmp_chunk_assembler.h"
#include "ringbuffer.h"
#include <stdarg.h>
#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif

rtmp_cb_status_t data_callback(
    rtmp_chunk_conn_t conn,
    const byte * restrict contents,
    size_t available,
    size_t remaining,
    const rtmp_chunk_stream_message_t *msg,
    void * restrict user
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


void logger( rtmp_err_t err, size_t line, const char * restrict file, const char * restrict msg, void* usr ){
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

#include "amf/amf_object.h"
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

struct foo{
    int a;
};



int main(){
    amf_t amf = amf_create( 0 );
    amf_push_object_simple( amf,
        RTMP_ARG_APP, "test",
        RTMP_ARG_FPAD, false,
        RTMP_ARG_VIDEOCODECS, RTMP_SUPPORT_VID_ALL,
        RTMP_ARG_FLASHVER, "1.2.4.4",
        RTMP_ARG_CUSTOM, AMF0_TYPE_NUMBER, "TestThing", 123456.789,
        RTMP_ARG_END );
    amf_print( amf );
    return 0;

    rtmp_chunk_conn_t client = rtmp_chunk_conn_create( true );
    rtmp_chunk_conn_t server = rtmp_chunk_conn_create( false );

    rtmp_chunk_conn_register_callbacks( client, data_callback, nullptr, logger, nullptr );
    rtmp_chunk_conn_register_callbacks( server, data_callback, nullptr, logger, nullptr );

    rtmp_chunk_assembler_t assembler1 = rtmp_chunk_assembler_create( RTMP_MAX_CHUNK_CACHE, data_callback, nullptr, logger, nullptr );
    rtmp_chunk_assembler_assign( assembler1, client );

    rtmp_chunk_assembler_t assembler2 = rtmp_chunk_assembler_create( RTMP_MAX_CHUNK_CACHE, data_callback, nullptr, logger, nullptr );
    rtmp_chunk_assembler_assign( assembler2, server );


    int state = 0;

    while( true ){
        service( client, server );

        if( state == 0 && rtmp_chunk_conn_connected(client) && rtmp_chunk_conn_connected(server) ){
            state = 1;
            rtmp_chunk_conn_set_chunk_size( client, 10 );
            rtmp_chunk_conn_set_chunk_size( server, 10 );
        }
        else if( state == 1 ){
            rtmp_chunk_conn_set_peer_bwidth( client, 10, RTMP_LIMIT_HARD );
            ++state;
        }
        else if( state == 2 ){
            rtmp_chunk_conn_set_chunk_size( server, 10 );
            size_t wrote = 0;
            unsigned char test[5000];
            unsigned char *teststr = (unsigned char*)"Hello there my friend!";
            rtmp_chunk_conn_send_message( server, RTMP_MSG_AUDIO, 2, 1, 0, teststr, 22, nullptr);
            rtmp_chunk_conn_send_message( server, RTMP_MSG_AUDIO, 200, 1, 0, teststr, 22, nullptr);
            rtmp_chunk_conn_send_message( server, RTMP_MSG_AUDIO, 9000, 1, 0, teststr, 22, nullptr);
            rtmp_chunk_conn_send_message( server, RTMP_MSG_AUDIO, 14, 1, 0, teststr, 22, nullptr);
            rtmp_chunk_conn_send_message( server, RTMP_MSG_AUDIO, 55556, 1, 0, teststr, 22, nullptr);
            rtmp_chunk_conn_send_message( server, RTMP_MSG_AUDIO, 55556, 1, 0, teststr, 22, nullptr);
            rtmp_chunk_conn_send_message( server, RTMP_MSG_AUDIO, 55556, 1, 0, teststr, 22, nullptr);
            /*rtmp_chunk_conn_send_message( server, RTMP_MSG_AUDIO, 55556, 1, 0, test, sizeof(test), &wrote);
            while( wrote < sizeof( test ) ){
                rtmp_chunk_conn_send_message( server, RTMP_MSG_AUDIO, 55556, 1, 0, test + wrote, sizeof(test), &wrote);
                service( client, server );
            }*/
            ++state;
        }

        usleep(10000);
    }


    return 0;
}

