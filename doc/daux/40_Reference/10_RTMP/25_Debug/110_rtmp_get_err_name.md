title: rtmp_get_err_name
--------------------------

Gets the name of an error code. 


## Syntax ##

```c
#include<openrtmp/rtmp/rtmp_debug.h>

const char* rtmp_get_err_name( 
	rtmp_err_t code
);
```

## Parameters ##
#### err ####
> A libOpenRTMP error code.

## Return Value ##
A human readable string which corresponds to `code`.
