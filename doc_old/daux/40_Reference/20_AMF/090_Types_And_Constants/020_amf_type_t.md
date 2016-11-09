title: amf_type_t
--------------------------

Refers to various AMF types.

## Syntax ##

```c
#include<openrtmp/amf/amf_constants.h>

typedef enum AMF_TYPE {
	AMF_TYPE_UNDEFINED,
	AMF_TYPE_NULL,
	AMF_TYPE_BOOLEAN,
	AMF_TYPE_INTEGER,
	AMF_TYPE_DOUBLE,
	AMF_TYPE_DATE,
	AMF_TYPE_STRING,
	AMF_TYPE_XML_DOCUMENT,
	AMF_TYPE_OBJECT,
	AMF_TYPE_TYPED_OBJECT,
	AMF_TYPE_OBJECT_END,
	AMF_TYPE_ARRAY,
	AMF_TYPE_VECTOR_INT,
	AMF_TYPE_VECTOR_UINT,
	AMF_TYPE_VECTOR_DOUBLE,
	AMF_TYPE_VECTOR_OBJECT,
	AMF_TYPE_BYTE_ARRAY,
	AMF_TYPE_MOVIECLIP,
	AMF_TYPE_RECORDSET,
	AMF_TYPE_REFERENCE,
	AMF_TYPE_UNSUPPORTED,
	AMF_TYPE_AVMPLUS,
	AMF_TYPE_NONE,
	AMF_TYPE_COMPLEX,
	AMF_TYPE_ECMA_ARRAY_ASSOC_END,
	AMF_TYPE_INTEGER24,
	AMF_TYPE_INTEGER16,
	AMF_TYPE_ASSOCIATIVE,
} amf_type_t;
```

## Members ##

#### AMF_TYPE_UNDEFINED ####
> Refers to an undefined type.
> 

#### AMF_TYPE_NULL ####
> Refers to a null value.
> 

#### AMF_TYPE_BOOLEAN ####
> Refers to a boolean value.
> 

#### AMF_TYPE_INTEGER ####
> Refers to an integer value.
> 

#### AMF_TYPE_DOUBLE ####
> Refers to a double precision floating point number.
> 

#### AMF_TYPE_DATE ####
> Refers to a date.
> 

#### AMF_TYPE_STRING ####
> Refers to a string.
> 

#### AMF_TYPE_XML_DOCUMENT ####
> Refers to an XML document, which is basically a string.
> 

#### AMF_TYPE_OBJECT ####
> Refers to an AMF object value.
> 

#### AMF_TYPE_TYPED_OBJECT ####
> Refers to an object alias.
> 

#### AMF_TYPE_OBJECT_END ####
> Refers to the end of an object.
> 

#### AMF_TYPE_ARRAY ####
> Refers to an ECMA array.
> 

#### AMF_TYPE_VECTOR_INT ####
> Refers to an array of integers.
> 

#### AMF_TYPE_VECTOR_UINT ####
> Refers to an array of unsigned integers.
> 

#### AMF_TYPE_VECTOR_DOUBLE ####
> Refers to an array of double precision floating point numbers.
> 

#### AMF_TYPE_VECTOR_OBJECT ####
> Refers to an array of AMF objects.
> 

#### AMF_TYPE_BYTE_ARRAY ####
> Refers to an array of bytes.
> 

#### AMF_TYPE_MOVIECLIP ####
> Refers to a movie clip.
> 

#### AMF_TYPE_RECORDSET ####
> Refers to a record set.
> 

#### AMF_TYPE_REFERENCE ####
> Refers to a reference type.
> 

#### AMF_TYPE_UNSUPPORTED ####
> Refers to an unsupported type.
> 

#### AMF_TYPE_AVMPLUS ####
> Indicates an AMF version upgrade.
> 

#### AMF_TYPE_NONE ####
> Refers to a non-existing type.
> 
> Mainly used internally, and to indicate the end of an AMF object.

#### AMF_TYPE_ECMA_ARRAY_ASSOC_END ####
> Refers to the end of an ECMA array.
> 
> Only used when building AMF objects with `amf_push_simple`.

#### AMF_TYPE_INTEGER24 ####
> Refers to a 24 bit signed integer.
> 
> Only used when building AMF objects with `amf_push_simple`.

#### AMF_TYPE_INTEGER16 ####
> Refers to a 16 bit signed integer.
> 
> Only used when building AMF objects with `amf_push_simple`.

#### AMF_TYPE_ASSOCIATIVE ####
> Refers to a type with associative values.
> 
> When used to check type equivalence, this will match any of the following types:
> *  AMF_TYPE_OBJECT
> *  AMF_TYPE_ARRAY
> *  AMF_TYPE_TYPED_OBJECT

#### AMF_TYPE_COMPLEX ####
> Refers to a complex type.
> 
> When used to check type equivalence, this will match any of the following types:
> *  AMF_TYPE_OBJECT
> *  AMF_TYPE_ARRAY
> *  AMF_TYPE_RECORDSET
> *  AMF_TYPE_TYPED_OBJECT
> *  AMF_TYPE_VECTOR_DOUBLE
> *  AMF_TYPE_VECTOR_INT
> *  AMF_TYPE_VECTOR_OBJECT
> *  AMF_TYPE_VECTOR_UINT
> *  AMF_TYPE_BYTE_ARRAY
> *  AMF_TYPE_MOVIECLIP


## Remarks ##
These types are only meant to be used with libOpenRTMP. There is no overlap between the underlying numeric values of these constants and the AMF specifications. They are merely a relatively version agnostic way of representing AMF types for use in the AMF object abstraction.
