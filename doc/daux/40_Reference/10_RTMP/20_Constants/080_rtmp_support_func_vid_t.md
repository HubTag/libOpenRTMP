title: rtmp_support_func_vid_t
--------------------------

A bitflag indicating whether a client supports various functions.


## Syntax ##

```c
#include<openrtmp/rtmp/rtmp_constants.h>

typedef enum {
    RTMP_SUPPORT_VID_CLIENT_SEEK = 0x0001
} rtmp_support_func_vid_t;
```

## Members ##

#### RTMP_SUPPORT_VID_CLIENT_SEEK ####
> Indicates that the player can perform frame-accurate seeks.
> 
> Defined in §7.2.1.1 on page 32