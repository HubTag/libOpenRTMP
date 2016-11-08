title: amf_push_date
--------------------------

Pushes a date value into the current push location.

## Syntax ##

```c
#include<openrtmp/amf/amf_object.h>

amf_err_t amf_push_date( 
	amf_t     amf, 
	double    timestamp, 
	int16_t   timezone 
);
```

## Parameters ##
#### amf ####
> The AMF object into which the value will be pushed.

#### timestamp ####
> The Unix timestamp to push.

#### timezone ####
> The timezone for this date, given in an offset from UTC in minutes. However, this is officially reserved by the AMF0 specification, and should always be 0. 

## Return Value ##
An AMF error code. 

## Remarks ##
A Unix timestamp is the number of seconds which have elapsed since 00:00:00 on January 1st, 1970 UTC. Unix timestamps do not count leap seconds. It is a very standard format, so a cursory search should give you all the information you need on Unix timestamps.

For more information about push locations, see `amf_t`.