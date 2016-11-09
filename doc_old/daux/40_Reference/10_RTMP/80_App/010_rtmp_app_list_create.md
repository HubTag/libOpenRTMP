title: rtmp_app_list_create
--------------------------
 
Creates an app list.

## Syntax ##

```c
#include<openrtmp/rtmp/rtmp_app.h>

rtmp_app_list_t rtmp_app_list_create( void );
```

## Parameters ##
This function takes no parameters.

## Return Value ##
If the function succeeds, an app list is constructed and returned.

If the function fails, `nullptr` is returned.
