title: rtmp_app_on_pub_proc
--------------------------

This callback is used by apps to indicate the receipt of a publish remote procedure call.

## Syntax ##

```c
#include<openrtmp/rtmp/rtmp_app.h>

typedef rtmp_cb_status_t (*rtmp_app_on_pub_proc)(
	rtmp_stream_t   stream,
	rtmp_app_t      app,
	const char    * name,
	const char    * type,
	void          * user
);
```

## Parameters ##
#### stream ####
> The stream that this callback was fired from.

#### app ####
> The app which this callback was registered with.

#### name ####
> The name of the stream. Often called the stream key.

#### type ####
> A string indicating the type of stream this is.
> 
> Common values are "live", "record", and "append". See Publishing.

#### user ####
> A user-defined pointer which was registered with the app.

## Return Value ##
This callback follows the standard callback return semantics.

