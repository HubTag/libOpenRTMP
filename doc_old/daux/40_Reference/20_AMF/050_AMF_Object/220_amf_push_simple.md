title: amf_push_simple
--------------------------

Pushes an aggregate of values into an AMF object in a single call.

## Syntax ##

```c
#include<openrtmp/amf/amf_object.h>

amf_err_t amf_push_simple( 
	amf_t   amf, 
	... 
);

amf_err_t amf_push_simple_list( 
	amf_t     amf, 
	va_list   list 
);

// Helper Macros
// Syntax isn't actual C, but is there to clarify usage.
#define AMF_NULL    ( [key]        )
#define AMF_DBL     ( [key,] value )
#define AMF_INT     ( [key,] value ) 
#define AMF_STR     ( [key,] value )  
#define AMF_BOOL    ( [key,] value )  
#define AMF_OBJ     ( [key,] ...   )  
#define AMF_ARR     ( [key,] ...   )  
#define AMF_ARR_ORD (        ...   )
#define AMF         (        ...   )
```

## Parameters ##
#### amf ####
> The AMF object into which the values will be pushed.

#### ... / list ####
> A list of value types and values to push.
> 

## Return Value ##
An AMF error code. 

## Remarks ##
These functions are described in much more detail in (Creating AMF objects with amf_push_simple)[/AMF/Creating_AMF_with_push_simple.html].

For more information about push locations, see `amf_t`.

## Example ##

This example will create an AMF object and push a number of values using `amf_push_simple` along with the helper macros.

```c
amf_t obj = amf_create( 0 );
amf_push_simple( obj, 
	AMF(
		AMF_STR("Hello"),
		AMF_NULL(),
		AMF_INT(123),
		AMF_OBJ(
			AMF_STR("first", "This is the first item"),
			AMF_DBL("second", 2.0),
			AMF_OBJ("Nested",
				AMF_STR("This", "is deep again!")
			),
			AMF_NULL("third"),
			AMF_BOOL("fourth", true)
		),
		AMF_ARR(
			AMF_INT("another", 100),
			AMF_INT("and_another", 200),
			AMF_ARR_ORD(
				AMF_INT("1", 300),
				AMF_STR("2", "Wow!"),
				AMF_NULL("3")
			)
		)
	)
);
```

When printed, this yields:

```
String: Hello
Null
Integer: 123
Object: {
    first: String: This is the first item
    second: Double: 2.000000
    Nested: Object: {
        This: String: is deep again!
    }
    third: Null
    fourth: Boolean: true
}
Array: [
    another: Integer: 100
    and_another: Integer: 200
    End Assoc.
    1: Integer: 300
    2: String: Wow!
    3: Null
]
```
