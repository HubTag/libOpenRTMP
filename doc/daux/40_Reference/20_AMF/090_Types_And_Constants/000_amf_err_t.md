title: amf_err_t
--------------------------

Refers to either a length or an AMF error code.

## Syntax ##

```c
#include<openrtmp/amf/amf_constants.h>

#define AMF_SIZE(n) ((amf_err_t)(n))

typedef enum AMF_ERR {
    AMF_ERR_NONE,
    AMF_ERR_INVALID_DATA = -10,
    AMF_ERR_INCOMPLETE,
    AMF_ERR_NEED_NAME,
    AMF_ERR_OOM,
    AMF_ERR_BAD_ALLOC
} amf_err_t;
```

## Members ##
#### AMF_ERR_NONE ####
> No error occured.
> 

#### AMF_ERR_INVALID_DATA ####
> Invalid data was provided to the codec.
> 

#### AMF_ERR_INCOMPLETE ####
> The data stream ended unexpectedly.
> 

#### AMF_ERR_NEED_NAME ####
> An attempt to write a member without first writing a member name was made.
> 

#### AMF_ERR_OOM ####
> An attempt to allocate dynamic memory failed.
> 

#### AMF_ERR_BAD_ALLOC ####
> Unused.
> 


## Remarks ##
Values of this type perform double duty. They are used both to indicate an error status with an AMF operation, as well as report on sizes. If the numeric value is less than zero, then there is an error. If it is zero or greater, then the operation was a success, and the value usually indicates how many bytes were read, written, or otherwise consumed.
