#include "data_stream.h"

int rtmp_emit_shake_0( ors_data_t output );
int rtmp_emit_shake_1( ors_data_t output, unsigned int timestamp, const byte* nonce, size_t length);
int rtmp_emit_shake_2( ors_data_t output, unsigned int timestamp1, unsigned int timestamp2, const byte* nonce, size_t length);

int rtmp_read_shake_0( ors_data_t input );
int rtmp_read_shake_1( ors_data_t input, unsigned int *timestamp, byte* nonce, size_t length);
int rtmp_read_shake_2( ors_data_t input, unsigned int *timestamp1, unsigned int *timestamp2, byte* data, size_t length);

int rtmp_emit_hdr_basic( ors_data_t output, byte format, size_t id );
int rtmp_read_hdr_basic( ors_data_t output, byte *format, size_t *id );

int rtmp_emit_hdr_chunk_0( ors_data_t output, unsigned int timestamp, size_t message_length, byte message_type_id, unsigned int message_stream_id );
int rtmp_emit_hdr_chunk_1( ors_data_t output, unsigned int time_delta, size_t message_length, byte message_type_id);
int rtmp_emit_hdr_chunk_2( ors_data_t output, unsigned int time_delta );
int rtmp_emit_hdr_chunk_3( ors_data_t output );

int rtmp_read_hdr_chunk_0( ors_data_t input, unsigned int *timestamp, size_t *message_length, byte *message_type_id, unsigned int *message_stream_id );
int rtmp_read_hdr_chunk_1( ors_data_t input, unsigned int *time_delta, size_t *message_length, byte *message_type_id );
int rtmp_read_hdr_chunk_2( ors_data_t input, unsigned int *time_delta );
int rtmp_read_hdr_chunk_3( ors_data_t input );

