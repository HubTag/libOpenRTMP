title: rtmp_server_create
--------------------------

Creates an RTMP server object. 


## Syntax ##

```c
#include<openrtmp/rtmp/rtmp_server.h>

rtmp_server_t rtmp_server_create( void );
```

## Parameters ##
This function takes no parameters.

## Return Value ##
If the function succeeds, the return value is an RTMP server object.

If the function fails, the return value is `nullptr`.

## Remarks ##
This is currently only really used internally.
