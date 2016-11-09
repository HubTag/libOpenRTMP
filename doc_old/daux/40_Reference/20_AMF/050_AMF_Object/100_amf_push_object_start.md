title: amf_push_object_start
--------------------------

Starts an object at the current push location.

## Syntax ##

```c
#include<openrtmp/amf/amf_object.h>

amf_err_t amf_push_object_start( 
	amf_t   amf
);
```

## Parameters ##
#### amf ####
> The AMF object in which the object will be started.

## Return Value ##
An AMF error code. 

## Remarks ##
For more information about push locations, see `amf_t`.

