title: Config (rtmp/rtmp_config.h)
--------------------------------------

The RTMP config header contains many definitions that drive a few factors which pertain mostly to memory consumption vs performance. Some aren't really meant to be modified, but may be changed to affect various aspects of the library.

|Name|Sane Default|Description|
|:-:|:-:|:--|
|`RTMP_FMSVER`|`0,0,0,0`|A string indicating the version of "Flash Media Server," or in this case, libOpenRTMP|
|`RTMP_FMSVER_STR`|`LibOpenRTMP/0,0,0,0`|A string indicating the name and version of "Flash Media Server," or in this case, libOpenRTMP|
|`RTMP_FLASHVER_VERSION`|`0,0`|A string indicating the version of "Flash," or in this case, libOpenRTMP|
|`RTMP_FLASHVER_STR`|`LibOpenRTMP/0,0`|A string indicating the name and version of "Flash," or in this case, libOpenRTMP|
|`RTMP_DEFAULT_PORT`|`1935`|The default port for RTMP. This should probably not be changed unless you're planning on breaking RTMP compatibility.|
|`RTMP_DEFAULT_WINDOW_SIZE`|`0x00FFFFFF`|The default requested window size.|
|`RTMP_DEFAULT_PEER_WINDOW_SIZE`|`0`|Doesn't really mean anything|
|`RTMP_DEFAULT_BANDWIDTH_TYPE`|`RTMP_LIMIT_HARD`|The default setting for changing window size.|
|`RTMP_MAX_CHUNK_SIZE`|`0x4FFFF`|The maximum tolerable chunk size.|
|`RTMP_DESIRED_CHUNK_SIZE`|`RTMP_MAX_CHUNK_SIZE`|The desired chunk size.|
|`RTMP_DEFAULT_IO_BUFFER_SIZE`|`0x7FFFF`|The default number of bytes allocated for input/output buffers. It may be desireable to be able to store a few seconds worth of data in these buffers.|
|`RTMP_STREAM_STATIC_CACHE_SIZE`|`10`|The size of the static stream cache allocation. If a chunk stream ID exceeds this number, dense dynamic allocation will be used.|
|`RTMP_STREAM_CACHE_MAX`|`100`|The max number of stream cache entries.|
|`RTMP_CONTROL_BUFFER_SIZE`|`12`|The size of the control message buffer. If a control message can't fit in this number of bytes, the message is ignored. As of writing, the biggest documented control message is 12 bytes.|
|`RTMP_SPEC_ENFORCE_HANDSHAKE_TIMES`| |If defined, connections with handshakes whose timestamps aren't correct are dropped.|
|`RTMP_SPEC_ENFORCE_HANDSHAKE_NONCES`| |If defined, connections with handshakes whose nonces are incorrect are dropped.|
|`RTMP_LOG_LEVEL`|`2`|The logging level. The lower the number, the less info is logged. More information is available in the header.|
|`RTMP_MAX_ALLOC`|`10000`|The maximum amount of memory allowed to be allocated by a stream. Currently unimplemented.|
|`RTMP_MAX_CHUNK_CACHE`|`0xFFFF`|The chunk assembler will allocate this much room for assembly. Chunks larger than this will still be fragmented. Usually, chunks larger than a few hundred bytes are media frames, which are much easier to deal with as fragments.|
|`RTMP_MAX_STREAMS`|`10`|The maximum number of message streams on one connection.|
|`RTMP_MAX_ASM_HARD_BUFFER`|`20`|The maximum number of assembler buffers. The stream will be aborted if there are more streams than this.|
|`RTMP_MAX_ASM_SOFT_BUFFER`|`5`|The maximum number of pre-allocated assembler buffers. If more buffers are needed than this, they will be allocated as needed, and destroyed once the relevant message has been assembled.|
|`RTMP_POLLTECH_EPOLL`| |If defined, the networking implementation used will be epoll based.|
|`RTMP_EPOLL_MAX`|`100`|The maximum number of file descriptors returned by epoll.|
|`RTMP_LISTEN_SIZE`|`100`|The maximum number of connections waiting to be accepted.|
|`RTMP_REFRESH_TIME`|`1000`|The interval, in milliseconds, at which the refresh event is fired.|
|`RTMP_CALL_TIMEOUT`|`10000`|How long, in milliseconds, the library will wait for a pending RPC to finish.|
