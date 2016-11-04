title: rtmp_service
--------------------------

Performs a service iteration over all connected streams.

## Syntax ##

```c
#include<openrtmp/rtmp.h>

rtmp_err_t rtmp_service(
	rtmp_t   mgr, 
	int      timeout 
);
```

## Parameters ##
#### mgr ####
> The RTMP manager to service

#### timeout ####
> The maximum number of milliseconds to block for.
>
> A timeout value of 0 will return immediately unless there is network traffic to deal with, in which case the traffic will be processed prior to this function returning.
>
> A timeout value of -1 will block forever until there is network traffic to deal with.

## Return Value ##
This function returns a libOpenRTMP error code.
