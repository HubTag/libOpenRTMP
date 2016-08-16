/*
    rtmp_chunk_conn.h

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

#ifndef RTMP_H_CHUNK_CONN_H
#define RTMP_H_CHUNK_CONN_H

#include "rtmp/rtmp_types.h"
#include "rtmp/rtmp_constants.h"
#include "rtmp_chunk_flow.h"

typedef struct rtmp_chunk_conn *rtmp_chunk_conn_t;

//Procedure prototype for handling partial chunk information
typedef rtmp_cb_status_t (*rtmp_chunk_proc)(
    rtmp_chunk_conn_t conn,                 //The originating connection
    const byte * restrict contents,         //The contents of the partial chunk
    size_t available,                       //The number of bytes availavle in contents
    size_t remaining,                       //The number of bytes remaining in this chunk
    const rtmp_chunk_stream_message_t *msg, //The message data
    void * restrict user                    //User-specified data
);

//Procedure prototype for handling chunk stream events
typedef rtmp_cb_status_t (*rtmp_event_proc)(
    rtmp_chunk_conn_t conn,             //The originating connection
    rtmp_event_t event,                 //The event
    void * restrict user                //User-specified data
);

//Procedure prototype for handling logging
typedef void (*rtmp_log_proc)(
    rtmp_err_t err,                     //The error status
    size_t line,                        //The line upon which the log was generated
    const char* restrict file,          //The file from which the log was generated
    const char* restrict message,       //An optional message associated with the log
    void * restrict user                //User-specified data
);


//Create an RTMP chunk connection object.
//The client parameter specifies whether it should act as a client (true) or server (false).
rtmp_chunk_conn_t rtmp_chunk_conn_create( bool client );

//Destroy an RTMP object.
rtmp_err_t rtmp_chunk_conn_close( rtmp_chunk_conn_t conn );

//This should be called every time the buffers are changed.
rtmp_err_t rtmp_chunk_conn_service( rtmp_chunk_conn_t conn );

//Pass in procedure definitions. The user pointer will be sent with all callbacks.
rtmp_err_t rtmp_chunk_conn_register_callbacks( rtmp_chunk_conn_t conn, rtmp_chunk_proc chunk_cb, rtmp_event_proc event_cb, rtmp_log_proc log_cb, void *user );

//Returns true if the chunk stream is connected and has successfully performed a handshake.
bool rtmp_chunk_conn_connected( rtmp_chunk_conn_t conn );

//Set the max chunk size. Generally higher is better, though has little real impact.
rtmp_err_t rtmp_chunk_conn_set_chunk_size( rtmp_chunk_conn_t conn, uint32_t size );

//Tells the peer to stop processing a message on the specified chunk stream.
rtmp_err_t rtmp_chunk_conn_abort( rtmp_chunk_conn_t conn, uint32_t chunk_stream );

//Sends an acknowledgement of the number of bytes received so far.
rtmp_err_t rtmp_chunk_conn_acknowledge( rtmp_chunk_conn_t conn );

//Tells the peer how many bytes will be received prior to an ack being sent.
rtmp_err_t rtmp_chunk_conn_set_window_ack_size( rtmp_chunk_conn_t conn, uint32_t size );

//Asks the peer to change their window size.
rtmp_err_t rtmp_chunk_conn_set_peer_bwidth( rtmp_chunk_conn_t conn, uint32_t size, rtmp_limit_t limit_type );

//Fetches a buffer along with its size used to feed the RTMP chunk connection with data.
rtmp_err_t rtmp_chunk_conn_get_in_buff( rtmp_chunk_conn_t conn, void **buffer, size_t *size );

//Fetches a buffer along with its size used to fetch data from the RTMP chunk connection.
rtmp_err_t rtmp_chunk_conn_get_out_buff( rtmp_chunk_conn_t conn, const void **buffer, size_t *size );

//Inform the connection about how many bytes were written to the buffer.
rtmp_err_t rtmp_chunk_conn_commit_in_buff( rtmp_chunk_conn_t conn, size_t size );

//Inform the connection about how many bytes were read from the buffer.
rtmp_err_t rtmp_chunk_conn_commit_out_buff( rtmp_chunk_conn_t conn, size_t size );

//Send an arbitrary message via the chunk stream.
rtmp_err_t
rtmp_chunk_conn_send_message(
    rtmp_chunk_conn_t conn,
    byte message_type,
    uint32_t chunk_stream,
    uint32_t message_stream,
    uint32_t timestamp,
    const byte * restrict data,
    size_t length,
    size_t *written
);

#endif
