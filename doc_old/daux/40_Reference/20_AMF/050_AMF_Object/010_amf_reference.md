title: amf_reference
--------------------------

Increments the reference count on an AMF object.

## Syntax ##

```c
#include<openrtmp/amf/amf_object.h>

amf_t amf_reference( 
	amf_t   other 
);
```

## Parameters ##
#### other ####
> An AMF value to take a reference from.

## Return Value ##
Returns `other`.

## Remarks ##
This will increase an internal reference count for `other`. An additional `amf_destroy` call must be made for every time `amf_reference` is called on an AMF object.
