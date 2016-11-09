title: amf_push_reference
--------------------------

Pushes a reference offset into the current push location.

## Syntax ##

```c
#include<openrtmp/amf/amf_object.h>

amf_err_t amf_push_reference( 
	amf_t          amf, 
	unsigned int   ref 
);
```

## Parameters ##
#### amf ####
> The AMF object into which the value will be pushed.

#### ref ####
> The reference offset to push.

## Return Value ##
An AMF error code. 

## Remarks ##
Reference offsets are simply a numeric value indicating the nth object value which was pushed into this AMF object. As of now, there is minimal support for references.

For more information about push locations, see `amf_t`.

