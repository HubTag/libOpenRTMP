title: rtmp_get_so_evt_type_name
--------------------------

Gets the name of a shared object event code. 


## Syntax ##

```c
#include<openrtmp/rtmp/rtmp_debug.h>

const char* rtmp_get_so_evt_type_name( 
	rtmp_so_evt_t code
);
```

## Parameters ##
#### code ####
> A shared object event code.

## Return Value ##
A human readable string which corresponds to `code`.
