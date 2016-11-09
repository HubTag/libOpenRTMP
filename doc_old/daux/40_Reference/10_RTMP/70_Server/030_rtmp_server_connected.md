title: rtmp_server_connected
--------------------------

Check if the server is connected.

## Syntax ##

```c
#include<openrtmp/rtmp/rtmp_server.h>

bool rtmp_server_connected( 
	rtmp_server_t   server 
);
```

## Parameters ##
#### server ####
> The server to check.

## Return Value ##
The return value is a boolean which is true if the server is connected, false otherwise.

## Remarks ##
This is basically the equivalent of running `rtmp_chunk_conn_connected` on the underlying chunk connection object.
