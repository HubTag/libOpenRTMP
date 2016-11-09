title: rtmp_app_set_release
--------------------------

Sets the releaseStream callback.

## Syntax ##

```c
#include<openrtmp/rtmp/rtmp_app.h>

void rtmp_app_set_release( 
	rtmp_app_t             app, 
	rtmp_app_on_amf_proc   proc, 
	void                 * user 
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
