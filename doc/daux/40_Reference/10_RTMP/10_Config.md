title: Config
--------------------------------------

The RTMP config header, found in `openrtmp/rtmp/rtmp_config.h`, contains many definitions that drive a few factors which pertain mostly to memory consumption vs performance. Some aren't really meant to be modified, but may be changed to affect various aspects of the library.

## Syntax ##
```c
#include<openrtmp/rtmp/rtmp_config.h>

#define RTMP_DEFAULT_PORT                   1935
#define RTMP_FMSVER                         "0,0,0,0"
#define RTMP_FMSVER_STR                     "LibOpenRTMP/" RTMP_FMSVER
#define RTMP_FLASHVER_VERSION               "0.0"
#define RTMP_FLASHVER_STR                   "LibOpenRTMP/" RTMP_FLASHVER_VERSION " (compatible; FMSc/1.0)"
#define RTMP_DEFAULT_WINDOW_SIZE            0x00FFFFFF
#define RTMP_DEFAULT_PEER_WINDOW_SIZE       0x0
#define RTMP_DEFAULT_BANDWIDTH_TYPE         RTMP_LIMIT_HARD
#define RTMP_MAX_CHUNK_SIZE                 0x0004FFFF
#define RTMP_DESIRED_CHUNK_SIZE             0x005FFFFF
#define RTMP_DEFAULT_IO_BUFFER_SIZE         0x00FFFFFF
#define RTMP_STREAM_STATIC_CACHE_SIZE       10
#define RTMP_STREAM_CACHE_MAX               100
#define RTMP_CONTROL_BUFFER_SIZE            16
#define RTMP_LOG_LEVEL                      4
#define RTMP_MAX_ALLOC                      10000
#define RTMP_MAX_CHUNK_CACHE                0xFFFF
#define RTMP_MAX_STREAMS                    10
#define RTMP_MAX_ASM_HARD_BUFFER            20
#define RTMP_MAX_ASM_SOFT_BUFFER            5

#define RTMP_EPOLL_MAX                      100
#define RTMP_LISTEN_SIZE                    1000
#define RTMP_REFRESH_TIME                   1000
#define RTMP_CALL_TIMEOUT                   10000

#define RTMP_POLLTECH_EPOLL

#define RTMP_SPEC_ENFORCE_HANDSHAKE_TIMES
#define RTMP_SPEC_ENFORCE_HANDSHAKE_NONCES
```

## Values ##

#### RTMP_FMSVER ####
> A string indicating the version of "Flash Media Server," or in this case, libOpenRTMP
> 

#### RTMP_FMSVER_STR ####
> A string indicating the name and version of "Flash Media Server," or in this case, libOpenRTMP
> 

#### RTMP_FLASHVER_VERSION ####
> A string indicating the version of "Flash," or in this case, libOpenRTMP
> 

#### RTMP_FLASHVER_STR ####
> A string indicating the name and version of "Flash," or in this case, libOpenRTMP
> 

#### RTMP_DEFAULT_PORT ####
> The default port for RTMP. This should probably not be changed unless you're planning on breaking RTMP compatibility.
> 

#### RTMP_DEFAULT_WINDOW_SIZE ####
> The default requested window size.
> 

#### RTMP_DEFAULT_PEER_WINDOW_SIZE ####
> Doesn't really mean anything
> 

#### RTMP_DEFAULT_BANDWIDTH_TYPE ####
> The default strategy setting for changing window size.
> 

#### RTMP_MAX_CHUNK_SIZE ####
> The maximum tolerable chunk size.
> 

#### RTMP_DESIRED_CHUNK_SIZE ####
> The desired chunk size.
> 

#### RTMP_DEFAULT_IO_BUFFER_SIZE ####
> The default number of bytes allocated for input/output buffers. It may be desireable to be able to store a few seconds worth of data in these buffers.
> 

#### RTMP_STREAM_STATIC_CACHE_SIZE ####
> The size of the static stream cache allocation. If a chunk stream ID exceeds this number, dense dynamic allocation will be used, which does have negative performance implications.
> 

#### RTMP_STREAM_CACHE_MAX ####
> The max number of stream cache entries.
> 

#### RTMP_CONTROL_BUFFER_SIZE ####
> The size of the control message buffer. If a control message can't fit in this number of bytes, the message is ignored. As of writing, the biggest documented control message is 12 bytes.
> 

#### RTMP_LOG_LEVEL ####
> The logging level. Currently, there are 5 logging levels:
> * 0 - No logging
> * 1 - Log fatal errors only (Suggested for production)
> * 2 - Log most non-fatal errors (Suggested for debugging)
> * 3 - Log all errors
> * 4 - Log all status codes other than ERR_NONE

#### RTMP_MAX_ALLOC ####
> The maximum amount of memory allowed to be allocated by a stream. Currently unimplemented.
> 

#### RTMP_MAX_CHUNK_CACHE ####
> The chunk assembler will allocate this much room for assembly. Chunks larger than this will still be fragmented. Usually, chunks larger than a few hundred bytes are media frames, which are much easier to deal with as fragments.
> 

#### RTMP_MAX_STREAMS ####
> The maximum number of message streams on one connection.
> 

#### RTMP_MAX_ASM_HARD_BUFFER ####
> The maximum number of assembler buffers. The stream will be aborted if there are more streams than this.
> 

#### RTMP_MAX_ASM_SOFT_BUFFER ####
> The maximum number of pre-allocated assembler buffers. If more buffers are needed than this, they will be allocated as needed, and destroyed once the relevant message has been assembled.
> 

#### RTMP_EPOLL_MAX ####
> The maximum number of file descriptors returned by epoll.
> 

#### RTMP_LISTEN_SIZE ####
> The maximum number of connections waiting to be accepted.
> 

#### RTMP_REFRESH_TIME ####
> The interval, in milliseconds, at which the refresh event is fired.
> 

#### RTMP_CALL_TIMEOUT ####
> How long, in milliseconds, the library will wait for a pending RPC to finish.
> 

#### RTMP_POLLTECH_EPOLL ####
> If defined, the networking implementation used will be epoll based.
> 
> As of writing this, epoll is the only available networking implementation.

#### RTMP_SPEC_ENFORCE_HANDSHAKE_TIMES ####
> If defined, connections with handshakes whose timestamps aren't correct are dropped.
> 

#### RTMP_SPEC_ENFORCE_HANDSHAKE_NONCES ####
> If defined, connections with handshakes whose nonces are incorrect are dropped.
> 

