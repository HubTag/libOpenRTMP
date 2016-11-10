/*
    rtmp_constants.h

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


    ----------------------------------------

    All references in this file are relevant to the official RTMP 1.0 spec.

    Download: http://wwwimages.adobe.com/content/dam/Adobe/en/devnet/rtmp/pdf/rtmp_specification_1.0.pdf
    Information Page: http://www.adobe.com/devnet/rtmp.html
    CRC32: 015fd1b5
    Date: June 29th, 2016
*/

#ifndef RTMP_H_CONSTANTS_H
#define RTMP_H_CONSTANTS_H

#ifdef __cplusplus
extern "C" {
#endif


#include <openrtmp/rtmp/rtmp_config.h>
#include <openrtmp/rtmp/rtmp_types.h>


/*! \addtogroup rtmp_ref RTMP
    @{ */
/*! \addtogroup rtmp_types Types and Constants
    @{ */

/*! @{ */
//Nonce size specified on page 8, §5.2.3
/// \brief      The size of handshake nonces.
/// \details    Defined in §5.2.3 on page 8 of the \ref rtmp_spec.
#define RTMP_NONCE_SIZE                     1528

/// \brief      The RTMP version.
/// \details    Defined in §5.2.2 on page 7 of the \ref rtmp_spec.
#define RTMP_VERSION                        3

/// \brief      The ID of the aggregate chunk cache (used internally)
#define RTMP_CACHE_AGGREGATE                0

/// \brief      The maximum size of a message header.
/// \details    Defined in §5.3.1 on page 12 of the \ref rtmp_spec.
#define RTMP_MESSAGE_HEADER_SIZE            (1 + 3 + 4 + 3)

/// \brief      When specifying a stream event, this will indicate that the callback should fire for any event of the given class.
#define RTMP_ANY                            0xFFFFFF

/// \brief      The indicated capabilities of the server. (I have no idea what this means; it was reverse engineered.)
#define RTMP_CAPABILITIES                   0x1F

/// \brief      The indicated mode of the server (I have no idea what this means; it was reverse engineered.)
#define RTMP_MODE                           1

/// \brief      A bind address indicating any network device.
#define RTMP_ADDR_ANY                       "0.0.0.0"

/// \brief      The default size of a chunk.
/// \details    Defined in §5.4.1 on page 19 of the \ref rtmp_spec.
#define RTMP_DEFAULT_CHUNK_SIZE             128

/// \brief      The message stream ID of the control stream.
/// \details    Defined in §5.4 on page 18 of the \ref rtmp_spec.
#define RTMP_CONTROL_MSG_STREAM 0

/// \brief      The chunk stream ID of the control stream.
/// \details    Defined in §5.4 on page 18 of the \ref rtmp_spec.
#define RTMP_CONTROL_CHUNK_STREAM 2

/*! @} */
//Minimum RTMP values.
#define RTMP_MIN_WINDOW_SIZE 1
#define RTMP_MIN_CHUNK_SIZE 1
#define RTMP_MIN_PEER_CHUNK_SIZE 1
#define RTMP_MIN_PEER_WINDOW_SIZE 1
#define RTMP_MIN_WINDOW_ACK_SIZE 1
#define RTMP_MIN_PEER_WINDOW_SIZE 1

/*! \brief      Contains names for events which are fired by libOpenRTMP in event callbacks.
    \sa         rtmp_get_event_name
*/
typedef enum {
    RTMP_EVENT_CONNECT_SUCCESS, //!< A connection was made and a valid handshake transpired. Communication may commence.
    RTMP_EVENT_CONNECT_FAIL,    //!< A connection was not able to be made, or the handshake failed.
    RTMP_EVENT_CLOSED,          //!< The connection was closed.
    RTMP_EVENT_INTERRUPTED,     //!< The connection was interrupted.
    RTMP_EVENT_FAILED,          //!< The RTMP stream failed in some unexpected way.
    RTMP_EVENT_FILLED,          //!< The output buffer was filled with at least one byte.
    RTMP_EVENT_EMPTIED,         //!< The input buffer had at least one byte removed.
    RTMP_EVENT_REFRESH,         //!< Called at regular intervals.
} rtmp_event_t;


typedef enum {
    RTMP_IO_IN = 1,
    RTMP_IO_OUT = 2,
    RTMP_IO_ERR = 4
} rtmp_io_t;

/*! \brief      Contains names for status codes which may be returned by callbacks to control the flow of an RTMP stream.
*/
typedef enum {
    RTMP_CB_CONTINUE = 0,   //!< The callback completed successfully. Continue processing the stream.
    RTMP_CB_ERROR,          //!< The callback suffered an error. Abort processing this message. This message is not removed from the input buffer.
    RTMP_CB_DEFER_PAUSE,    //!< The callback wishes to abort processing this message, and pause the processing on this stream until further notice. This message is not removed from the input buffer.
    RTMP_CB_ABORT           //!< Abort the stream entirely.
} rtmp_cb_status_t;

/*! \brief      A list of flags used to store the status of a chunk connection.
    \remarks    \parblock
                These flags are used internally by both the client and server.

                When used by the client, the client flags indicate that the specified client messages were sent,
                and the server messages indicate that the specified server messages were received.

                When used by the server, the client flags indicate that the specified client messages were received,
                and the server messages indicate that the specified server messages were sent.

                For more information about the RTMP handshake, see §5.2 on page 7 of the \ref rtmp_spec.
                \endparblock
*/
typedef enum {
    RTMP_STATUS_UNINIT          =  0x0000,  //!< The connection hasn't been initialized at all.
    RTMP_STATUS_SHAKING_C0      =  0x0001,  //!< Indicates that the C0 handshake message has been transmitted from the client to the server.
    RTMP_STATUS_SHAKING_C1      =  0x0002,  //!< Indicates that the C1 handshake message has been transmitted from the client to the server.
    RTMP_STATUS_SHAKING_C2      =  0x0004,  //!< Indicates that the C2 handshake message has been transmitted from the client to the server.
    RTMP_STATUS_SHAKING_S0      =  0x0008,  //!< Indicates that the S0 handshake message has been transmitted from the server to the client.
    RTMP_STATUS_SHAKING_S1      =  0x0010,  //!< Indicates that the S1 handshake message has been transmitted from the server to the client.
    RTMP_STATUS_SHAKING_S2      =  0x0020,  //!< Indicates that the S2 handshake message has been transmitted from the server to the client.
    RTMP_STATUS_SHAKING_DONE    =  0x003F,  //!< A mask of all the shaking flags to indicate that the handshake has been completed.
    RTMP_STATUS_IS_CLIENT       =  0x0040   //!< A flag indicating that this is a client connection.
} rtmp_chunk_conn_status_t;

//Set Peer Bandwidth Limit Type         //Page 21, §5.4.5
/*! \brief      Provides names to indicate how the strategy a peer should use to change their window size after a \ref RTMP_MSG_SET_PEER_BWIDTH call.
    \remarks    These values are defined in §5.4.5 on page 21 of the \ref rtmp_spec.
    \sa         rtmp_get_limit_type_name
*/
typedef enum {
    RTMP_LIMIT_HARD = 0,                //!< The peer should limit output bandwidth to the accompanying value.
    RTMP_LIMIT_SOFT,                    //!< The peer should limit output bandwidth to the accompanying value, unless the existing limit is smaller.
    RTMP_LIMIT_DYNAMIC                  //!< If the previous limit was hard, treat this message as also being hard. Otherwise, ignore this message.
} rtmp_limit_t;

//RTMP Chunk Stream Message Types       //Page 18
/*! \brief      Contains names for message classes which are sent over an RTMP chunk stream.
    \remarks    \parblock
                The message types are all transmitted through a single field of the RTMP chunk header. However, there are three distinct types of message:
                * **chunk control messages**, **user control messages**, and **RTMP command messages**. There is an unfortunate bit over overlap between two
                arguably independent protocols when talking about RTMP. There is RTMP itself, and there is the RTMP chunk stream protocol, which is
                designed to basically be a part of RTMP. However, the one message field is used to send messages for both RTMP and the underlying RTMP
                chunk stream protocol.

                * **Chunk control messages** are used to control aspects of the RTMP chunk stream underlying an active RTMP stream.

                * **User control messages** are not really currently implemented.

                * **RTMP command messages** are used to send actual RTMP data over the RTMP chunk stream, such as audio/video content,
                remote procedure calls, AMF data, and so on.
                \endparblock
    \sa         rtmp_get_message_type_name
*/
typedef enum {
    //Chunk Control Messages
    RTMP_MSG_SET_CHUNK_SIZE = 1,        //!< A chunk control message indicating a new chunk size which the sender will use from now on. \n Defined in §5.4.1 on page 19 of the \ref rtmp_spec.
    RTMP_MSG_ABORT = 2,                 //!< A chunk control message that requests the peer abort processing of a message prematurely. \n Defined in §5.4.2 on page 19 of the \ref rtmp_spec.
    RTMP_MSG_ACK = 3,                   //!< A chunk control message which acknowledges the number of bytes received so far. \n Defined in §5.4.3 on page 20 of the \ref rtmp_spec.
    RTMP_MSG_WIN_ACK_SIZE = 5,          //!< A chunk control message indicating the window size of the sender. \n Defined in §5.4.4 on page 20 of the \ref rtmp_spec.
    RTMP_MSG_SET_PEER_BWIDTH = 6,       //!< A chunk control message indicating to the peer that the sender wishes the peer to change their window size. \n Defined in §5.4.5 on page 21 of the \ref rtmp_spec.
    //User Control Messages
    RTMP_MSG_USER_CTL = 4,              //!< Indicates that this is a user control message. \n Defined in §6.2 on page 23 of the \ref rtmp_spec.

    //RTMP Command Messages
    RTMP_MSG_AUDIO = 8,                 //!< An RTMP command message which indicates that the payload contains audio. \n Defined in §7.1.4 on page 26 of the \ref rtmp_spec.
    RTMP_MSG_VIDEO = 9,                 //!< An RTMP command message which indicates that the payload contains video. \n Defined in §7.1.5 on page 26 of the \ref rtmp_spec.
    RTMP_MSG_AGGREGATE = 22,            //!< An RTMP command message which indicates that this is an aggregate RTMP message. Aggregate RTMP messages are currently not supported. \n Defined in §7.1.6 on page 26 of the \ref rtmp_spec.
    RTMP_MSG_AMF3_DAT = 15,             //!< Indicates that the payload of this message is AMF3 information. \n Defined in §7.1.2 on page 24 of the \ref rtmp_spec.
    RTMP_MSG_AMF3_SO = 16,              //!< An RTMP command message which indicates that the payload is a serialized shared object in AMF3 format. \n Defined in §7.1.3 on page 24 of the \ref rtmp_spec.
    RTMP_MSG_AMF3_CMD = 17,             //!< An RTMP command message which indicates that the payload is a RPC call in AMF3 format. \n Defined in §7.1.1 on page 24 of the \ref rtmp_spec.
    RTMP_MSG_AMF0_DAT = 18,             //!< An RTMP command message which indicates that the payload is AMF0 information. \n Defined in §7.1.2 on page 24 of the \ref rtmp_spec.
    RTMP_MSG_AMF0_SO = 19,              //!< An RTMP command message which indicates that the payload is a serialized shared object in AMF0 format. \n Defined in §7.1.3 on page 24 of the \ref rtmp_spec.
    RTMP_MSG_AMF0_CMD = 20              //!< An RTMP command message which indicates that the payload is a RPC call in AMF0 format. \n Defined in §7.1.1 on page 24 of the \ref rtmp_spec.
} rtmp_message_type_t;

//Audio Codec Support Bitflags          //Page 31
/*! \brief      A set of bitflags which are used to indicate to the peer what sort of audio codecs are supported.
    \remarks    These values are defined in §7.2.1.1 on page 31 of the \ref rtmp_spec.
    \sa         rtmp_get_snd_codec_flag_name
    \sa         rtmp_get_snd_codec_flag_list
*/
typedef enum {
    RTMP_SUPPORT_SND_NONE       = 0x0001,       //!< Indicates support for Raw sound
    RTMP_SUPPORT_SND_ADPCM      = 0x0002,       //!< Indicates support for ADPCM audio compression
    RTMP_SUPPORT_SND_MP3        = 0x0004,       //!< Indicates support for MP3 audio compression
    RTMP_SUPPORT_SND_INTEL      = 0x0008,       //!< Not used
    RTMP_SUPPORT_SND_UNUSED     = 0x0010,       //!< Not used
    RTMP_SUPPORT_SND_NELLY8     = 0x0020,       //!< Indicates support for NellyMoser audio compression at 8kHz
    RTMP_SUPPORT_SND_NELLY      = 0x0040,       //!< Indicates support for NellyMoser audio compression at 5, 11, 22, and 44kHz
    RTMP_SUPPORT_SND_G711A      = 0x0080,       //!< Indicates support for G711A "a-law" audio compression
    RTMP_SUPPORT_SND_G711U      = 0x0100,       //!< Indicates support for G711U "u-law" audio compression
    RTMP_SUPPORT_SND_NELLY16    = 0x0200,       //!< Indicates support for NellyMoser audio compression at 16kHz
    RTMP_SUPPORT_SND_AAC        = 0x0400,       //!< Indicates support for Advanced Audio Coding
    RTMP_SUPPORT_SND_SPEEX      = 0x0800,       //!< Indicates support for Speex audio compression
    RTMP_SUPPORT_SND_ALL        = 0x0FFF        //!< Indicates support for all compatible audio compression codecs
} rtmp_support_codec_snd_t;

//Video Codec Support Bitflags          //Page 32
/*! \brief      A set of bitflags which are used to indicate to the peer what sort of video codecs are supported.
    \remarks    These values are defined in §7.2.1.1 on page 32 of the \ref rtmp_spec.
    \sa         rtmp_get_vid_codec_flag_name
    \sa         rtmp_get_vid_codec_flag_list
*/
typedef enum {
    RTMP_SUPPORT_VID_UNUSED     = 0x0001,       //!< Not used
    RTMP_SUPPORT_VID_JPEG       = 0x0002,       //!< Not used
    RTMP_SUPPORT_VID_SORENSON   = 0x0004,       //!< Indicates support for Sorenson flash video compression
    RTMP_SUPPORT_VID_HOMEBREW   = 0x0008,       //!< Indicates support for V1 screen sharing
    RTMP_SUPPORT_VID_VP6        = 0x0010,       //!< Indicates support for On2 video compression
    RTMP_SUPPORT_VID_VP6ALPHA   = 0x0020,       //!< Indicates support for On2 video with alpha compression
    RTMP_SUPPORT_VID_HOMEBREWV  = 0x0040,       //!< Indicates support for V2 screen sharing
    RTMP_SUPPORT_VID_H264       = 0x0080,       //!< Indicates support for Advanced Video Coding
    RTMP_SUPPORT_VID_ALL        = 0x00FF        //!< Indicates support for all compatible codecs
} rtmp_support_codec_vid_t;

//Video Function Bitflags               //Page 32
/*! \brief      A bitflag indicating whether a client supports various video functions.
    \remarks    This value is defined in §7.2.1.1 on page 32 of the \ref rtmp_spec.
    \sa         rtmp_get_vid_func_flag_name
    \sa         rtmp_get_vid_func_flag_list
*/
typedef enum {
    RTMP_SUPPORT_VID_CLIENT_SEEK = 0x0001       //!< Indicates that the player can perform frame-accurate seeks.
} rtmp_support_func_vid_t;

//Object Encoding Version               //Page 33
/*! \brief      Provides some symbols to specify AMF version.
*/
typedef enum {
    RTMP_AMF0 = 0,                              //!< Indicates AMF version 0
    RTMP_AMF3 = 3                               //!< Indicates AMF version 3
} rtmp_amf_version_t;


//Shared Object Message Event Types     //Page 25
/*! \brief      Specifies the various shared object events which may occur.
    \remarks    \parblock
                These definitions can be found on page 25 of the RTMP specification.

                These are not used currently.
                \endparblock
    \sa         rtmp_get_so_evt_type_name
*/
typedef enum {
    RTMP_SO_EVT_USE = 1,                        //!<
    RTMP_SO_EVT_RELEASE,                        //!<
    RTMP_SO_EVT_REQ_CHANGE,                     //!<
    RTMP_SO_EVT_CHANGE,                         //!<
    RTMP_SO_EVT_SUCCESS,                        //!<
    RTMP_SO_EVT_SENDMESSAGE,                    //!<
    RTMP_SO_EVT_STATUS,                         //!<
    RTMP_SO_EVT_CLEAR,                          //!<
    RTMP_SO_EVT_REMOVE,                         //!<
    RTMP_SO_EVT_REQ_REMOVE,                     //!<
    RTMP_SO_EVT_USE_SUCCESS                     //!<
} rtmp_so_evt_t;

//User Control Message Event Types      //Page 27
/*! \brief      Specifies the various user events that may occur.
    \remarks    \parblock
                These definitions can be found on page 27 of the RTMP specification.

                These are not used currently.
                \endparblock
    \sa         rtmp_get_usr_evt_type_name
*/
typedef enum {
    RTMP_USR_EVT_STREAM_BEGIN = 0,              //!<
    RTMP_USR_EVT_STREAM_EOF,                    //!<
    RTMP_USR_EVT_STREAM_DRY,                    //!<
    RTMP_USR_EVT_SET_BUF_LEN,                   //!<
    RTMP_USR_EVT_STREAM_IS_REC,                 //!<
    RTMP_USR_EVT_PING_REQ,                      //!<
    RTMP_USR_EVT_PING_RES                       //!<
} rtmp_usr_evt_t;

/*! \brief      Contains names for error codes which are returned by more libOpenRTMP calls.
    \remarks    Most of the error codes are ignorable, however fatal errors will always result in the current stream being aborted.
    \sa         rtmp_get_err_name
*/
typedef enum {
    RTMP_ERR_NONE = 0,                          //!< No error occured.
    RTMP_ERR_CONNECTION_CLOSED,                 //!< The connection was closed gracefully.
    RTMP_ERR_DIVERGENCE_METER_ERROR,            //!< The value of a window acknowledgement indicates that the recipient has more data than you sent. Are they in the future?
    RTMP_ERR_ERROR,                             //!< A generic error. Usually returned because of callbacks which return \ref RTMP_CB_ERROR.
    RTMP_ERR_NOT_READY,                         //!< The stream has not completed the handshake process yet.
    RTMP_ERR_INVALID,                           //!< Invalid data found in the stream. This is often fatal in the sense that either the client or server's binary stream is corrupted, though sometimes it's not.
    RTMP_ERR_BAD_WRITE,                         //!< An attempt to write information (usually AMF data) has failed for some reason.
    RTMP_ERR_BAD_READ,                          //!< An attempt to read information (usually AMF data) has failed for some reason.
    RTMP_ERR_PAUSE,                             //!< The stream is paused.
    RTMP_ERR_AGAIN,                             //!< The data could not be completely written into the output buffer.
    RTMP_ERR_DNS_FAIL,                          //!< A domain name failed to resolve.
    RTMP_ERR_POLL_FAIL,                         //!< The underlying network polling mechanism aborted unexpectedly.
    RTMP_ERR_FATAL,                             //!< A generic fatal error. Fatal errors indicate some unreconcileable problem with local or global state.
    RTMP_ERR_CONNECTION_FAIL,                   //!< A fatal error indicating that the underlying transport layer has failed in some fashion.
    RTMP_ERR_OOM,                               //!< A fatal error indicating that there is insufficient memory to perform the request.
    RTMP_ERR_INADEQUATE_CHUNK,                  //!< A fatal error indicating that the chunk cache is full. This means that more than \ref RTMP_STREAM_CACHE_MAX chunk IDs are in use.
    RTMP_ERR_ABORT                              //!< A fatal error indicating that the stream is aborting for some reason, usually as a result of a callback.
} rtmp_err_t;


/*! @} */
/*! @} */

//NetConnection Commands                //Page 29, §7.2.1
#define RTMP_CMD_CONNECT "connect"
#define RTMP_CMD_CALL "call"
#define RTMP_CMD_CLOSE "close"
#define RTMP_CMD_CREATESTREAM "createStream"

//NetStream Commands                    //Page 37, §7.2.2
#define RTMP_CMD_PLAY "play"
#define RTMP_CMD_PLAY2 "play2"
#define RTMP_CMD_DELETESTREAM "deleteStream"
#define RTMP_CMD_CLOSESTREAM "closeStream"
#define RTMP_CMD_RECEIVEAUDIO "receiveAudio"
#define RTMP_CMD_RECEIVEVIDEO "receiveVideo"
#define RTMP_CMD_PUBLISH "publish"
#define RTMP_CMD_SEEK "seek"
#define RTMP_CMD_PAUSE "pause"

#define RTMP_NETCON_ACCEPT "NetConnection.Connect.Success"
#define RTMP_NETCON_REJECT "NetConnection.Connect.Rejected"
#define RTMP_NETSTREAM_START "NetStream.Publish.Start"
#define RTMP_TEMP_BUFF_SIZE 600

#ifdef __cplusplus
}
#endif

#endif
