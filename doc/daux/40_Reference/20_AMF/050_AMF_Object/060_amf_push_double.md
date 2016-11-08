title: amf_push_double
--------------------------

Pushes a double into the current push location.

## Syntax ##

```c
#include<openrtmp/amf/amf_object.h>

amf_err_t amf_push_double( 
	amf_t    amf, 
	double   number 
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
