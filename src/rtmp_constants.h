/*
    rtmp_constants.h

    Copyright (C) 2016 Hubtag LLC.

    ----------------------------------------

    This file is part of libOpenRTMP.

    libOpenRTMP is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    libOpenRTMP is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with libOpenRTMP.  If not, see <http://www.gnu.org/licenses/>.


    ----------------------------------------

    All references in this file are relevant to the official RTMP 1.0 spec.

    Download: http://wwwimages.adobe.com/content/dam/Adobe/en/devnet/rtmp/pdf/rtmp_specification_1.0.pdf
    Information Page: http://www.adobe.com/devnet/rtmp.html
    CRC32: 015fd1b5
    Date: June 29th, 2016
*/

#pragma once

#include "rtmp_types.h"

//Defaults

#define RTMP_DEFAULT_CHUNK_SIZE             0x00000FFF
#define RTMP_DEFAULT_WINDOW_SIZE            0x000FFFFF
#define RTMP_DEFAULT_BANDWIDTH_TYPE         RTMP_LIMIT_HARD

//Nonce size specified on page 8, §5.2.3
#define RTMP_NONCE_SIZE                     1528

#define RTMP_SPEC_ENFORCE_HANDSHAKE_TIMES
#define RTMP_SPEC_ENFORCE_HANDSHAKE_NONCES

typedef enum {
    RTMP_IO_IN = 1,
    RTMP_IO_OUT = 2,
    RTMP_IO_ERR = 4
} rtmp_io_t;

typedef enum {
    RTMP_STATUS_UNINIT          =  0x0000,
    RTMP_STATUS_SHAKING_C0      =  0x0001,
    RTMP_STATUS_SHAKING_C1      =  0x0002,
    RTMP_STATUS_SHAKING_C2      =  0x0004,
    RTMP_STATUS_SHAKING_S0      =  0x0008,
    RTMP_STATUS_SHAKING_S1      =  0x0010,
    RTMP_STATUS_SHAKING_S2      =  0x0020,
    RTMP_STATUS_SHAKING_DONE    =  0x003F,
    RTMP_STATUS_IS_CLIENT       =  0x0040
} rtmp_chunk_conn_status_t;

//Set Peer Bandwidth Limit Type         //Page 21, §5.4.5
typedef enum{
    RTMP_LIMIT_HARD = 0,
    RTMP_LIMIT_SOFT,
    RTMP_LIMIT_DYNAMIC
} rtmp_limit_t;

//RTMP Chunk Stream Message Types       //Page 18
typedef enum{
    //Chunk Control Messages            //Page 18, §5.4
    RTMP_MSG_SET_CHUNK_SIZE = 1,        //Page 19, §5.4.1
    RTMP_MSG_ABORT = 2,                 //Page 19, §5.4.2
    RTMP_MSG_ACK = 3,                   //Page 20, §5.4.3
    RTMP_MSG_WIN_ACK_SIZE = 5,          //Page 20, §5.4.4
    RTMP_MSG_SET_PEER_BWIDTH = 6,       //Page 21, §5.4.5

    //User Control Messages             //Page 23, §6.2
    RTMP_MSG_USER_CTL = 4,              //Page 23, §6.2

    //RTMP Command Messages             //Page 23, §7
    RTMP_MSG_AUDIO = 8,                 //Page 26, §7.1.4
    RTMP_MSG_VIDEO = 9,                 //Page 26, §7.1.5
    RTMP_MSG_AGGREGATE = 22,            //Page 26, §7.1.6
    RTMP_MSG_AMF3_DAT = 15,             //Page 24, §7.1.2
    RTMP_MSG_AMF3_SO = 16,              //Page 24, §7.1.3
    RTMP_MSG_AMF3_CMD = 17,             //Page 24, §7.1.1
    RTMP_MSG_AMF0_DAT = 18,             //Page 24, §7.1.2
    RTMP_MSG_AMF0_SO = 19,              //Page 24, §7.1.3
    RTMP_MSG_AMF0_CMD = 20              //Page 24, §7.1.1
} rtmp_message_type_t;

//Audio Codec Support Bitflags          //Page 31
typedef enum{
    RTMP_SUPPORT_SND_NONE       = 0x0001,
    RTMP_SUPPORT_SND_ADPCM      = 0x0002,
    RTMP_SUPPORT_SND_MP3        = 0x0004,
    RTMP_SUPPORT_SND_INTEL      = 0x0008,
    RTMP_SUPPORT_SND_UNUSED     = 0x0010,
    RTMP_SUPPORT_SND_NELLY8     = 0x0020,
    RTMP_SUPPORT_SND_NELLY      = 0x0040,
    RTMP_SUPPORT_SND_G711A      = 0x0080,
    RTMP_SUPPORT_SND_G711U      = 0x0100,
    RTMP_SUPPORT_SND_NELLY16    = 0x0200,
    RTMP_SUPPORT_SND_AAC        = 0x0400,
    RTMP_SUPPORT_SND_SPEEX      = 0x0800,
    RTMP_SUPPORT_SND_ALL        = 0x0FFF
} rtmp_support_codec_snd_t;

//Video Codec Support Bitflags          //Page 32
typedef enum{
    RTMP_SUPPORT_VID_UNUSED     = 0x0001,
    RTMP_SUPPORT_VID_JPEG       = 0x0002,
    RTMP_SUPPORT_VID_SORENSON   = 0x0004,
    RTMP_SUPPORT_VID_HOMEBREW   = 0x0008,
    RTMP_SUPPORT_VID_VP6        = 0x0010,
    RTMP_SUPPORT_VID_VP6ALPHA   = 0x0020,
    RTMP_SUPPORT_VID_HOMEBREWV  = 0x0040,
    RTMP_SUPPORT_VID_H264       = 0x0080,
    RTMP_SUPPORT_VID_ALL        = 0x00FF
} rtmp_support_codec_vid_t;

//Video Function Bitflags               //Page 32
typedef enum{
    RTMP_SUPPORT_VID_CLIENT_SEEK = 0x0001
} rtmp_support_func_vid_t;

//Object Encoding Version               //Page 33
typedef enum{
    RTMP_AMF0 = 0,
    RTMP_AMF3 = 3
} rtmp_amf_version_t;


//Shared Object Message Event Types     //Page 25
typedef enum{
    RTMP_SO_EVT_USE = 1,
    RTMP_SO_EVT_RELEASE,
    RTMP_SO_EVT_REQ_CHANGE,
    RTMP_SO_EVT_CHANGE,
    RTMP_SO_EVT_SUCCESS,
    RTMP_SO_EVT_SENDMESSAGE,
    RTMP_SO_EVT_STATUS,
    RTMP_SO_EVT_CLEAR,
    RTMP_SO_EVT_REMOVE,
    RTMP_SO_EVT_REQ_REMOVE,
    RTMP_SO_EVT_USE_SUCCESS
} rtmp_so_evt_t;

//User Control Message Event Types      //Page 27
typedef enum{
    RTMP_USR_EVT_STREAM_BEGIN = 0,
    RTMP_USR_EVT_STREAM_EOF,
    RTMP_USR_EVT_STREAM_DRY,
    RTMP_USR_EVT_SET_BUF_LEN,
    RTMP_USR_EVT_STREAM_IS_REC,
    RTMP_USR_EVT_PING_REQ,
    RTMP_USR_EVT_PING_RES
} rtmp_usr_evt_t;

typedef enum {
    RTMP_ERR_NONE = 0,
    RTMP_ERR_NOT_READY,
    RTMP_ERR_ERROR,
    RTMP_ERR_OOM,
    RTMP_ERR_INVALID,
    RTMP_ERR_BAD_WRITE,
    RTMP_ERR_BAD_READ,
    RTMP_ERR_INADEQUATE_CHUNK
} rtmp_err_t;

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
