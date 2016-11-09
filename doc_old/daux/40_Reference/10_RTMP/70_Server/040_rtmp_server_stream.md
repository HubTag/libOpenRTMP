title: rtmp_server_stream
--------------------------

Get the underlying RTMP stream for a server connection object.

## Syntax ##

```c
#include<openrtmp/rtmp/rtmp_server.h>

rtmp_stream_t rtmp_server_stream(
	rtmp_server_t   server
);
```

## Parameters ##
#### server ####
> The target server connection object.

## Return Value ##
The return value is the underlying RTMP stream object for `server`.
