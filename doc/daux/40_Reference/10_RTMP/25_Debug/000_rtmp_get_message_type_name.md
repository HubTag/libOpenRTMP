title: rtmp_get_message_type_name
--------------------------

Gets the name of a message type code.

## Syntax ##

```c
#include<openrtmp/rtmp/rtmp_debug.h>

const char* rtmp_get_message_type_name( 
	rtmp_message_type_t code 
);
```

## Parameters ##
#### code  ####
> The message type code.

## Return Value ##
A human readable string which corresponds to `code`.
