title: rtmp_app_set_fcpublish
--------------------------

Sets the FCPublish callback.

## Syntax ##

```c
#include<openrtmp/rtmp/rtmp_app.h>

void rtmp_app_on_fcpub_proc( 
	rtmp_app_t               app, 
	rtmp_app_on_fcpub_proc   proc, 
	void                   * user 
);
```

## Parameters ##
#### app ####
> The app to register the callback with.

#### proc ####
> The callback procedure to register.

#### user ####
> An optional pointer to pass into the callback.

## Return Value ##
This function does not return a value.
