title: rtmp_gen_error
--------------------------

Performs logging on a libOpenRTMP error code.

## Syntax ##

```c
#include<openrtmp/rtmp.h>

rtmp_err_t rtmp_gen_error(
    rtmp_err_t   err, 
    size_t       line, 
    const char * file, 
    const char * msg
);

#define RTMP_GEN_ERROR(err)
#define RTMP_GEN_ERROR_MSG(err, msg)
```

## Parameters ##
#### err ####
> The error code to log and return.

#### line ####
> The line number to indicate in the logs.

#### file ####
> The file name to indicate in the logs.

#### msg ####
> A message to send with the status code.


## Return Value ##
This function returns `err`.


## Remarks ##
When returning a libOpenRTMP error code, this function should be used to perform logging. LibOpenRTMP uses this internally, so any errors will still be logged, however the closer to an error this function is called, the easier the error is to track.

It is strongly suggested that you make use of the macros `RTMP_GEN_ERROR` and `RTMP_GEN_ERROR_MSG` to automatically fill in the line and file information.
