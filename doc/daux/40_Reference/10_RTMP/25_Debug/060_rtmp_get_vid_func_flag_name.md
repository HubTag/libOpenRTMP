title: rtmp_get_vid_func_flag_name
--------------------------

Gets the name of a video function flag. 


## Syntax ##

```c
#include<openrtmp/rtmp/rtmp_debug.h>

const char* rtmp_get_vid_func_flag_name( 
	rtmp_support_func_vid_t flag
);
```

## Parameters ##
#### flag ####
> A video function flag.

## Return Value ##
A human readable string which corresponds to `flag`.

## Remarks ##
This function will only recognize individual flags. If you want to pass in a whole bitset, please use `rtmp_get_vid_func_flag_list`.