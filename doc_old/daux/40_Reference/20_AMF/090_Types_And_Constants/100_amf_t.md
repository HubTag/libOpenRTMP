title: amf_t
--------------------------

Represents a generic AMF abstraction. 

## Syntax ##

```c
#include<openrtmp/amf/amf_object.h>

typedef struct amf_object * amf_t;
```

## Remarks ##
It currently only implements AMF0 internally.

When pushing values to an AMF object, there is a notion of push slots or push position. This is merely the logical name for the area where the next pushed value will reside. This is always the deepest nested object or array which has not yet been closed.

If there are no complex values in an AMF object, a pushed value just gets inserted at the end of the AMF object. However, if the last item in the AMF object is a complex value which hasn't been closed, then the item is added to the end of that complex value, unless the end of that complex value is in and of itself a complex value, in which case the algorithm will recurse until it finds a location which follows a complete value, and inserts the pushed value there.

Some rough illustrations:

If there is no complex value at the end of our object:

```
String: "Foo"
Number: 123
// Next insert will be here.
```

If the last value is an open object:

```
String: "Foo"
Number: 123
Object: {
String:     memb: "bar"
            // Next insert will be here.
```

If the last value is an open object with a nested open object at the end:


```
String: "Foo"
Number: 123
Object: {
String:     memb: "bar"
Object:     {
String:         thing: "A thing"
                // Next insert will be here
```

If the last value is an open object with a nested closed object at the end:


```
String: "Foo"
Number: 123
Object: {
String:     memb: "bar"
Object:     {
String:         thing: "A thing"
            }
            // Next Insert will be here.
```


If the last value is a closed object:


```
String: "Foo"
Number: 123
Object: {
String:     memb: "bar"
Object:     {
String:         thing: "A thing"
            }
String:     another: "So much data"
        }
// Next Insert will be here.
```

