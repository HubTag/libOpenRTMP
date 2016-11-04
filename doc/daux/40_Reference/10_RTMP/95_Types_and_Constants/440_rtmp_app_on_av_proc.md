title: rtmp_app_on_av_proc
--------------------------

This callback is used by apps to indicate the receipt of audiovisual content.

## Syntax ##

```c
#include<openrtmp/rtmp/rtmp_app.h>

typedef rtmp_cb_status_t (*rtmp_app_on_av_proc)(
	rtmp_stream_t   stream,
	rtmp_app_t      app,
	size_t          streamid,
	rtmp_time_t     timestamp,
	const void    * av_data,
	size_t          av_length,
	bool            final_part,
	void          * user
);
```

## Parameters ##
#### stream ####
> The stream that this callback was fired from.

#### app ####
> The app which this callback was registered with.

#### streamid ####
> The message stream that originated this message.

#### timestamp ####
> The timestamp for this message.

#### av_data ####
> A buffer containing a raw payload of audiovisual content.

#### av_length ####
> The length of `av_data`.

#### final_part ####
> Indicates that this is the final part of a message.
>
> Messages are sometimes fragmented and returned over multiple calls to rtmp_app_on_av_proc. `final_part` is only set to true once the final fragment has been delivered to the relevant callback.
> 
> If a message is small enough to not be fragmented, `final_part` is true on the first call of this callback for the message.

#### user ####
> A user-defined pointer which was registered with the app.

## Return Value ##
This callback follows the standard callback return semantics.

