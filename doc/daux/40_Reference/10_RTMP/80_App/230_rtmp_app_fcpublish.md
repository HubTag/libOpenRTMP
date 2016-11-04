title: rtmp_app_fcpublish
--------------------------

Fires the FCPublish callback on an app. 

## Syntax ##

```c
#include<openrtmp/rtmp/rtmp_app.h>

rtmp_cb_status_t rtmp_app_fcpublish( 
	rtmp_stream_t   stream, 
	rtmp_app_t      app, 
	const char    * name 
);
```

## Parameters ##
#### stream ####
> The RTMP stream to fire the FCPublish event on.

#### app ####
> The app to use for firing the callback.

#### name ####
> The name of the stream. Also referred to as the stream key.

## Return Value ##
This function returns the result of the callback.

If no callback was specified, the return value is `RTMP_CB_CONTINUE`.

## See Also ##
* RPC / FCPublish