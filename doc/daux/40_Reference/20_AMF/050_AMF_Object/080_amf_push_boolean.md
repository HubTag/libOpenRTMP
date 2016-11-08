title: amf_push_boolean
--------------------------

Pushes a boolean into the current push location.

## Syntax ##

```c
#include<openrtmp/amf/amf_object.h>

amf_err_t amf_push_boolean( 
	amf_t   amf, 
	char    boolean 
);
```

## Parameters ##
#### amf ####
> The AMF object into which the value will be pushed.

#### boolean ####
> The boolean value to push.

## Return Value ##
An AMF error code. 

## Remarks ##
For more information about push locations, see `amf_t`.

