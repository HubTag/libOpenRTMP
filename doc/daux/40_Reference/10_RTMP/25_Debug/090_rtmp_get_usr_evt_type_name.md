title: rtmp_get_usr_evt_type_name
--------------------------

Gets the name of a user event code. 


## Syntax ##

```c
#include<openrtmp/rtmp/rtmp_debug.h>

const char* rtmp_get_usr_evt_type_name( 
	rtmp_usr_evt_t code
);
```

## Parameters ##
#### code ####
> A user event code.

## Return Value ##
A human readable string which corresponds to `code`.
