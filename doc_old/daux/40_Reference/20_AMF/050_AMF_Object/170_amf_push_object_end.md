title: amf_push_object_end
--------------------------

Indicates the end of the complex value where values are currently being pushed.

## Syntax ##

```c
#include<openrtmp/amf/amf_object.h>

amf_err_t amf_push_object_end( 
	amf_t   amf 
);
```

## Parameters ##
#### amf ####
> The AMF object in which the value will be closed.

## Return Value ##
An AMF error code. 

## Remarks ##
Complex values are ECMA arrays and object values.

Attempting to call this function on an AMF object which has no open complex values to close will invoke undefined behavior.

Attempting to close an ECMA array before all associative members have been pushed will invoke undefined behavior.

For more information about push locations, see `amf_t`.

