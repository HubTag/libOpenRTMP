title: rtmp_create
--------------------------

Creates an RTMP manager object.

## Syntax ##

```c
#include<openrtmp/rtmp.h>

rtmp_t rtmp_create( void );
```

## Parameters ##
This function has no parameters.

## Return Value ##
If the return value is `nullptr`, then the function failed to create an RTMP manager object.

Otherwise, the return value is an RTMP manager object. 
