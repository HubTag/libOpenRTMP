title: amf_create
--------------------------

Creates an AMF object abstraction for manipulating and reading AMF data.

## Syntax ##

```c
#include<openrtmp/amf/amf_object.h>

amf_t amf_create( 
	char   type
);
```

## Parameters ##
#### type ####
> The version of AMF that this object should represent. Currently, only AMF0 is supported.

## Return Value ##
On success, the return value is a valid `amf_t` pointer.

On failure, `nullptr` is returned.

## Remarks ##
As of now, only AMF version 0 is supported, and the API mostly reflects this. When AMF3 support is added, the API is almost certainly going to change to be less coupled with the underlying value representations.
