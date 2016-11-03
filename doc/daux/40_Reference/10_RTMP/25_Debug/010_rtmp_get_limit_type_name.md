title: rtmp_get_limit_type_name
--------------------------

Gets the name of a bandwidth limit strategy code. 


## Syntax ##

```c
#include<openrtmp/rtmp/rtmp_debug.h>

const char* rtmp_get_limit_type_name( 
	rtmp_limit_t code
);
```

## Parameters ##
#### bitset ####
> A bandwidth limit strategy code.

## Return Value ##
A human readable string which corresponds to `code`.
