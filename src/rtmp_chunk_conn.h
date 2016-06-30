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


rtmp_chunk_conn_t rtmp_chunk_conn_create( byte client, ors_data_t inflow, ors_data_t outflow, rtmp_chunk_proc callback );
void rtmp_chunk_conn_close( rtmp_chunk_conn_t conn );

int rtmp_chunk_conn_set_chunk_size( rtmp_chunk_conn_t conn, unsigned int size );
int rtmp_chunk_conn_abort( rtmp_chunk_conn_t conn, unsigned int chunk_stream );
int rtmp_chunk_conn_acknowledge( rtmp_chunk_conn_t conn );
int rtmp_chunk_conn_set_window_ack_size( rtmp_chunk_conn_t conn, unsigned int size );
int rtmp_chunk_conn_set_peer_bwidth( rtmp_chunk_conn_t conn, unsigned int size, rtmp_limit_type_t limit_type );
