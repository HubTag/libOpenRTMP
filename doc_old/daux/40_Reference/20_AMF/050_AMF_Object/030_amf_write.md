title: amf_write
--------------------------

Serializes AMF data into a buffer.

## Syntax ##

```c
#include<openrtmp/amf/amf_object.h>

amf_err_t amf_write( 
	amf_t    amf, 
	byte   * dest, 
	size_t   size, 
	size_t * written 
);
```

## Parameters ##
#### amf ####
> The AMF object to serialize.

#### dest ####
> The buffer which will receive the serialized data. This may be `nullptr`, which is useful to determine the size of the serialized data without actually having a buffer to receive it.

#### size ####
> The size of `dest`.

#### written ####
> An optional reference to a variable which is used to control serialization. If it is `nullptr`, then an attempt is made to serialize the whole message.
> 
> When set, the initial value indicates the index of the value to begin serialization from. 
> Upon returning, the value is set to the index after the last index which was serialized.

## Return Value ##
On success, the return value is the number of bytes written, unless `dest` is `nullptr`, in which case the returned value is the number of bytes which would have been written, given an adequately large buffer.

On failure, the return value is an AMF error code. 
