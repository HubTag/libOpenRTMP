title: amf_push_ecma_start
--------------------------

Starts an ECMA array at the current push location.

## Syntax ##

```c
#include<openrtmp/amf/amf_object.h>

amf_err_t amf_push_ecma_start( 
	amf_t      amf, 
	uint32_t   assoc_members 
);
```

## Parameters ##
#### amf ####
> The AMF object in which the array will be started.

#### assoc_members ####
> The number of associative members which will be pushed. After this number of members are pushed, all future members pushed into this array will be pushed as ordinal members.

## Return Value ##
An AMF error code. 

## Remarks ##
For more information about push locations, see `amf_t`.

