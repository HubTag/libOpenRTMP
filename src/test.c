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
#include "amf/amf_object.h"
#include "rtmp/chunk/rtmp_chunk_conn.h"
#include "rtmp/rtmp_constants.h"
#include "rtmp.h"
#include "rtmp/chunk/rtmp_chunk_assembler.h"
#include "ringbuffer.h"
#include <stdarg.h>
#include "parseurl.h"
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


void print_url(parseurl_t url){
    const char *urlstr = parseurl_get( url, PARSEURL_URL, "N/A");
    printf( "Asm\t%s\n", urlstr );
    printf("Scheme\t%s\n"
           "User\t%s\n"
           "Pass\t%s\n"
           "Host\t%s\n"
           "Port\t%s\n"
           "Path\t%s\n"
           "Query\t%s\n"
           "Frag\t%s\n\n",
           parseurl_get( url, PARSEURL_SCHEME, "N/A"),
           parseurl_get( url, PARSEURL_USER, "N/A"),
           parseurl_get( url, PARSEURL_PASS, "N/A"),
           parseurl_get( url, PARSEURL_HOST, "N/A"),
           parseurl_get( url, PARSEURL_PORT, "N/A"),
           parseurl_get( url, PARSEURL_PATH, "N/A"),
           parseurl_get( url, PARSEURL_QUERY, "N/A"),
           parseurl_get( url, PARSEURL_FRAGMENT, "N/A") );
}
void parse_n_print( const char* url ){
    parseurl_t url_p = parseurl_create();
    parseurl_set( url_p, PARSEURL_URL_STRICT, url );
    printf("URL\t%s\n", url);
    print_url( url_p );

    parseurl_destroy( url_p );
}

static  rtmp_cb_status_t connect_proc(
    rtmp_server_t connection,
    void *user;
){
    printf("Got connection\n");
    return RTMP_CB_CONTINUE;
}

int main(){


    int *vec = nullptr;
    size_t len = 0, reserve = 0;
    for( size_t i = 0; i < 1000; ++i ){
        int *ptr = VEC_MK_SPACE(vec, len, reserve );
        if( ptr ){
            *ptr = i;
        }
    }
    for( size_t i = 0; i < len; ++i ){
        printf("%d\n", vec[i] );
    }
    rtmp_t rtmp = rtmp_create();
    rtmp_listen( rtmp, "0.0.0.0", 1935, connect_proc, nullptr );
    while( true ){
        rtmp_service( rtmp, -1 );
        printf("Looped\n");
    }

    amf_t amf = amf_create(0);
    amf_push_simple( amf,
        AMF(
            AMF_DBL(12.0),
            AMF_STR("Hello there"),
            AMF_OBJ(
                AMF_STR("foo", "bar"),
                AMF_STR("another", "one"),
                AMF_BOOL("bool_is", true),
                AMF_OBJ("nested",
                    AMF_DBL("Year", 2016.0),
                    AMF_DBL("Port", 12345.0)
                ),
                AMF_STR("end", "IS NEAR!")
            ),
            AMF_INT(42)
        )
    );
    amf_print( amf );
    amf_destroy( amf );
    return 0;

    parse_n_print("ftp://ftp.is.co.za/rfc/rfc1808.txt");
    parse_n_print("http://www.ietf.org/rfc/rfc2396.txt");
    parse_n_print("ldap://[2001:db8::7]/c=GB?objectClass?one");
    parse_n_print("mailto:John.Doe@example.com");
    parse_n_print("news:comp.infosystems.www.servers.unix");
    parse_n_print("tel:+1-816-555-1212");
    parse_n_print("telnet://192.0.2.16:80/");
    parse_n_print("urn:oasis:names:specification:docbook:dtd:xml:4.1.2");

    srand(time(0));
    size_t count = 2000000000;
    char chars[] = "abasd[][][]:::?f23asfa:?/:#:&//1@@/?a";
    char url[100];
    for( size_t i = 0; i < count; ++i ){
        size_t len = rand() % 40;
        for( size_t j = 0; j < len; ++j ){
            url[j] = chars[rand() % (sizeof( chars )-1) ];
        }
        url[len] = 0;
        parseurl_t url_p = parseurl_create();
        parseurl_set( url_p, PARSEURL_URL, url );
        parseurl_set( url_p, PARSEURL_URL_FORGIVING, url );
        parseurl_set( url_p, PARSEURL_URL_STRICT, url );
        parseurl_destroy( url_p );
        //parse_n_print( url );
        if( i % 10000 == 0 ){
            printf("%lld / %lld (%lld.%04lld%%)\r", i, count, 100 * i / count, 1000000 * i / count % 10000 );
            fflush(stdout);
        }
    }



    /*amf_t amf = amf_create( 0 );
    amf_push_object_simple( amf,
        RTMP_ARG_APP, "test",
        RTMP_ARG_FPAD, false,
        RTMP_ARG_VIDEOCODECS, RTMP_SUPPORT_VID_ALL,
        RTMP_ARG_FLASHVER, "1.2.4.4",
        RTMP_ARG_CUSTOM, AMF0_TYPE_NUMBER, "TestThing", 123456.789,
        RTMP_ARG_END );
    amf_print( amf );*/
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

