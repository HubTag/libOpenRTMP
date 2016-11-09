title: amf_push_member
--------------------------

Pushes a member name for the current push location.

## Syntax ##

```c
#include<openrtmp/amf/amf_object.h>

amf_err_t amf_push_member( 
	amf_t        amf, 
	const void * str 
);
```

## Parameters ##
#### amf ####
> The AMF object into which the member will be pushed.

#### str ####
> A string buffer which contains the name of the member.
>
> This value may be allocated with a previous call to `amf_push_string_alloc`. If it was, this function will consume the buffer. Otherwise, the input will be interpreted as a C string, and a copy will be made internally.

## Return Value ##
An AMF error code. 

## Remarks ##
All associative values use this function to set the names for any keys. This includes both ECMA arrays and objects.

If an AMF push function returns `AMF_ERR_NEED_NAME`, that means this function needs to be called to set the member name for the next value.

For more information about push locations, see `amf_t`.

