title: rtmp_get_event_name
--------------------------

Gets the name of an event code. 


## Syntax ##

```c
#include<openrtmp/rtmp/rtmp_debug.h>

const char* rtmp_get_event_name( 
	rtmp_event_t code
);
```

## Parameters ##
#### code ####
> A libOpenRTMP event code.

## Return Value ##
A human readable string which corresponds to `code`.
