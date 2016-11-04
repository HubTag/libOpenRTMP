title: rtmp_server_destroy
--------------------------

Destroys an RTMP server connection object.

## Syntax ##

```c
#include<openrtmp/rtmp/rtmp_server.h>

void rtmp_server_destroy( 
	rtmp_server_t   server 
);
```

## Parameters ##
#### server ####
> A valid server connection object to destroy.
>
> Passing an invalid server connection, including `nullptr`, is undefined.

## Return Value ##
This function does not return a value.

## Remarks ##
This is currently only really used internally. RTMP managers currently manage the lifetime of RTMP server connections.