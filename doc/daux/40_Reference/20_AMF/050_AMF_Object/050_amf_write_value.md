title: amf_write_value
--------------------------

Serializes a single AMF value into a buffer.

## Syntax ##

```c
#include<openrtmp/amf/amf_object.h>

amf_err_t amf_write_value( 
	amf_value_t   value, 
	byte        * dest, 
	size_t        size 
);
```

## Parameters ##
#### value ####
> The AMF value to serialize.

#### dest ####
> The buffer which will receive the serialized data. This may be `nullptr`, which is useful to determine the size of the serialized data without actually having a buffer to receive it.

#### size ####
> The size of `dest`.

## Return Value ##
On success, the return value is the number of bytes written, unless `dest` is `nullptr`, in which case the returned value is the number of bytes which would have been written, given an adequately large buffer.

On failure, the return value is an AMF error code. 

## Remarks ##
`amf_write` calls this function internally on all of its members. If you wish to shortcircuit `amf_write`, this is how you do it.
