title: amf_push_string_alloc
--------------------------

Prepares a buffer into which strings may be written.

## Syntax ##

```c
#include<openrtmp/amf/amf_object.h>

amf_err_t amf_push_string_alloc( 
	amf_t     amf, 
	void   ** destination, 
	size_t    length 
);
```

## Parameters ##
#### amf ####
> The AMF object for which the buffer is going to be allocated.

#### destination ####
> A reference to a `void *` which will receive the buffer pointer.

#### length ####
> The length in bytes of the buffer which is being requested. This must be the length of the string without a null terminator. All functions assume that there are no null terminators on the buffers and will use the length of the buffer as the string length. As such, overallocating will lead to uninitialized access to the end of the buffer.

## Return Value ##
An AMF error code.

## Remarks ##
All of the push functions which take string values in the AMF library rely on this function to allocate space for the strings. First, this function is called to allocate a buffer, then the string is `memcpy`'d into the allocated buffer, and then the buffer is pushed.

If the user passes in an external string buffer to a push function, then this function is called internally to make a copy of the string.

After calling `amf_push_string_alloc`, the buffer should be pushed into the object before calling it again. There is a single internal slot which is used to store a reference to the allocation, and if that slot is not emptied with a push prior to calling the function again, memory will be leaked. Attempts to push an old buffer after overwriting the slot will result in the object not realizing that it's an internal allocation, and will treat it like a normal string buffer. Since a null pointer won't have been written to the end of the buffer, this will not only leak memory, but it will also invoke undefined behavior.