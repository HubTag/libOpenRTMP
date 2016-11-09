title: rtmp_app_list_register
--------------------------

Registers a new application with the app list.

## Syntax ##

```c
#include<openrtmp/rtmp/rtmp_app.h>

rtmp_app_t rtmp_app_list_register( 
	rtmp_app_list_t   list, 
	const char      * appname 
);
```

## Parameters ##
#### list ####
> The app list to register an application with.

#### appname ####
> The name of the application to register.

## Return Value ##
If this function succeeds, the return value is a valid app object.

If this function fails, `nullptr` is returned.
