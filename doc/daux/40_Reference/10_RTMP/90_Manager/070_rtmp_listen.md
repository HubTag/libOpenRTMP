title: rtmp_listen
--------------------------

Starts a listen loop on the RTMP manager to initiate connections with clients.

## Syntax ##

```c
#include<openrtmp/rtmp.h>

rtmp_err_t rtmp_listen( 
    rtmp_t              mgr, 
    const char        * iface, 
    short               port, 
    rtmp_connect_proc   cb, 
    void              * user 
);
```

## Parameters ##
#### mgr ####
> The manager to listen with.

#### iface ####
> A string representing the network interface to listen on. 
>
> This may be `RTMP_ADDR_ANY`, which refers to all interfaces.

#### port ####
> The port to listen on for connections.
>
> It is suggested that you use `RTMP_DEFAULT_PORT`

#### cb ####
> An optional `rtmp_connect_proc` which will be called when a connection is established.
>
> If no callback is desired, pass in `nullptr`.

#### user ####
> A pointer which will be passed into `cb`.

## Return Value ##
This function returns a libOpenRTMP error code.
