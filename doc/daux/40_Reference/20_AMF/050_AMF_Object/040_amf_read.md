title: amf_read
--------------------------

Deserializes AMF data from a buffer.

## Syntax ##

```c
#include<openrtmp/amf/amf_object.h>

amf_err_t amf_read( 
	amf_t        amf, 
	const byte * src, 
	size_t       size, 
	size_t     * read 
);
```

## Parameters ##
#### amf ####
> The AMF object to read AMF data into.

#### src ####
> The buffer which holds the AMF data to deserialize.

#### size ####
> The size of `src`.

#### read ####
> An optional reference to a variable which, upon failure, will be filled with the number of bytes which were read from `src`. This value is not filled on success.

## Return Value ##
On success, the number of bytes which were read is returned. Otherwise, an AMF error code is returned.

## Remarks ##
If the error code is `AMF_ERR_INCOMPLETE`, then it is safe to advance the buffer by the amount stored in `read`, fill it with more data, and call this function again. This failure mode is for reading AMF data out of multiple packets of data. Other error codes indicate a deeper failure in the data, and as a result, indicate that processing of the message should be aborted.