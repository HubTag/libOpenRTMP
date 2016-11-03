title: rtmp_cb_status_t
--------------------------

Contains names for status codes which may be returned by callbacks to control the flow of an RTMP stream.


## Syntax ##

```c
#include<openrtmp/rtmp/rtmp_constants.h>

typedef enum {
    RTMP_CB_CONTINUE = 0,
    RTMP_CB_ERROR,
    RTMP_CB_DEFER_PAUSE,
    RTMP_CB_ABORT
} rtmp_cb_status_t;
```

## Members ##

#### RTMP_CB_CONTINUE ####
> The callback completed successfully. Continue processing the stream.
> 

#### RTMP_CB_ERROR ####
> The callback suffered an error. Abort processing this message. This message is not removed from the input buffer.
> 

#### RTMP_CB_DEFER_PAUSE ####
> The callback wishes to abort processing this message, and pause the processing on this stream until further notice. This message is not removed from the input buffer.
> 

#### RTMP_CB_ABORT ####
> Abort the stream entirely.
> 
