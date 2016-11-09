title: rtmp_server_set_app_list
--------------------------

Set the app list for a connection.

## Syntax ##

```c
#include<openrtmp/rtmp/rtmp_server.h>

void rtmp_server_set_app_list( 
	rtmp_server_t     server, 
	rtmp_app_list_t   list
);
```

## Parameters ##
#### server ####
> The target server connection object.

#### list ####
> The app list which will replace `server`'s current app list.

## Return Value ##
This function does not return a value.

## Remarks ##
The effects of this change are immediate. All subsequent actions on this connection will be run through the new app list, even if events have already transpired under the logic of the old list. Take care when changing an app list in the middle of an active connection.
