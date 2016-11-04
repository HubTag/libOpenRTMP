title: rtmp_t
--------------------------

A RTMP manager object.

## Syntax ##

```c
#include<openrtmp/rtmp.h>

typedef struct rtmp_mgr * rtmp_t;
```

## Functions ##
#### [rtmp_create](rtmp_create.html) ####
> Creates an RTMP manager object.

#### [rtmp_destroy](rtmp_destroy.html) ####
> Destroys an RTMP manager object.

#### [rtmp_set_app_list](rtmp_set_app_list.html) ####
> Sets the app list to use for connections made to this manager.

#### [rtmp_service](rtmp_service.html) ####
> Performs a service iteration over all connected streams.

#### [rtmp_connect](rtmp_connect.html) ####
> Attempts to connect an RTMP client object to an RTMP manager object.

#### [rtmp_disconnect](rtmp_disconnect.html) ####
> Disconnects a client from a manager.

#### [rtmp_close_stream](rtmp_close_stream.html) ####
> Attempts to close the connection associated with an RTMP stream.

#### [rtmp_listen](rtmp_listen.html) ####
> Starts a listen loop on the RTMP manager to initiate connections with clients.

#### [rtmp_amferr](rtmp_amferr.html) ####
> Returns a libOpenRTMP error code derived from a libOpenRTMP AMF error code.

#### [rtmp_gen_error](rtmp_gen_error.html) ####
> Performs logging on a libOpenRTMP error code.

## Remarks ##
This object is used to move data back and forth between RTMP streams and the network. This is one of the primary interfaces for accessing RTMP functionality in libOpenRTMP.