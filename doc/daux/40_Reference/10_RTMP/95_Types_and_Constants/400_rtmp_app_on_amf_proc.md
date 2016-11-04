title: rtmp_app_on_amf_proc
--------------------------

This callback is used by apps to indicate the receipt of an AMF-based RTMP message.

## Syntax ##

```c
#include<openrtmp/rtmp/rtmp_app.h>

typedef rtmp_cb_status_t (*rtmp_app_on_amf_proc)(
	rtmp_stream_t   stream,
	rtmp_app_t      app,
	amf_t           args,
	void          * user
);
```

## Parameters ##
#### stream ####
> The stream that this callback was fired from.

#### app ####
> The app which this callback was registered with.

#### args ####
> An AMF object containing data for an AMF-based RTMP message.

#### user ####
> A user-defined pointer which was registered with the app.

## Return Value ##
This callback follows the standard callback return semantics.

## Remarks ##
This is the callback used for all AMF-based RTMP messages. No distinction is made between shared object messages, command messages, or data messages.