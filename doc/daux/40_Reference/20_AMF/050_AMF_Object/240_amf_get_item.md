title: amf_get_item
--------------------------

Returns the AMF value at the given index in an AMF object.

## Syntax ##

```c
#include<openrtmp/amf/amf_object.h>

amf_value_t amf_get_item( 
	amf_t          amf, 
	const size_t   idx 
);
```

## Parameters ##
#### amf ####
> The AMF object from which the AMF value will be fetched.

#### idx ####
> The index of the value to fetch.

## Return Value ##
The AMF value from `amf` at index `idx`.

## Remarks ##
Attempting to access an index which is out of bounds is undefined.
