title: rtmp_close_stream
--------------------------

Attempts to close the connection associated with an RTMP stream.

## Syntax ##

```c
#include<openrtmp/rtmp.h>

rtmp_err_t rtmp_close_stream( 
	rtmp_t          mgr, 
	rtmp_stream_t   stream
);
```

## Parameters ##
#### mgr ####
> The RTMP manager to disconnect the stream from.

#### stream ####
> The stream to disconnect. See remarks.

## Return Value ##
This function returns a libOpenRTMP error code.

## Remarks ##
This function is intended only to shutdown streams generated with `rtmp_listen`. While it might be tempting to try and disconnect a client with this function, that is what `rtmp_disconnect` is for. This function exists as a result of a somewhat poor architecture choice, so it might be refactored out at some point.

At some point it may be desireable to determine what kind of streams are associated with what object and act accordingly, though it might be a little bit less clear to the caller that there is a client which needs to be freed in some cases, but not in others.
