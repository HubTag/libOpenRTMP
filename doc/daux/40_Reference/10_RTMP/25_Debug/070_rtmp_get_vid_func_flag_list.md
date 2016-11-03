title: rtmp_get_vid_func_flag_list
--------------------------

Gets a list of names for a video function bitset.


## Syntax ##

```c
#include<openrtmp/rtmp/rtmp_debug.h>

const char* rtmp_get_vid_func_flag_list( 
	rtmp_support_func_vid_t bitset
);
```

## Parameters ##
#### bitset ####
> A video function flag bitset.

## Return Value ##
A comma separated list of human readable strings for `bitset`.

## Remarks ##
The library uses an internally allocated static buffer for this operation. Subsequent calls to this function or similar functions will cause the return value from prior calls to no longer be valid. Additionally, repeated calls to this function with different arguments will likely yield the same pointer, though the contents will likely have changed every time.