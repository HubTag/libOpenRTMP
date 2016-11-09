title: amf_value_t
--------------------------

Refers to a value encapsulated within an `amf_t`

## Syntax ##

```c
#include<openrtmp/amf/amf_object.h>

typedef union amf_value * amf_value_t;
```

## Remarks ##
The state of an AMF value is dependent on the underlying AMF object. If the underlying AMF object is destroyed, then all AMF values which descend from the object will be invalidated.

Additionally, actual primitive values should be acquired through the AMF object getter functions.