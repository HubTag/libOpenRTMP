title: rtmp_event_t
----------------------

Contains names for events which are fired by libOpenRTMP in event callbacks.

## Syntax ##

```c
#include<openrtmp/rtmp/rtmp_constants.h>

typedef enum {
    RTMP_EVENT_CONNECT_SUCCESS,
    RTMP_EVENT_CONNECT_FAIL,
    RTMP_EVENT_CLOSED,
    RTMP_EVENT_INTERRUPTED,
    RTMP_EVENT_FAILED,
    RTMP_EVENT_FILLED,
    RTMP_EVENT_EMPTIED,
    RTMP_EVENT_REFRESH,
} rtmp_event_t;
```

## Members ##

#### RTMP_EVENT_CONNECT_SUCCESS ####
> A connection was made and a valid handshake transpired. Communication may commence.
>

#### RTMP_EVENT_CONNECT_FAIL ####
> A connection was not able to be made, or the handshake failed.
>

#### RTMP_EVENT_CLOSED ####
> The connection was closed.
>

#### RTMP_EVENT_INTERRUPTED ####
> The connection was interrupted.
>

#### RTMP_EVENT_FAILED ####
> The RTMP stream failed in some unexpected way.
>

#### RTMP_EVENT_FILLED ####
> The output buffer was filled with at least one byte.
>

#### RTMP_EVENT_EMPTIED ####
> The input buffer had at least one byte removed.
>

#### RTMP_EVENT_REFRESH ####
> Called at regular intervals.
>

