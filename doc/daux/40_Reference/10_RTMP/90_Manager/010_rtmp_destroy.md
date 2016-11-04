title: rtmp_destroy
--------------------------

Destroys an RTMP manager object.

## Syntax ##

```c
#include<openrtmp/rtmp.h>

void rtmp_destroy( 
	rtmp_t   mgr
);
```

## Parameters ##
#### mgr ####
> A pointer to a valid RTMP manager.
> 
> Passing an invalid pointer, including `nullptr`, is undefined.

## Return Value ##
This function does not return a value.
