title: amf_push_integer
--------------------------

Pushes an integer into the current push location.

## Syntax ##

```c
#include<openrtmp/amf/amf_object.h>

amf_err_t amf_push_integer( 
	amf_t     amf, 
	int32_t   number 
);
```

## Parameters ##
#### amf ####
> The AMF object into which the value will be pushed.

#### number ####
> The numeric value to push.

## Return Value ##
An AMF error code. 

## Remarks ##
For more information about push locations, see `amf_t`.

