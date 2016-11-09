title: amf_push_xml
--------------------------

Pushes an XML document into the current push location.

## Syntax ##

```c
#include<openrtmp/amf/amf_object.h>

amf_err_t amf_push_xml( 
	amf_t        amf, 
	const void * xml 
);
```

## Parameters ##
#### amf ####
> The AMF object into which the value will be pushed.

#### xml ####
> The string buffer to push.
>
> This value may be allocated with a previous call to `amf_push_string_alloc`. If it was, this function will consume the buffer. Otherwise, the input will be interpreted as a C string, and a copy will be made internally.

## Return Value ##
An AMF error code. 

## Remarks ##
XML documents are basically just strings. They are given their own name, but internally, they are represented exactly like a long string, just with a different name.

For more information about push locations, see `amf_t`.

