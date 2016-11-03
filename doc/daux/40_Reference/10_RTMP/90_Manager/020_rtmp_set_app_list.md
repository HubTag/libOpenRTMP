title: rtmp_set_app_list
--------------------------

Sets the app list to use for connections made to this manager.

## Syntax ##

```c
#include<openrtmp/rtmp.h>

void rtmp_set_app_list( 
    rtmp_t            mgr, 
    rtmp_app_list_t   list 
);
```

## Parameters ##
#### mgr ####
> The manager to associate an app list with.

#### list ####
> An app list to associate with `mgr`.

## Return Value ##
This function does not return a value.

## Remarks ##
Only connections established after this function is called will use the provided app list. Any outstanding connections will continue to use the app list which was associated with them when the connections were established.
