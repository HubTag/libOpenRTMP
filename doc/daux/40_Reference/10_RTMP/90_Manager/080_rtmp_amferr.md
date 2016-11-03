title: rtmp_amferr
--------------------------

Returns a libOpenRTMP error code derived from a libOpenRTMP AMF error code.

## Syntax ##

```c
#include<openrtmp/rtmp.h>

rtmp_err_t rtmp_amferr( 
    amf_err_t   err 
);
```

## Parameters ##
#### err ####
> A libOpenRTMP AMF error code.

## Return Value ##
This function returns a libOpenRTMP error code based on `err`.

## Remarks ##
An attempt is made to convert the AMF error code into an equivalend RTMP error code. This is merely a convenience function if an RTMP function depends on the success of an AMF processing function.
