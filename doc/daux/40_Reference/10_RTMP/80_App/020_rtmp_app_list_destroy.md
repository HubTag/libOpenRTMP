title: rtmp_app_list_destroy
--------------------------

Destroys an app list. 


## Syntax ##

```c
#include<openrtmp/rtmp/rtmp_app.h>

void rtmp_app_list_destroy( 
	rtmp_app_list_t   list 
);
```

## Parameters ##
#### list ####
> A valid list object.
>
> Passing an invalid list object, including `nullptr`, is undefined behavior.

## Return Value ##
This function does not return a value.

## Remarks ##
Be very careful when destroying app lists. There is currently no reference counting on app lists, so any outstanding connections must not have their app lists deleted. Make sure you've disconnected all connections which make use of the app list before destroying the list.