title: rtmp_usr_evt_t
--------------------------

Specifies the various user events that may occur.


## Syntax ##

```c
typedef enum {
    RTMP_USR_EVT_STREAM_BEGIN = 0,
    RTMP_USR_EVT_STREAM_EOF,
    RTMP_USR_EVT_STREAM_DRY,
    RTMP_USR_EVT_SET_BUF_LEN,
    RTMP_USR_EVT_STREAM_IS_REC,
    RTMP_USR_EVT_PING_REQ,
    RTMP_USR_EVT_PING_RES
} rtmp_usr_evt_t;
```

## Members ##

#### RTMP_USR_EVT_STREAM_BEGIN ####
> 
> 

#### RTMP_USR_EVT_STREAM_EOF ####
> 
> 

#### RTMP_USR_EVT_STREAM_DRY ####
> 
> 

#### RTMP_USR_EVT_SET_BUF_LEN ####
> 
> 

#### RTMP_USR_EVT_STREAM_IS_REC ####
> 
> 

#### RTMP_USR_EVT_PING_REQ ####
> 
> 

#### RTMP_USR_EVT_PING_RES ####
> 
> 

## Remarks ##

These definitions can be found on page 27 of the RTMP specification.

These are not used currently.