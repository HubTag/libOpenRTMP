Contains names for various useful values throughout the library.

## Syntax ##

```c
#include<openrtmp/rtmp/rtmp_constants.h>

#define RTMP_NONCE_SIZE                     1528
#define RTMP_VERSION                        3
#define RTMP_CACHE_AGGREGATE                0
#define RTMP_MESSAGE_HEADER_SIZE            (1 + 3 + 4 + 3)
#define RTMP_ANY                            0xFFFFFF
#define RTMP_CAPABILITIES                   0x1F
#define RTMP_MODE                           1
#define RTMP_ADDR_ANY                       "0.0.0.0"
#define RTMP_DEFAULT_CHUNK_SIZE             128
```

## Values ##

#### RTMP_NONCE_SIZE ####
> The size of handshake nonces.
> 
> Defined in §5.2.3 on page 8

#### RTMP_VERSION ####
> The RTMP version.
>
> Defined in §5.2.2 on page 7

#### RTMP_MESSAGE_HEADER_SIZE ####
> The maximum size of a message header.
> 
> Defined in §5.3.1 on page 12

#### RTMP_ANY ####
> When specifying a stream event, this will indicate that the callback should fire for any event of the given class.
> 

#### RTMP_CAPABILITIES ####
> The indicated capabilities of the server. (I have no idea what this means; it was reverse engineered.)
> 

#### RTMP_MODE ####
> The indicated mode of the server (I have no idea what this means; it was reverse engineered.)
> 

#### RTMP_ADDR_ANY ####
> A bind address indicating any network device.
> 

#### RTMP_DEFAULT_CHUNK_SIZE ####
> The default size of a chunk.
> 
> Defined in §5.4.1 on page 19

#### RTMP_CONTROL_MSG_STREAM ####
> The message stream ID of the control stream.
> 
> Defined in §5.4 on page 18

#### RTMP_CONTROL_CHUNK_STREAM ####
> The chunk stream ID of the control stream.
> 
> Defined in §5.4 on page 18

