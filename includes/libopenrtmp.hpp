#include <cstdint>

const unsigned RTMP_VERSION = 3;
const unsigned RTMP_DEFAULT_CHUNK_SIZE = 128;

/* RTMP MESSAGE TYPES */
enum class RTMP_MESSAGE_TYPE{
    CHUNK_SIZE      = 1,
    ABORT           = 2,
    ACK             = 3,
    USER            = 4,
    ACK_SIZE        = 5,
    BANDWIDTH       = 6,
    EDGE            = 7,
    AUDIO           = 8,
    VIDEO           = 9,
    AMF3_META       = 15,
    AMF3_SHARED     = 16,
    AMF3_CMD        = 17,
    AMF_META        = 18,
    AMF_SHARED      = 19,
    AMF_CMD         = 20,
    AGGREGATE       = 22,
    MAX             = 22
};

/* RTMP CONTROL MESSAGE TYPES */
enum class RTMP_CONTROL_MSG_TYPE{
    STREAM_BEGIN    = 0,
    STREAM_EOF      = 1,
    STREAM_DRY      = 2,
    SET_BUFLEN      = 3,
    RECORDED        = 4,
    PING_REQUEST    = 6,
    PING_RESPONSE   = 7,
    UNKOWN          = 8,
    BUFFER_END      = 31
};

struct RTMPChunk{
    uint32_t csID;
    uint32_t timeStamp;
    uint32_t msgLen;
    uint8_t  msgType;
    uint32_t msgID;
};

// server functions
void *rtmp_make_server();
void rtmp_reference_server(void* server);
void rtmp_destroy_server(void* server);

// client functions
void *rtmp_make_client();
void rtmp_reference_client(void* client);
void rtmp_destroy_client(void* client);
