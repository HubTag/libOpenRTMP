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
    ors_data_t contents,
    size_t available,
    size_t remaining,
    rtmp_chunk_stream_message_t *msg,
    void *user
){
    printf("Avail: %d\tRemain: %d\n", available, remaining );
    byte b[available];
    ors_data_read( contents, b, available );
    for( int i = 0; i < available; ++i ){
        printf("%c", b[i]);
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

int main(){
    ors_data_t client_to_server = ors_data_create_memsnk(10000);
    ors_data_t server_to_client = ors_data_create_memsnk(10000);

    rtmp_chunk_conn_t client = rtmp_chunk_conn_create( true, server_to_client, client_to_server );
    rtmp_chunk_conn_t server = rtmp_chunk_conn_create( false, client_to_server, server_to_client );

    rtmp_chunk_conn_register_callbacks( client, data_callback, nullptr, nullptr );

    size_t client_to_server_read, server_to_client_read;
    client_to_server_read = server_to_client_read = 0;

    int state = 0;

    while( true ){
        ors_data_seek( client_to_server, client_to_server_read, ORS_SEEK_START );
        ors_data_seek( server_to_client, server_to_client_read, ORS_SEEK_START );
        if( !ors_data_eof( client_to_server ) ){
            check_conn_err( rtmp_chunk_conn_service( server, RTMP_IO_IN ) );
        }
        if( !ors_data_eof( server_to_client ) ){
            check_conn_err( rtmp_chunk_conn_service( client, RTMP_IO_IN ) );
        }
        client_to_server_read = ors_data_tell( client_to_server );
        server_to_client_read = ors_data_tell( server_to_client );
        ors_data_seek( client_to_server, 0, ORS_SEEK_END );
        ors_data_seek( server_to_client, 0, ORS_SEEK_END );


        check_conn_err( rtmp_chunk_conn_service( client, RTMP_IO_OUT ) );
        check_conn_err( rtmp_chunk_conn_service( server, RTMP_IO_OUT ) );

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
            rtmp_chunk_conn_set_chunk_size( server, 10 );
            rtmp_chunk_conn_send_message( server, RTMP_MSG_AUDIO, 2, 1, 0, "Hello there my friend!", 22);
            rtmp_chunk_conn_send_message( server, RTMP_MSG_AUDIO, 200, 1, 0, "Hello there my friend!", 22);
            rtmp_chunk_conn_send_message( server, RTMP_MSG_AUDIO, 9000, 1, 0, "Hello there my friend!", 22);
            rtmp_chunk_conn_send_message( server, RTMP_MSG_AUDIO, 14, 1, 0, "Hello there my friend!", 22);
            rtmp_chunk_conn_send_message( server, RTMP_MSG_AUDIO, 55556, 1, 0, "Hello there my friend!", 22);
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
