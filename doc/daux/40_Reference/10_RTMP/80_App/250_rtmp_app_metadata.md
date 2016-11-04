title: rtmp_app_metadata
--------------------------

Fires the onMetadata callback on an app. 

## Syntax ##

```c
#include<openrtmp/rtmp/rtmp_app.h>

rtmp_cb_status_t rtmp_app_metadata( 
	rtmp_stream_t   stream, 
	rtmp_app_t      app, 
	amf_t           params 
);
```

## Parameters ##
#### stream ####
> The RTMP stream to fire the onMetadata event on.

#### app ####
> The app to use for firing the callback.

#### params ####
> The AMF parameters.

## Return Value ##
This function returns the result of the callback.

If no callback was specified, the return value is `RTMP_CB_CONTINUE`.

## See Also ##
* RPC / onMetadata