title: rtmp_app_video
--------------------------

Fires the the video callback on an app. 

## Syntax ##

```c
#include<openrtmp/rtmp/rtmp_app.h>

rtmp_cb_status_t rtmp_app_video( 
	rtmp_stream_t   stream, 
	rtmp_app_t      app, 
	size_t          streamid, 
	rtmp_time_t     timestamp, 
	const void    * av_data, 
	size_t          av_length, 
	bool            final_part 
);
```

## Parameters ##
#### stream ####
> The RTMP stream to fire the video event on.

#### app ####
> The app to use for firing the callback.

#### streamid ####
> The message stream that originated this message.

#### timestamp ####
> The timestamp for this message.

#### av_data ####
> A buffer containing a raw payload of video content.

#### av_length ####
> The length of `av_data`.

#### final_part ####
> Indicates that this is the final part of a message.
>
> Messages are sometimes fragmented and returned over multiple calls to rtmp_app_on_av_proc. `final_part` is only set to true once the final fragment has been delivered to the relevant callback.
> 
> If a message is small enough to not be fragmented, `final_part` is true on the first call of this callback for the message.

## Return Value ##
This function returns the result of the callback.

If no callback was specified, the return value is `RTMP_CB_CONTINUE`.

## See Also ##
* Video Messages