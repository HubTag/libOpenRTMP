title: amf_push_long_string
--------------------------

Pushes a long string into the current push location.

## Syntax ##

```c
#include<openrtmp/amf/amf_object.h>

amf_err_t amf_push_long_string( 
	amf_t        amf, 
	const void * str 
);
```

## Parameters ##
#### amf ####
> The AMF object into which the value will be pushed.

#### str ####
> The string buffer to push.
>
> This value may be allocated with a previous call to `amf_push_string_alloc`. If it was, this function will consume the buffer. Otherwise, the input will be interpreted as a C string, and a copy will be made internally.

## Return Value ##
An AMF error code. 

## Remarks ##
For more information about push locations, see `amf_t`.

