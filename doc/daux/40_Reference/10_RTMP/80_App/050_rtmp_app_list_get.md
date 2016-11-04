title: rtmp_app_list_get
--------------------------

Gets an app object by name.

## Syntax ##

```c
#include<openrtmp/rtmp/rtmp_app.h>

rtmp_app_t rtmp_app_list_get( 
	rtmp_app_list_t   list, 
	const char      * appname 
);
```

## Parameters ##
#### list ####
> The app list to fetch the app from.

#### appname ####
> The name of the app to fetch.

## Return Value ##
If there is an app with a name that matches `appname`, then that app is returned.

If no such app exists, `nullptr` is returned.
