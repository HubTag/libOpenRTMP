title: rtmp_limit_t
--------------------------

Provides names to indicate how the strategy a peer should use to change their window size after a `RTMP_MSG_SET_PEER_BWIDTH` call.

## Syntax ##

```c
#include<openrtmp/rtmp/rtmp_constants.h>

typedef enum {
    RTMP_LIMIT_HARD = 0,
    RTMP_LIMIT_SOFT,
    RTMP_LIMIT_DYNAMIC
} rtmp_limit_t;
```

## Members ##

#### RTMP_LIMIT_HARD ####
> The peer should limit output bandwidth to the accompanying value.
> 
> Defined in §5.4.5 on page 21

#### RTMP_LIMIT_SOFT ####
> The peer should limit output bandwidth to the accomanying value, unless the existing limit is smaller.
> 
> Defined in §5.4.5 on page 21

#### RTMP_LIMIT_DYNAMIC ####
> If the previous limit was hard, treat this message as also being hard. Otherwise, ignore this message.
> 
> Defined in §5.4.5 on page 21
