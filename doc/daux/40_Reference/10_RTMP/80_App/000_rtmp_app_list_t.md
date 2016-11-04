title: rtmp_app_list_t
--------------------------

A list of RTMP applications.

## Syntax ##

```c
#include<openrtmp/rtmp/rtmp_app.h>

typedef struct rtmp_app_list * rtmp_app_list_t;
```

## Remarks ##
This object is used to store RTMP applications by name so that they may be associated with an RTMP manager, and subsequently, RTMP connections.