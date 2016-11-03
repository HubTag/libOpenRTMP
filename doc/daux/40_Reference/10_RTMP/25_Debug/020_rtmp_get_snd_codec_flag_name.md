title: rtmp_get_snd_codec_flag_name
--------------------------

Gets the name of a sound codec flag. 


## Syntax ##

```c
#include<openrtmp/rtmp/rtmp_debug.h>

const char* rtmp_get_snd_codec_flag_name( 
	rtmp_support_codec_snd_t flag
);
```

## Parameters ##
#### flag ####
> A sound codec flag.

## Return Value ##
A human readable string which corresponds to `flag`.

## Remarks ##
This function will only recognize individual flags. If you want to pass in a whole bitset, please use `rtmp_get_snd_codec_flag_list`.