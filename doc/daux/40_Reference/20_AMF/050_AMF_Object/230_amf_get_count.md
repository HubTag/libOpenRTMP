title: amf_get_count
--------------------------

Returns the number of values in an AMF object.

## Syntax ##

```c
#include<openrtmp/amf/amf_object.h>

size_t amf_get_count( 
	amf_t   amf 
);
```

## Parameters ##
#### amf ####
> The AMF object whose member count will be fetched.

## Return Value ##
The number of values in `amf`.
