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
#include "amf.h"
#include "rtmp_chunk_conn.h"
#include "rtmp_constants.h"



rtmp_cb_status_t data_callback(
    rtmp_chunk_conn_t conn,
    byte *contents,
    size_t available,
    size_t remaining,
    rtmp_chunk_stream_message_t *msg,
    void *user
){
    printf("Avail: %d\tRemain: %d\n", available, remaining );
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
            "Message Length: %d\n"
            "Message Type: %s (%d)\n"
            "Steam ID: %d\n\n",
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

#include "ringbuffer.h"
#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif

void service( rtmp_chunk_conn_t client, rtmp_chunk_conn_t server ){
    const void *from;
    size_t from_size;
    const void *to;
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

int main(){
    ors_data_t client_to_server = ors_data_create_memsnk(10000);
    ors_data_t server_to_client = ors_data_create_memsnk(10000);

    rtmp_chunk_conn_t client = rtmp_chunk_conn_create( true );
    rtmp_chunk_conn_t server = rtmp_chunk_conn_create( false );

    rtmp_chunk_conn_register_callbacks( client, data_callback, nullptr, nullptr );

    size_t client_to_server_read, server_to_client_read;
    client_to_server_read = server_to_client_read = 0;

    int state = 0;

    while( true ){
        service( client, server );

        if( state == 0 && rtmp_chunk_conn_connected(client) && rtmp_chunk_conn_connected(server) ){
            state = 1;
            rtmp_chunk_conn_set_chunk_size( client, 200 );
            rtmp_chunk_conn_set_chunk_size( server, 1000 );
        }
        else if( state == 1 ){
            rtmp_chunk_conn_set_peer_bwidth( client, 2000, RTMP_LIMIT_HARD );
            ++state;
        }
        else if( state == 2 ){
            rtmp_chunk_conn_set_chunk_size( server, 1000 );
            size_t wrote;
            char test[5000];
            rtmp_chunk_conn_send_message( server, RTMP_MSG_AUDIO, 2, 1, 0, "Hello there my friend!", 22, nullptr);
            rtmp_chunk_conn_send_message( server, RTMP_MSG_AUDIO, 200, 1, 0, "Hello there my friend!", 22, nullptr);
            rtmp_chunk_conn_send_message( server, RTMP_MSG_AUDIO, 9000, 1, 0, "Hello there my friend!", 22, nullptr);
            rtmp_chunk_conn_send_message( server, RTMP_MSG_AUDIO, 14, 1, 0, "Hello there my friend!", 22, nullptr);
            rtmp_chunk_conn_send_message( server, RTMP_MSG_AUDIO, 55556, 1, 0, "Hello there my friend!", 22, nullptr);
            rtmp_chunk_conn_send_message( server, RTMP_MSG_AUDIO, 55556, 1, 0, "Hello there my friend!", 22, nullptr);
            rtmp_chunk_conn_send_message( server, RTMP_MSG_AUDIO, 55556, 1, 0, "Hello there my friend!", 22, nullptr);
            rtmp_chunk_conn_send_message( server, RTMP_MSG_AUDIO, 55556, 1, 0, test, sizeof(test), &wrote);
            size_t total = wrote;
            while( total < sizeof( test ) ){
                rtmp_chunk_conn_send_message( server, RTMP_MSG_AUDIO, 55556, 1, 0, test + total, sizeof(test) - total, &wrote);
                total += wrote;
                service( client, server );
            }
            ++state;
        }

        usleep(10000);
        //printf( "%d, %d\n\n", client->lag, server->lag );
    }
    #if 0
    ors_data_t client_out = ors_data_create_file("data2", "rb");
    char nonce1[1528], nonce2[1528];
    unsigned int time1, time2;
    rtmp_chunk_stream_message_t cache[RTMP_STREAM_CACHE_MAX];
    memset( cache, 0, sizeof( cache ) );

    check( rtmp_chunk_read_shake_0( client_out ) );
    check( rtmp_chunk_read_shake_1( client_out, &time1, nonce1, 1528 ) );
    check( rtmp_chunk_read_shake_2( client_out, &time1, &time2, nonce2, 1528 ) );
    rtmp_chunk_stream_message_t *msg;
    while( ors_data_eof( client_out ) == 0 ){
        check_chunk( rtmp_chunk_read_hdr( client_out, &msg, cache ), &msg );
        if( !msg ){
            continue;
        }

        if( msg->message_type == RTMP_MSG_AMF0_CMD || msg->message_type == RTMP_MSG_AMF0_SO || msg->message_type == RTMP_MSG_AMF0_DAT ){
            amf0_print( client_out, msg->message_length, rtmp_default_printer );
        }
        else{
            byte buf[msg->message_length];

            ors_data_read( client_out, buf, msg->message_length);
            for( int i = 0; i < msg->message_length; ++i){
                //printf("%02X ", buf[i]);
            }
        }
        printf( "\n");
        //ors_data_seek( client_out, msg->message_length - count, ORS_SEEK_OFFSET );
    }
    #endif
    return 0;
}
