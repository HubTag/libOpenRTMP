/*
    rtmp_client.h

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

#ifndef RTMP_H_CLIENT_H
#define RTMP_H_CLIENT_H

#ifdef __cplusplus
extern "C" {
#endif

/*! \addtogroup rtmp_ref RTMP
    @{ */
/*! \struct     rtmp_client_t
    \brief      The interface for creating RTMP client connections
    \remarks    This is the interface for creating connections to other RTMP servers.
 */
typedef struct rtmp_client * rtmp_client_t;
/*! @} */

#include <openrtmp/rtmp/rtmp_types.h>
#include <openrtmp/rtmp/rtmp_stream.h>
#include <openrtmp/rtmp.h>


/*! \brief      Creates an RTMP client object.
    \param      url         The \ref rtmp_url "RTMP URL" which this client will connect to.
    \param      playpath    The \ref rtmp_playpath "stream name" which will be used by the client.
    \return     If the function succeeds, the return value is an RTMP client object.
    \return     If the function fails, the return value is `nullptr`.
    \remarks    Before this object will do anything, it needs to be connected to a \ref rtmp_t with \ref rtmp_connect
    \memberof   rtmp_client_t
*/
rtmp_client_t rtmp_client_create( const char * url, const char * playpath );

/*! \brief      References an RTMP client object
    \param      client      The client to reference
    \return     Returns \a client.
    \remarks    Calling this function increases an internal reference count for the client. Subsequent calls to
    \remarks    \ref rtmp_client_destroy will only decrement the reference count until it hits zero, at which point
    \remarks    the client will actually be destroyed.
    \memberof   rtmp_client_t
*/
rtmp_client_t rtmp_client_reference( rtmp_client_t client );

/*! \brief      Destroys an RTMP client object.
    \param      client      The client to destroy.
    \noreturn
    \remarks    \parblock
                Be sure to disconnect the client from the \ref rtmp_t with \ref rtmp_disconnect prior to destroying it.

                This function will decrement the reference count for the client. If the reference count hits zero, then
                the client will be destroyed.
                \endparblock
    \memberof   rtmp_client_t
*/
void rtmp_client_destroy( rtmp_client_t client );

rtmp_err_t rtmp_client_disconnect( rtmp_client_t client );
bool rtmp_client_connected( rtmp_client_t client );

/*! \brief      Get the underlying RTMP stream for an RTMP client object.
    \param      client  The target RTMP client object.
    \return     The return value is the underlying RTMP stream object for \a client.
    \memberof   rtmp_client_t
*/
rtmp_stream_t rtmp_client_stream( rtmp_client_t client );

/*! \brief      Returns information about the destination URL for an RTMP client.
    \param      client  The target RTMP client object.
    \param[out] host    Receives the host portion of the destination URL.
    \param[out] port    Receives the port portion of the destination URL.
    \return     Returns a \ref rtmp_err_t code.
    \memberof   rtmp_client_t
*/
rtmp_err_t rtmp_client_get_conninfo( rtmp_client_t client, const char **host, uint16_t * port );

/*! \brief      Returns the \ref rtmp_playpath "playpath" for an RTMP client object.
    \param      client  The target RTMP client object.
    \return     Returns the \ref rtmp_playpath "playpath" for an RTMP client object, if available.
    \memberof   rtmp_client_t
*/
const char * rtmp_client_get_playpath( rtmp_client_t client );

//See NetConnect documentation for usage

/*! \brief      Issues a \ref rpc_connect remote procedure call.
    \param      client      The RTMP client to issue the call with.
    \param      app         The \ref rtmp_app "RTMP app" to connect to.
    \param      swfUrl      Indicates the SWF URL to indicate this connection originates from.
    \param      tcUrl       The page URL to indicate this connection originates from.
    \param      audioCodecs A \ref rtmp_support_codec_snd_t bitset indicating supported sound codecs.
    \param      videoCodecs A \ref rtmp_support_codec_vid_t bitset indicating supported sound codecs.
    \param      proc        A \ref rtmp_stream_amf_proc which will be called upon the receipt of a response to this command.
    \param      userdata    An optional pointer to pass in with the \a proc callback.
    \return     Returns a \ref rtmp_err_t code.
    \remarks    See \ref rpc_connect for usage.
    \memberof   rtmp_client_t
*/
rtmp_err_t rtmp_client_connect(
    rtmp_client_t client,
    const char * restrict app,
    const char * swfUrl,
    const char * tcUrl,
    size_t audioCodecs,
    size_t videoCodecs,
    rtmp_stream_amf_proc proc,
    void *userdata
);

/*! \brief      Issues a \ref rpc_releasestream remote procedure call.
    \param      client      The RTMP client to issue the call with.
    \param      path        The \ref rtmp_playpath "playpath" which should be freed.
    \param      proc        A \ref rtmp_stream_amf_proc which will be called upon the receipt of a response to this command.
    \param      userdata    An optional pointer to pass in with the \a proc callback.
    \return     Returns a \ref rtmp_err_t code.
    \remarks    See \ref rpc_releasestream for usage.
    \memberof   rtmp_client_t
*/
rtmp_err_t rtmp_client_releasestream(
    rtmp_client_t client,
    const char * path,
    rtmp_stream_amf_proc proc,
    void *userdata
);

/*! \brief      Issues a \ref rpc_fcpublish remote procedure call.
    \param      client      The RTMP client to issue the call with.
    \param      path        The \ref rtmp_playpath "playpath" which should be published to.
    \param      proc        A \ref rtmp_stream_amf_proc which will be called upon the receipt of a response to this command.
    \param      userdata    An optional pointer to pass in with the \a proc callback.
    \return     Returns a \ref rtmp_err_t code.
    \remarks    See \ref rpc_fcpublish for usage.
    \memberof   rtmp_client_t
*/
rtmp_err_t rtmp_client_fcpublish(
    rtmp_client_t client,
    const char * path,
    rtmp_stream_amf_proc proc,
    void *userdata
);

/*! \brief      Issues a \ref rpc_createstream remote procedure call.
    \param      client      The RTMP client to issue the call with.
    \param      proc        A \ref rtmp_stream_amf_proc which will be called upon the receipt of a response to this command.
    \param      userdata    An optional pointer to pass in with the \a proc callback.
    \return     Returns a \ref rtmp_err_t code.
    \remarks    See \ref rpc_createstream for usage.
    \memberof   rtmp_client_t
*/
rtmp_err_t rtmp_client_createstream(
    rtmp_client_t client,
    rtmp_stream_amf_proc proc,
    void *userdata
);

/*! \brief      Issues a \ref rpc_deletestream remote procedure call.
    \param      client      The RTMP client to issue the call with.
    \param      stream_id   The message stream to delete.
    \param      proc        A \ref rtmp_stream_amf_proc which will be called upon the receipt of a response to this command.
    \param      userdata    An optional pointer to pass in with the \a proc callback.
    \return     Returns a \ref rtmp_err_t code.
    \remarks    See \ref rpc_deletestream for usage.
    \memberof   rtmp_client_t
*/
rtmp_err_t rtmp_client_deletestream(
    rtmp_client_t client,
    size_t stream_id,
    rtmp_stream_amf_proc proc,
    void *userdata
);

/*! \brief      Issues a \ref rpc_publish remote procedure call.
    \param      client      The RTMP client to issue the call with.
    \param      streamid    The message stream to publish on.
    \param      path        The \ref rtmp_playpath "playpath" which should be published to.
    \param      type        The type of stream this will be.
    \param      proc        A \ref rtmp_stream_amf_proc which will be called upon the receipt of a response to this command.
    \param      userdata    An optional pointer to pass in with the \a proc callback.
    \return     Returns a \ref rtmp_err_t code.
    \remarks    See \ref rpc_publish for usage.
    \memberof   rtmp_client_t
*/
rtmp_err_t rtmp_client_publish(
    rtmp_client_t client,
    size_t streamid,
    const char * playpath,
    const char * type,
    rtmp_stream_amf_proc proc,
    void *userdata
);

/*! \brief      Issues a \ref rpc_unpublish remote procedure call.
    \param      client      The RTMP client to issue the call with.
    \param      streamid    The message stream to stop publishing on.
    \param      path        The \ref rtmp_playpath "playpath" which should be unpublished.
    \param      proc        A \ref rtmp_stream_amf_proc which will be called upon the receipt of a response to this command.
    \param      userdata    An optional pointer to pass in with the \a proc callback.
    \return     Returns a \ref rtmp_err_t code.
    \remarks    See \ref rpc_unpublish for usage.
    \memberof   rtmp_client_t
*/
rtmp_err_t rtmp_client_unpublish(
    rtmp_client_t client,
    size_t streamid,
    const char * playpath,
    rtmp_stream_amf_proc proc,
    void *userdata
);

/*! \brief      Issues a \ref rpc_setdataframe remote procedure call.
    \param      client          The RTMP client to issue the call with.
    \param      streamid        The message stream to send this metadata on.
    \param      frame_name      The name to give the data frame. Should be `onMetaData` for FLV.
    \param      duration        The length of the video.
    \param      size            The size of the video file.
    \param      width           The width of the video frame.
    \param      height          The height of the video frame.
    \param      vid_codecid     The codec used for the video.
    \param      vid_data_rate   The bitrate in kbps of the video.
    \param      framerate       The video framerate.
    \param      aud_codecid     The codec used for the audio.
    \param      aud_data_rate   The bitrate in kbps of the audio.
    \param      aud_sample_rate The sample rate of the audio.
    \param      aud_sample_size The sample size of each audio sample.
    \param      aud_channels    The number of audio channels.
    \param      encoder         The name of the encoder used to provide the stream.
    \return     Returns a \ref rtmp_err_t code.
    \remarks    See \ref rpc_setdataframe for usage.
    \memberof   rtmp_client_t
    \sa         rtmp_client_setdataframe_amf
*/
rtmp_err_t rtmp_client_setdataframe(
    rtmp_client_t client,
    size_t streamid,
    const char * frame_name,
    double duration,
    double size,
    uint32_t width,
    uint32_t height,
    const char * vid_codecid,
    double vid_data_rate,
    double framerate,
    const char * aud_codecid,
    double aud_data_rate,
    double aud_sample_rate,
    double aud_sample_size,
    uint32_t aud_channels,
    const char * encoder
);

/*! \brief      Issues a \ref rpc_setdataframe remote procedure call using a premade AMF object.
    \param      client      The RTMP client to issue the call with.
    \param      streamid    The message stream to send this metadata on.
    \param      object      A premade AMF object to send for this message.
    \return     Returns a \ref rtmp_err_t code.
    \remarks    See \ref rpc_setdataframe for usage.
    \remarks    This is a simplified version of \ref rtmp_client_setdataframe intended to simplify the process of proxying RTMP connections.
    \memberof   rtmp_client_t
    \sa         rtmp_client_setdataframe
*/
rtmp_err_t rtmp_client_setdataframe_amf(
    rtmp_client_t client,
    size_t streamid,
    amf_t object
);


rtmp_err_t rtmp_client_play( rtmp_client_t client,
    const char *stream_name,
    double start DEFAULT(-1),
    double duration DEFAULT(-1),
    bool reset DEFAULT(false) );

rtmp_err_t rtmp_client_play2( rtmp_client_t client,
    const char * old_stream_name,
    const char * new_stream_name,
    double start_time,
    double offset,
    double len,
    const char *restrict transition );

rtmp_err_t rtmp_client_recv_audio( rtmp_client_t client,
    bool want_audio);

rtmp_err_t rtmp_client_recv_video( rtmp_client_t client,
    bool want_video);

rtmp_err_t rtmp_client_seek( rtmp_client_t client,
    rtmp_time_t seek_ms);

rtmp_err_t rtmp_client_pause( rtmp_client_t client,
    bool paused,
    rtmp_time_t pause_ms);

#ifdef __cplusplus
}
#endif


#endif
