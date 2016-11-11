/*
    rtmp_server.h

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

#ifndef RTMP_H_SERVER_H
#define RTMP_H_SERVER_H

#ifdef __cplusplus
extern "C" {
#endif


/*! \addtogroup rtmp_ref RTMP
    @{ */
/*! \struct     rtmp_server_t
    \brief      An RTMP streaming server connection.
    \remarks    This object is used to provide the basic functions of an RTMP streaming server connection.
 */
typedef struct rtmp_server * rtmp_server_t;

/*! @} */

#include <openrtmp/rtmp/rtmp_types.h>
#include <openrtmp/rtmp/rtmp_app.h>
#include <openrtmp/rtmp.h>


/*! \brief      Creates an RTMP server object.
    \noparam
    \return     If the function succeeds, the return value is an RTMP server object.
    \return     If the function fails, the return value is `nullptr`.
    \remarks    This is currently only really used internally.
    \memberof   rtmp_server_t
*/
rtmp_server_t rtmp_server_create( void );

/*! \brief      Destroys an RTMP server connection object.
    \param      server  A valid server connection object to destroy. \n
                        Passing an invalid server connection, including `nullptr`, is undefined.
    \noreturn
    \remarks    This is currently only really used internally. RTMP managers currently manage the lifetime of RTMP server connections.
    \memberof   rtmp_server_t
*/
void rtmp_server_destroy( rtmp_server_t server );

/*! \brief      Check if the server is connected.
    \param      server  The server to check.
    \return     The return value is a boolean which is true if the server is connected, false otherwise.
    \remarks    This is basically the equivalent of running `rtmp_chunk_conn_connected()` on the underlying chunk connection object.
    \memberof   rtmp_server_t
*/
bool rtmp_server_connected( rtmp_server_t server );

/*! \brief      Get the underlying RTMP stream for a server connection object.
    \param      server  The target server connection object.
    \return     The return value is the underlying RTMP stream object for \a server.
    \memberof   rtmp_server_t
*/
rtmp_stream_t rtmp_server_stream( rtmp_server_t server );

/*! \brief      Set the app list for a connection.
    \param      server  The target server connection object.
    \param      list    The app list which will replace `server`'s current app list.
    \noreturn
    \remarks    The effects of this change are immediate. All subsequent actions on this connection will be run through the new app
                list, even if events have already transpired under the logic of the old list. Take care when changing an app list in
                the middle of an active connection.
    \memberof   rtmp_server_t
*/
void rtmp_server_set_app_list( rtmp_server_t server, rtmp_app_list_t list );


#ifdef __cplusplus
}
#endif

#endif
