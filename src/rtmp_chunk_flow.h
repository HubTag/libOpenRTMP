#include "data_stream.h"
#define RTMP_STREAM_CACHE_MAX 10

typedef struct rtmp_chunk_stream_message{
    unsigned int chunk_stream_id;
    unsigned int timestamp;
    size_t message_length;
    byte message_type;
    unsigned int message_stream_id;
} rtmp_chunk_stream_message_t;

int rtmp_chunk_emit_shake_0( ors_data_t output );
int rtmp_chunk_emit_shake_1( ors_data_t output, unsigned int timestamp, const byte* nonce, size_t length);
int rtmp_chunk_emit_shake_2( ors_data_t output, unsigned int timestamp1, unsigned int timestamp2, const byte* nonce, size_t length);

int rtmp_chunk_read_shake_0( ors_data_t input );
int rtmp_chunk_read_shake_1( ors_data_t input, unsigned int *timestamp, byte* nonce, size_t length);
int rtmp_chunk_read_shake_2( ors_data_t input, unsigned int *timestamp1, unsigned int *timestamp2, byte* data, size_t length);

int rtmp_chunk_emit_hdr( ors_data_t output, rtmp_chunk_stream_message_t *message, rtmp_chunk_stream_message_t cache[RTMP_STREAM_CACHE_MAX] );
int rtmp_chunk_read_hdr( ors_data_t output, rtmp_chunk_stream_message_t **message, rtmp_chunk_stream_message_t cache[RTMP_STREAM_CACHE_MAX] );

int rtmp_chunk_emit_hdr_basic( ors_data_t output, byte format, size_t id );
int rtmp_chunk_read_hdr_basic( ors_data_t input, byte *format, size_t *id );

