title: rtmp_so_evt_t
--------------------------

Specifies the various shared object events which may occur.


## Syntax ##

```c
typedef enum {
    RTMP_SO_EVT_USE = 1,
    RTMP_SO_EVT_RELEASE,
    RTMP_SO_EVT_REQ_CHANGE,
    RTMP_SO_EVT_CHANGE,
    RTMP_SO_EVT_SUCCESS,
    RTMP_SO_EVT_SENDMESSAGE,
    RTMP_SO_EVT_STATUS,
    RTMP_SO_EVT_CLEAR,
    RTMP_SO_EVT_REMOVE,
    RTMP_SO_EVT_REQ_REMOVE,
    RTMP_SO_EVT_USE_SUCCESS
} rtmp_so_evt_t;
```

## Members ##

#### RTMP_SO_EVT_USE ####
> 
> 

#### RTMP_SO_EVT_RELEASE ####
> 
> 

#### RTMP_SO_EVT_REQ_CHANGE ####
> 
> 

#### RTMP_SO_EVT_CHANGE ####
> 
> 

#### RTMP_SO_EVT_SUCCESS ####
> 
> 

#### RTMP_SO_EVT_SENDMESSAGE ####
> 
> 

#### RTMP_SO_EVT_STATUS ####
> 
> 

#### RTMP_SO_EVT_CLEAR ####
> 
> 

#### RTMP_SO_EVT_REMOVE ####
> 
> 

#### RTMP_SO_EVT_REQ_REMOVE ####
> 
> 

#### RTMP_SO_EVT_USE_SUCCESS ####
> 
> 

## Remarks ##

These definitions can be found on page 25 of the RTMP specification. 

These are not used currently.