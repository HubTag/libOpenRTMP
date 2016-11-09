title: rtmp_disconnect
--------------------------

Disconnects a client from a manager.

## Syntax ##

```c
#include<openrtmp/rtmp.h>

rtmp_err_t rtmp_disconnect( 
	rtmp_t          mgr, 
	rtmp_client_t   client 
);
```

## Parameters ##
#### mgr ####
> The RTMP manager to disconnect from.

#### client ####
> The RTMP client object to attempt to disconnect from `mgr`.

## Return Value ##
This function returns a libOpenRTMP error code.