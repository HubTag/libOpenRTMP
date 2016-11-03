title: rtmp_err_t
--------------------------

Contains names for error codes which are returned by more libOpenRTMP calls.


## Syntax ##

```c
#include<openrtmp/rtmp/rtmp_constants.h>

typedef enum {
    RTMP_ERR_NONE = 0,
    RTMP_ERR_CONNECTION_CLOSED,
    RTMP_ERR_DIVERGENCE_METER_ERROR,
    RTMP_ERR_ERROR,
    RTMP_ERR_NOT_READY,
    RTMP_ERR_INVALID,
    RTMP_ERR_BAD_WRITE,
    RTMP_ERR_BAD_READ,
    RTMP_ERR_PAUSE,
    RTMP_ERR_AGAIN,
    RTMP_ERR_DNS_FAIL,
    RTMP_ERR_POLL_FAIL,
    RTMP_ERR_FATAL,
    RTMP_ERR_CONNECTION_FAIL,
    RTMP_ERR_OOM,
    RTMP_ERR_INADEQUATE_CHUNK,
    RTMP_ERR_ABORT
} rtmp_err_t;
```

## Members ##

#### RTMP_ERR_NONE ####
> No error occured.
> 
> Severity: N/A

#### RTMP_ERR_DIVERGENCE_METER_ERROR ####
> The value of a window acknowledgement indicates that the recipient has more data than you sent. Are they in the future?
> 
> Severity: Low

#### RTMP_ERR_POLL_FAIL ####
> The underlying network polling mechanism aborted unexpectedly.
> 
> Severity: Low

#### RTMP_ERR_NOT_READY ####
> The stream has not completed the handshake process yet.
> 
> Severity: Low

#### RTMP_ERR_PAUSE ####
> The stream is paused.
> 
> Severity: Low

#### RTMP_ERR_AGAIN ####
> The data could not be completely written into the output buffer.
> 
> Severity: Low

#### RTMP_ERR_ERROR ####
> A generic error. Usually returned because of callbacks which return `RTMP_CB_ERROR`.
> 
> Severity: Medium

#### RTMP_ERR_DNS_FAIL ####
> A domain name failed to resolve.
> 
> Severity: Medium

#### RTMP_ERR_BAD_WRITE ####
> An attempt to write information (usually AMF data) has failed for some reason.
> 
> Severity: Medium

#### RTMP_ERR_BAD_READ ####
> An attempt to read information (usually AMF data) has failed for some reason.
> 
> Severity: Medium

#### RTMP_ERR_INVALID ####
> Invalid data found in the stream. This is often fatal, though sometimes not.
> 
> Severity: Medium

#### RTMP_ERR_FATAL ####
> A generic fatal error. Fatal errors indicate some unreconcileable problem with local or global state.
> 
> Severity: Fatal

#### RTMP_ERR_CONNECTION_FAIL ####
> The underlying transport layer has failed in some fashion.
> 
> Severity: Fatal

#### RTMP_ERR_OOM ####
> Insufficient memory to perform the request.
> 
> Severity: Fatal

#### RTMP_ERR_INADEQUATE_CHUNK ####
> The chunk cache is full. This means that more than `RTMP_STREAM_CACHE_MAX` chunk IDs are in use.
> 
> Severity: Fatal

#### RTMP_ERR_ABORT ####
> The stream is aborting for some reason, usually as a result of a callback.
> 
> Severity: Fatal
