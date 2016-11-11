/*
    rtmp_app.h

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

#ifndef RTMP_H_APP_H
#define RTMP_H_APP_H

#ifdef __cplusplus
extern "C" {
#endif



/*! \addtogroup rtmp_ref RTMP
    @{ */

/*! \struct     rtmp_app_t
    \brief      An RTMP application.
    \remarks    \parblock
                This object is used to store common callbacks for use when talking to clients as a server.
                \endparblock
*/
typedef struct rtmp_app * rtmp_app_t;

/*! \struct     rtmp_app_list_t
    \brief      A list of RTMP applications.
    \remarks    \parblock
                This object is used to store RTMP applications by name so that they may be associated with an RTMP manager,
                and subsequently, RTMP connections.
                \endparblock
*/
typedef struct rtmp_app_list * rtmp_app_list_t;
/*! @} */

#include <openrtmp/rtmp/rtmp_types.h>
#include <openrtmp/rtmp.h>

/*! \brief      This callback is used by apps to indicate the receipt of an AMF-based RTMP message.
    \param      stream  The stream that this callback was fired from.
    \param      app     The app which this callback was registered with.
    \param      args    An AMF object containing data for an AMF-based RTMP message.
    \param      user    A user-defined pointer which was registered with the app.
    \return     See \ref callback_semantics
    \remarks    This is the callback used for all AMF-based RTMP messages. No distinction is made between
                shared object messages, command messages, or data messages.
    \memberof   rtmp_app_t
*/
typedef rtmp_cb_status_t (*rtmp_app_on_amf_proc)(
    rtmp_stream_t stream,
    rtmp_app_t app,
    amf_t args,
    void *user
);

/*! \brief      This callback is used by apps to indicate the receipt of an \ref rpc_fcpublish remote procedure call.
    \param      stream  The stream that this callback was fired from.
    \param      app     The app which this callback was registered with.
    \param      name    The name of the stream. Often called the stream key.
    \param      user    A user-defined pointer which was registered with the app.
    \return     See \ref callback_semantics
    \memberof   rtmp_app_t
*/
typedef rtmp_cb_status_t (*rtmp_app_on_fcpub_proc)(
    rtmp_stream_t stream,
    rtmp_app_t app,
    const char * name,
    void *user
);

/*! \brief      This callback is used by apps to indicate the receipt of a \ref rpc_publish remote procedure call.
    \param      stream  The stream that this callback was fired from.
    \param      app     The app which this callback was registered with.
    \param      name    The name of the stream. Often called the stream key.
    \param      type    \parblock
                        A string indicating the type of stream this is.

                        Common values are "live", "record", and "append". See Publishing.
                        \endparblock
    \param      user    A user-defined pointer which was registered with the app.
    \return     See \ref callback_semantics
    \memberof   rtmp_app_t
*/
typedef rtmp_cb_status_t (*rtmp_app_on_pub_proc)(
    rtmp_stream_t stream,
    rtmp_app_t app,
    const char * name,
    const char * type,
    void *user
);

/*! \brief      This callback is used by apps to indicate the receipt of audiovisual content.
    \param      stream      The stream that this callback was fired from.
    \param      app         The app which this callback was registered with.
    \param      streamid    The message stream that originated this message.
    \param      timestamp   The timestamp for this message.
    \param      av_data     A buffer containing a raw payload of audiovisual content.
    \param      av_length   The length of \a av_data.
    \param      final_part  \parblock
                            Indicates that this is the final part of a message.

                            Messages are sometimes fragmented and returned over multiple calls to `rtmp_app_on_av_proc`.
                            \a final_part is only set to true once the final fragment has been delivered to the relevant callback.

                            If a message is small enough to not be fragmented, \a final_part is true on the first call of this callback
                            for the message.
                            \endparblock
    \param      user    A user-defined pointer which was registered with the app.
    \return     See \ref callback_semantics
    \memberof   rtmp_app_t
*/
typedef rtmp_cb_status_t (*rtmp_app_on_av_proc)(
    rtmp_stream_t stream,
    rtmp_app_t app,
    size_t streamid,
    rtmp_time_t timestamp,
    const void * av_data,
    size_t av_length,
    bool final_part,
    void *user
);

/*! \brief      Creates an app list.
    \noparam
    \return     If the function succeeds, an app list is constructed and returned.
    \return     If the function fails, `nullptr` is returned.
    \memberof   rtmp_app_list_t
 */
rtmp_app_list_t rtmp_app_list_create( void );

/*! \brief      Destroys an app list.
    \param      list    \parblock
                        A valid list object.
                        Passing an invalid list object, including `nullptr`, is undefined behavior.
                        \endparblock
    \noreturn
    \remarks    Be very careful when destroying app lists. There is currently no reference counting on app lists,
                so any outstanding connections must not have their app lists deleted. Make sure you've disconnected
                all connections which make use of the app list before destroying the list.
    \memberof   rtmp_app_list_t
 */
void rtmp_app_list_destroy( rtmp_app_list_t list );


/*! \brief      Registers a new application with the app list.
    \param      list    The app list to register an application with.
    \param      appname The name of the application to register.
    \return     If this function succeeds, the return value is a valid app object.
    \return     If this function fails, `nullptr` is returned.
    \memberof   rtmp_app_list_t
 */
rtmp_app_t rtmp_app_list_register( rtmp_app_list_t list, const char *appname );

/*! \brief      Gets an app object by name.
    \param      list    The app list to fetch the app from.
    \param      appname The name of the app to fetch.
    \return     If there is an app with a name that matches \a appname, then that app is returned.
    \return     If no such app exists, `nullptr` is returned.
    \memberof   rtmp_app_list_t
 */
rtmp_app_t rtmp_app_list_get( rtmp_app_list_t list, const char *appname );


/*! \brief      Sets the connect callback.
    \param      app     The app to register the callback with.
    \param      proc    The callback procedure to register.
    \param      user    An optional pointer to pass into the callback.
    \noreturn
    \memberof   rtmp_app_t
*/
void rtmp_app_set_connect( rtmp_app_t app, rtmp_app_on_amf_proc proc, void *user );

/*! \brief      Sets the disconnect callback.
    \param      app     The app to register the callback with.
    \param      proc    The callback procedure to register.
    \param      user    An optional pointer to pass into the callback.
    \noreturn
    \memberof   rtmp_app_t
*/
void rtmp_app_set_disconnect( rtmp_app_t app, rtmp_app_on_amf_proc proc, void *user );

/*! \brief      Sets the releaseStream callback.
    \param      app     The app to register the callback with.
    \param      proc    The callback procedure to register.
    \param      user    An optional pointer to pass into the callback.
    \noreturn
    \memberof   rtmp_app_t
*/
void rtmp_app_set_release( rtmp_app_t app, rtmp_app_on_amf_proc proc, void *user );

/*! \brief      Sets the FCPublish callback.
    \param      app     The app to register the callback with.
    \param      proc    The callback procedure to register.
    \param      user    An optional pointer to pass into the callback.
    \noreturn
    \memberof   rtmp_app_t
*/
void rtmp_app_set_fcpublish( rtmp_app_t app, rtmp_app_on_fcpub_proc proc, void *user );

/*! \brief      Sets the publish callback.
    \param      app     The app to register the callback with.
    \param      proc    The callback procedure to register.
    \param      user    An optional pointer to pass into the callback.
    \noreturn
    \memberof   rtmp_app_t
*/
void rtmp_app_set_publish( rtmp_app_t app, rtmp_app_on_pub_proc proc, void *user );

/*! \brief      Sets the onMetadata callback.
    \param      app     The app to register the callback with.
    \param      proc    The callback procedure to register.
    \param      user    An optional pointer to pass into the callback.
    \noreturn
    \memberof   rtmp_app_t
*/
void rtmp_app_set_metadata( rtmp_app_t app, rtmp_app_on_amf_proc proc, void *user );

/*! \brief      Sets the video callback.
    \param      app     The app to register the callback with.
    \param      proc    The callback procedure to register.
    \param      user    An optional pointer to pass into the callback.
    \noreturn
    \memberof   rtmp_app_t
*/
void rtmp_app_set_video( rtmp_app_t app, rtmp_app_on_av_proc proc, void *user );

/*! \brief      Sets the audio callback.
    \param      app     The app to register the callback with.
    \param      proc    The callback procedure to register.
    \param      user    An optional pointer to pass into the callback.
    \noreturn
    \memberof   rtmp_app_t
*/
void rtmp_app_set_audio( rtmp_app_t app, rtmp_app_on_av_proc proc, void *user );


/*! \brief      Fires the connect callback on an app.
    \param      stream  The RTMP stream to fire the connect event on.
    \param      app     The app to use for firing the callback.
    \param      params  The AMF parameters.
    \return     This function returns the result of the callback.
    \return     If no callback was specified, the return value is `RTMP_CB_CONTINUE`.
    \return     See \ref callback_semantics
    \memberof   rtmp_app_t
*/
rtmp_cb_status_t rtmp_app_connect( rtmp_stream_t stream, rtmp_app_t app, amf_t params );

/*! \brief      Fires the disconnect callback on an app.
    \param      stream  The RTMP stream to fire the disconnect event on.
    \param      app     The app to use for firing the callback.
    \param      params  The AMF parameters.
    \return     This function returns the result of the callback.
    \return     If no callback was specified, the return value is `RTMP_CB_CONTINUE`.
    \return     See \ref callback_semantics
    \memberof   rtmp_app_t
*/
rtmp_cb_status_t rtmp_app_disconnect( rtmp_stream_t stream, rtmp_app_t app, amf_t params  );

/*! \brief      Fires the release callback on an app.
    \param      stream  The RTMP stream to fire the release event on.
    \param      app     The app to use for firing the callback.
    \param      params  The AMF parameters.
    \return     This function returns the result of the callback.
    \return     If no callback was specified, the return value is `RTMP_CB_CONTINUE`.
    \return     See \ref callback_semantics
    \memberof   rtmp_app_t
*/
rtmp_cb_status_t rtmp_app_release( rtmp_stream_t stream, rtmp_app_t app, amf_t params  );

/*! \brief      Fires the FCPublish callback on an app.
    \param      stream  The RTMP stream to fire the FCPublish event on.
    \param      app     The app to use for firing the callback.
    \param      name    The name of the stream. Also referred to as the stream key.
    \return     This function returns the result of the callback.
    \return     If no callback was specified, the return value is `RTMP_CB_CONTINUE`.
    \return     See \ref callback_semantics
    \memberof   rtmp_app_t
*/
rtmp_cb_status_t rtmp_app_fcpublish( rtmp_stream_t stream, rtmp_app_t app, const char * name );

/*! \brief      Fires the publish callback on an app.
    \param      stream  The RTMP stream to fire the publish event on.
    \param      app     The app to use for firing the callback.
    \param      name    The name of the stream. Also referred to as the stream key.
    \param      type    The type of the broadcast. \n
                        Common values are "live", "record", and "append". See Publishing.
    \return     This function returns the result of the callback.
    \return     If no callback was specified, the return value is `RTMP_CB_CONTINUE`.
    \return     See \ref callback_semantics
    \memberof   rtmp_app_t
*/
rtmp_cb_status_t rtmp_app_publish( rtmp_stream_t stream, rtmp_app_t app, const char * name, const char * type );

/*! \brief      Fires the onMetadata callback on an app.
    \param      stream  The RTMP stream to fire the onMetadata event on.
    \param      app     The app to use for firing the callback.
    \param      params  The AMF parameters.
    \return     This function returns the result of the callback.
    \return     If no callback was specified, the return value is `RTMP_CB_CONTINUE`.
    \return     See \ref callback_semantics
    \memberof   rtmp_app_t
*/
rtmp_cb_status_t rtmp_app_metadata( rtmp_stream_t stream, rtmp_app_t app, amf_t params  );

/*! \brief      Fires the the video callback on an app.
    \param      stream      The RTMP stream to fire the video event on.
    \param      app         The app to use for firing the callback.
    \param      streamid    The message stream that originated this message.
    \param      timestamp   The timestamp for this message.
    \param      av_data     A buffer containing a raw payload of video content.
    \param      av_length   The length of \a av_data.
    \param      final_part  \parblock
                            Indicates that this is the final part of a message.

                            Messages are sometimes fragmented and returned over multiple calls to `rtmp_app_on_av_proc`.
                            \a final_part is only set to true once the final fragment has been delivered to the relevant callback.

                            If a message is small enough to not be fragmented, \a final_part is true on the first call of this callback
                            for the message.
                            \endparblock
    \return     This function returns the result of the callback.
    \return     If no callback was specified, the return value is `RTMP_CB_CONTINUE`.
    \return     See \ref callback_semantics
    \memberof   rtmp_app_t
*/
rtmp_cb_status_t rtmp_app_video( rtmp_stream_t stream, rtmp_app_t app, size_t streamid, rtmp_time_t timestamp, const void * av_data, size_t av_length, bool final_part );

/*! \brief      Fires the the audio callback on an app.
    \param      stream      The RTMP stream to fire the audio event on.
    \param      app         The app to use for firing the callback.
    \param      streamid    The message stream that originated this message.
    \param      timestamp   The timestamp for this message.
    \param      av_data     A buffer containing a raw payload of audio content.
    \param      av_length   The length of \a av_data.
    \param      final_part  \parblock
                            Indicates that this is the final part of a message.

                            Messages are sometimes fragmented and returned over multiple calls to `rtmp_app_on_av_proc`.
                            \a final_part is only set to true once the final fragment has been delivered to the relevant callback.

                            If a message is small enough to not be fragmented, \a final_part is true on the first call of this callback
                            for the message.
                            \endparblock
    \return     This function returns the result of the callback.
    \return     If no callback was specified, the return value is `RTMP_CB_CONTINUE`.
    \return     See \ref callback_semantics
    \memberof   rtmp_app_t
*/
rtmp_cb_status_t rtmp_app_audio( rtmp_stream_t stream, rtmp_app_t app, size_t streamid, rtmp_time_t timestamp, const void * av_data, size_t av_length, bool final_part );

#ifdef __cplusplus
}
#endif

#endif
