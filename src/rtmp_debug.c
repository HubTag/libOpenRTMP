/*
    rtmp_debug.c

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

#include "rtmp_debug.h"
#include <stdio.h>
#include <string.h>
#include <limits.h>

static char scratch_buffer[512];

static void rtmp_print_s(const char *v){
    printf("%s", v);
}

static void rtmp_print_s2(const char *v, size_t len){
    int amt = (int) len;
    if( len > INT_MAX ){
        amt = INT_MAX;
    }
    printf("%.*s", amt, v);
}

static void rtmp_print_d(int v){
    printf("%d", v);
}

static void rtmp_print_f(double v){
    printf("%lf", v);
}

static void rtmp_print_u(unsigned int v){
    printf("%u", v);
}


static rtmp_printer def = {
    s : rtmp_print_s,
    s2 : rtmp_print_s2,
    d : rtmp_print_d,
    f : rtmp_print_f,
    u : rtmp_print_u
};

rtmp_printer_t rtmp_default_printer = &def;



const char* rtmp_get_message_type_name( rtmp_message_type_t mtype ){
    switch( mtype ){
        case RTMP_MSG_SET_CHUNK_SIZE: return "Set Chunk Size";
        case RTMP_MSG_ABORT: return "Abort Message";
        case RTMP_MSG_ACK: return "Acknowledgement";
        case RTMP_MSG_WIN_ACK_SIZE: return "Window Acknowledgement Size";
        case RTMP_MSG_SET_PEER_BWIDTH: return "Set Peer Bandwidth";
        case RTMP_MSG_USER_CTL: return "User Control Message";
        case RTMP_MSG_AUDIO: return "Audio Message";
        case RTMP_MSG_VIDEO: return "Video Message";
        case RTMP_MSG_AGGREGATE: return "Aggregate Message";
        case RTMP_MSG_AMF3_DAT: return "AMF3 Data Message";
        case RTMP_MSG_AMF3_SO: return "AMF3 Shared Object Message";
        case RTMP_MSG_AMF3_CMD: return "AMF3 Command Message";
        case RTMP_MSG_AMF0_DAT: return "AMF0 Data Message";
        case RTMP_MSG_AMF0_SO: return "AMF0 Shared Object Message";
        case RTMP_MSG_AMF0_CMD: return "AMF0 Command Message";
        default: return "Unknown Message Type";
    }
}

const char* rtmp_get_limit_type_name( rtmp_limit_t mtype ){
    switch( mtype ){
        case RTMP_LIMIT_HARD: return "Hard Limit";
        case RTMP_LIMIT_SOFT: return "Soft Limit";
        case RTMP_LIMIT_DYNAMIC: return "Dynamic Limit";
        default: return "Unknown Limit Type";
    }
}

const char* rtmp_get_snd_codec_flag_name( rtmp_support_codec_snd_t mtype ){
    switch( mtype ){
        case RTMP_SUPPORT_SND_NONE: return "Raw Sound";
        case RTMP_SUPPORT_SND_ADPCM: return "ADPCM";
        case RTMP_SUPPORT_SND_MP3: return "MP3";
        case RTMP_SUPPORT_SND_INTEL: return "Intel";
        case RTMP_SUPPORT_SND_UNUSED: return "Unused";
        case RTMP_SUPPORT_SND_NELLY8: return "NellyMoser@8kHz";
        case RTMP_SUPPORT_SND_NELLY: return "NellyMoser@5/11/22/44khz";
        case RTMP_SUPPORT_SND_G711A: return "G711A";
        case RTMP_SUPPORT_SND_G711U: return "G711U";
        case RTMP_SUPPORT_SND_NELLY16: return "NellyMoser@16kHz";
        case RTMP_SUPPORT_SND_AAC: return "AAC";
        case RTMP_SUPPORT_SND_SPEEX: return "Speex";
        case RTMP_SUPPORT_SND_ALL: return "All";
        default: return "Unknown Codec";
    }
}

const char* rtmp_get_snd_codec_flag_list( rtmp_support_codec_snd_t mtype ){
    if( (mtype & RTMP_SUPPORT_SND_ALL) == 0){
        return "No Codecs Specified";
    }
    if( (mtype & RTMP_SUPPORT_SND_ALL) == RTMP_SUPPORT_SND_ALL){
        return rtmp_get_snd_codec_flag_name( RTMP_SUPPORT_SND_ALL );
    }
    size_t offset = 0;
    size_t bit = 1;
    //Iterate through the bits and pass them into the name matcher
    while( bit < RTMP_SUPPORT_SND_ALL ){
        if( mtype & bit ){
            if( offset != 0 ){
                scratch_buffer[offset++] = ',';
            }
            //Append the flag name to the scratch buffer
            const char *flag = rtmp_get_snd_codec_flag_name( mtype & bit );
            size_t len = strlen( flag );
            strncpy(scratch_buffer + offset, flag, len + 1);
            offset += len;
        }
        bit <<= 1;
    }
    return scratch_buffer;
}

const char* rtmp_get_vid_codec_flag_name( rtmp_support_codec_vid_t mtype ){
    switch( mtype ){
        case RTMP_SUPPORT_VID_UNUSED: return "Raw Video";
        case RTMP_SUPPORT_VID_JPEG: return "Motion JPEG";
        case RTMP_SUPPORT_VID_SORENSON: return "Sorenson";
        case RTMP_SUPPORT_VID_HOMEBREW: return "Screen Sharing";
        case RTMP_SUPPORT_VID_VP6: return "VP6";
        case RTMP_SUPPORT_VID_VP6ALPHA: return "VP6 w/Alpha";
        case RTMP_SUPPORT_VID_HOMEBREWV: return "Screen Sharing v2";
        case RTMP_SUPPORT_VID_H264: return "h.264";
        case RTMP_SUPPORT_VID_ALL: return "All";
        default: return "Unknown Codec";
    }
}

const char* rtmp_get_vid_codec_flag_list( rtmp_support_codec_vid_t mtype ){
    if( (mtype & RTMP_SUPPORT_VID_ALL) == 0){
        return "No Codecs Specified";
    }
    if( (mtype & RTMP_SUPPORT_VID_ALL) == RTMP_SUPPORT_VID_ALL){
        return rtmp_get_vid_codec_flag_name( RTMP_SUPPORT_VID_ALL );
    }
    size_t offset = 0;
    size_t bit = 1;
    //Iterate through the bits and pass them into the name matcher
    while( bit < RTMP_SUPPORT_VID_ALL ){
        if( mtype & bit ){
            if( offset != 0 ){
                scratch_buffer[offset++] = ',';
            }
            //Append the flag name to the scratch buffer
            const char *flag = rtmp_get_vid_codec_flag_name( mtype & bit );
            size_t len = strlen( flag );
            strncpy(scratch_buffer + offset, flag, len + 1);
            offset += len;
        }
        bit <<= 1;
    }
    return scratch_buffer;
}

const char* rtmp_get_vid_func_flag_name( rtmp_support_func_vid_t mtype ){
    switch( mtype ){
        case RTMP_SUPPORT_VID_CLIENT_SEEK: return "Seeking";
        default: return "Unknown Function";
    }
}

const char* rtmp_get_vid_func_flag_list( rtmp_support_func_vid_t mtype ){
    if( mtype == RTMP_SUPPORT_VID_CLIENT_SEEK ){
        return rtmp_get_vid_func_flag_name(RTMP_SUPPORT_VID_CLIENT_SEEK);
    }
    return "No Functions Specified";
}

const char* rtmp_get_so_evt_type_name( rtmp_so_evt_t mtype ){
    switch( mtype ){
        case RTMP_SO_EVT_USE: return "Use";
        case RTMP_SO_EVT_RELEASE: return "Release";
        case RTMP_SO_EVT_REQ_CHANGE: return "Request Change";
        case RTMP_SO_EVT_CHANGE: return "Change";
        case RTMP_SO_EVT_SUCCESS: return "Success";
        case RTMP_SO_EVT_SENDMESSAGE: return "SendMessage";
        case RTMP_SO_EVT_STATUS: return "Status";
        case RTMP_SO_EVT_CLEAR: return "Clear";
        case RTMP_SO_EVT_REMOVE: return "Remove";
        case RTMP_SO_EVT_REQ_REMOVE: return "Request Remove";
        case RTMP_SO_EVT_USE_SUCCESS: return "Use Success";
        default: return "Unknown Shared Object Event Type";
    }
}

const char* rtmp_get_usr_evt_type_name( rtmp_usr_evt_t mtype ){
    switch( mtype ){
        case RTMP_USR_EVT_STREAM_BEGIN: return "Stream Begin";
        case RTMP_USR_EVT_STREAM_EOF: return "Stream EOF";
        case RTMP_USR_EVT_STREAM_DRY: return "StreamDry";
        case RTMP_USR_EVT_SET_BUF_LEN: return "SetBufferLength";
        case RTMP_USR_EVT_STREAM_IS_REC: return "StreamIsRecorded";
        case RTMP_USR_EVT_PING_REQ: return "PingRequest";
        case RTMP_USR_EVT_PING_RES: return "PingResponse";
        default: return "Unknown User Control Message Event Type";
    }
}

const char* rtmp_get_event_name( rtmp_event_t mtype ){
    switch( mtype ){
        case RTMP_EVENT_CONNECT_SUCCESS:    return "Connection Successful";
        case RTMP_EVENT_CONNECT_FAIL:       return "Connection Failed";
        case RTMP_EVENT_CLOSED:             return "Connection Closed";
        case RTMP_EVENT_INTERRUPTED:        return "Connection Interrupted";
        case RTMP_EVENT_FAILED:             return "Connection Failed";
        case RTMP_EVENT_FILLED:             return "Buffer Filled Event";
        case RTMP_EVENT_EMPTIED:            return "Buffer Emptied Event";
        default:                            return "Unknown Connection Event";
    }
}


const char* rtmp_get_err_name( rtmp_err_t err ){
    switch( err ){
        case RTMP_ERR_NONE: return "No error";
        case RTMP_ERR_NOT_READY: return "Stream not ready";
        case RTMP_ERR_ERROR: return "Generic error";
        case RTMP_ERR_OOM: return "Out of memory";
        case RTMP_ERR_INVALID: return "Invalid stream data";
        case RTMP_ERR_BAD_WRITE: return "Write fail to output";
        case RTMP_ERR_BAD_READ: return "Read fail from input";
        case RTMP_ERR_INADEQUATE_CHUNK: return "Inadequate chunk cache";
        case RTMP_ERR_AGAIN: return "Try again later";
        case RTMP_ERR_DIVERGENCE_METER_ERROR: return "Divergence meter in erroneous state (time travel is possible?)";
        case RTMP_ERR_CONNECTION_FAIL: return "Failed to connect or listen";
        case RTMP_ERR_DNS_FAIL: return "Failed to lookup host";
        case RTMP_ERR_POLL_FAIL: return "Polling system failed";
        case RTMP_ERR_ABORT: return "Connection aborted";
        case RTMP_ERR_FATAL: return "Fatal error";
        default:
            snprintf( scratch_buffer, 511, "Unknown error code %d", err );
            return scratch_buffer;
    }
}


