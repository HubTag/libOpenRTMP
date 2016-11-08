title: amf_push_null
--------------------------

Pushes a null value into the current push location.

## Syntax ##

```c
#include<openrtmp/amf/amf_object.h>

amf_err_t amf_push_null( 
	amf_t   amf 
);
```

## Parameters ##
#### amf ####
> The AMF object into which the value will be pushed.

## Return Value ##
An AMF error code. 

## Remarks ##
For more information about push locations, see `amf_t`.

