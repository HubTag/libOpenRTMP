title: rtmp_app_publish
--------------------------

Fires the Publish callback on an app. 

## Syntax ##

```c
#include<openrtmp/rtmp/rtmp_app.h>

rtmp_cb_status_t rtmp_app_publish( 
	rtmp_stream_t   stream, 
	rtmp_app_t      app, 
	const char    * name, 
	const char    * type 
);
```

## Parameters ##
#### stream ####
> The RTMP stream to fire the Publish event on.

#### app ####
> The app to use for firing the callback.

#### name ####
> The name of the stream. Also referred to as the stream key.

#### type ####
> The broadcast type.
> 
> Common values are "live", "record", and "append". See Publishing.

## Return Value ##
This function returns the result of the callback.

If no callback was specified, the return value is `RTMP_CB_CONTINUE`.

## See Also ##
* RPC / Publish