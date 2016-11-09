title: rtmp_connect
--------------------------

Attempts to connect an RTMP client object to an RTMP manager object.

## Syntax ##

```c
#include<openrtmp/rtmp.h>

rtmp_err_t rtmp_connect( 
	rtmp_t          mgr, 
	rtmp_client_t   client 
);
```

## Parameters ##
#### mgr ####
> The RTMP manager to connect to.

#### client ####
> The RTMP client to connect with.

## Return Value ##
This function returns a libOpenRTMP error code.

## Remarks ##
After calling this connection, `client` will immediately attempt to connect to the pre-specified destination.