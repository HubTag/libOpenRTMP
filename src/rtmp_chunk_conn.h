#include "data_stream.h"
#include "rtmp_chunk_flow.h"

#define RTMP_STREAM_CACHE_MAX 10

typedef int (*rtmp_chunk_proc)(ors_data_t contents, unsigned int chunk_stream_id, unsigned int message_stream_id, unsigned int timestamp, byte message_type );

typedef rtmp_chunk_conn {
    ors_data_t inflow, outflow;
    rtmp_chunk_stream_message_t stream_cache[RTMP_STREAM_CACHE_MAX];
    void *nonce_a, *nonce_b;
    rtmp_chunk_proc callback;

    unsigned int chunk_size;
    unsigned int recieved_size;
    unsigned int window_size;
    unsigned int peer_bandwidth_size;
    byte peer_bandwidth_type;
} *rtmp_chunk_conn_t;

typedef enum{
    RTMP_LIMIT_TYPE_HARD = 0,
    RTMP_LIMIT_TYPE_SOFT,
    RTMP_LIMIT_TYPE_DYNAMIC
} rtmp_limit_type_t;


typedef enum{
    //Chunk Control Messages        //Page 18, §5.4
    RTMP_MSG_SET_CHUNK_SIZE = 1,    //Page 19, §5.4.1
    RTMP_MSG_ABORT = 2,             //Page 19, §5.4.2
    RTMP_MSG_ACK = 3,               //Page 20, §5.4.3
    RTMP_MSG_WIN_ACK_SIZE = 3,      //Page 20, §5.4.4
    RTMP_MSG_SET_PEER_BWIDTH = 6,   //Page 21, §5.4.5

    //User Control Messages         //Page 23, §6.2
    RTMP_MSG_USER_CTL = 4,          //Page 23, §6.2

    //RTMP Command Messages         //Page 23, §7
    RTMP_MSG_AUDIO = 8,             //Page 26, §7.1.4
    RTMP_MSG_VIDEO = 9,             //Page 26, §7.1.5
    RTMP_MSG_AGGREGATE = 22,        //Page 26, §7.1.6
    RTMP_MSG_AMF3_DAT = 15,         //Page 24, §7.1.2
    RTMP_MSG_AMF3_SO = 16,          //Page 24, §7.1.3
    RTMP_MSG_AMF3_CMD = 17,         //Page 24, §7.1.1
    RTMP_MSG_AMF0_DAT = 18,         //Page 24, §7.1.2
    RTMP_MSG_AMF0_SO = 19,          //Page 24, §7.1.3
    RTMP_MSG_AMF0_CMD = 20          //Page 24, §7.1.1
} rtmp_message_type_t;

typedef enum{
    //Audio Codec Support Bitflags          Page 31
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

typedef enum{
    //Video Codec Support Bitflags          Page 32
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

typedef enum{
    //Video Function Bitflags               Page 32
    RTMP_SUPPORT_VID_CLIENT_SEEK = 0x0001
} rtmp_support_func_vid_t;

typedef enum{
    //Object Encoding Version               Page 33
    RTMP_AMF0 = 0,
    RTMP_AMF3 = 3
} rtmp_amf_version_t;


typedef enum{
    //Shared Object Message Event Types     Page 25
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

typedef enum{
    //User Control Message Event Types      Page 27
    RTMP_USR_EVT_STREAM_BEGIN = 0,
    RTMP_USR_EVT_STREAM_EOF,
    RTMP_USR_EVT_STREAM_DRY,
    RTMP_USR_EVT_SET_BUF_LEN,
    RTMP_USR_EVT_STREAM_IS_REC,
    RTMP_USR_EVT_PING_REQ,
    RTMP_USR_EVT_PING_RES
} rtmp_usr_evt_t;

//NetConnection Commands                    Page 29, §7.2.1
#define RTMP_CMD_CONNECT "connect"
#define RTMP_CMD_CALL "call"
#define RTMP_CMD_CLOSE "close"
#define RTMP_CMD_CREATESTREAM "createStream"

//NetStream Commands                        Page 37, §7.2.2
#define RTMP_CMD_PLAY "play"
#define RTMP_CMD_PLAY2 "play2"
#define RTMP_CMD_DELETESTREAM "deleteStream"
#define RTMP_CMD_CLOSESTREAM "closeStream"
#define RTMP_CMD_RECEIVEAUDIO "receiveAudio"
#define RTMP_CMD_RECEIVEVIDEO "receiveVideo"
#define RTMP_CMD_PUBLISH "publish"
#define RTMP_CMD_SEEK "seek"
#define RTMP_CMD_PAUSE "pause"

rtmp_chunk_conn_t rtmp_chunk_conn_create( byte client, ors_data_t inflow, ors_data_t outflow, rtmp_chunk_proc callback );
void rtmp_chunk_conn_close( rtmp_chunk_conn_t conn );

int rtmp_chunk_conn_set_chunk_size( rtmp_chunk_conn_t conn, unsigned int size );
int rtmp_chunk_conn_abort( rtmp_chunk_conn_t conn, unsigned int chunk_stream );
int rtmp_chunk_conn_acknowledge( rtmp_chunk_conn_t conn );
int rtmp_chunk_conn_set_window_ack_size( rtmp_chunk_conn_t conn, unsigned int size );
int rtmp_chunk_conn_set_peer_bwidth( rtmp_chunk_conn_t conn, unsigned int size, rtmp_limit_type_t limit_type );
