title: amf_destroy
--------------------------

Decreases the reference count and/or destroys an AMF object.

## Syntax ##

```c
#include<openrtmp/amf/amf_object.h>

void amf_destroy( 
	amf_t   amf 
);
```

## Parameters ##
#### amf ####
> The AMF object which should be destroyed.


## Return Value ##
This function returns no value.

## Remarks ##
If the AMF object has been referenced, an additional call to `amf_destroy` must be made for every time it was referenced before it will actually be destroyed.
