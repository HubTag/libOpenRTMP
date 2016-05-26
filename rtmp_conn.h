#include "data_stream.h"

#define RTMP_STREAM_CACHE_MAX 10

typedef rtmp_stream_cache{
    unsigned int timestamp;
    size_t message_length;
    byte message_type;
    unsigned int message_stream_id;
} rtmp_stream_cache_t;

typedef rtmp_conn {
    ors_data_t inflow, outflow;
    rtmp_stream_cache_t stream_cache[RTMP_STREAM_CACHE_MAX];
    void *nonce_a, *nonce_b;
    int (*callback)(ors_data_t contents, unsigned int chunk_stream_id, unsigned int message_stream_id, unsigned int timestamp, byte message_type );

    unsigned int chunk_size;

} *rtmp_conn_t;

