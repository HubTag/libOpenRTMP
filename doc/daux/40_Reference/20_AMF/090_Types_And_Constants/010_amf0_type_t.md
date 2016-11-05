title: amf0_type_t
--------------------------

Refers to AMF0 types using their numeric values as defined in the AMF0 specification.

## Syntax ##

```c
#include<openrtmp/amf/amf_constants.h>

typedef enum AMF0_TYPE {
	AMF0_TYPE_NUMBER,           //Page 5, §2.2
	AMF0_TYPE_BOOLEAN,          //Page 5, §2.3
	AMF0_TYPE_STRING,           //Page 5, §2.4
	AMF0_TYPE_OBJECT,           //Page 5, §2.5
	AMF0_TYPE_MOVIECLIP,        //Page 5, §2.6
	AMF0_TYPE_NULL,             //Page 6, §2.7
	AMF0_TYPE_UNDEFINED,        //Page 6, §2.8
	AMF0_TYPE_REFERENCE,        //Page 6, §2.9
	AMF0_TYPE_ECMA_ARRAY,       //Page 6, §2.10
	AMF0_TYPE_OBJECT_END,       //Page 6, §2.11
	AMF0_TYPE_STRICT_ARRAY,     //Page 7, §2.12
	AMF0_TYPE_DATE,             //Page 7, §2.13
	AMF0_TYPE_LONG_STRING,      //Page 7, §2.14
	AMF0_TYPE_UNSUPPORTED,      //Page 7, §2.15
	AMF0_TYPE_RECORDSET,        //Page 7, §2.16
	AMF0_TYPE_XML_DOCUMENT,     //Page 7, §2.17
	AMF0_TYPE_TYPED_OBJECT,     //Page 8, §2.18
	AMF0_TYPE_AVMPLUS,          //Page 8, §3.1
	AMF0_TYPE_NONE,
} amf0_type_t;
```

## Members ##
#### AMF0_TYPE_NUMBER ####
> Indicates an AMF0 number.
> 
> Defined as `0` in §2.2 on page 5 of the AMF0 spec.

#### AMF0_TYPE_BOOLEAN ####
> Indicates an AMF0 boolean.
> 
> Defined as `1` in §2.3 on page 5 of the AMF0 spec.

#### AMF0_TYPE_STRING ####
> Indicates an AMF0 string.
> 
> Defined as `2` in §2.4 on page 5 of the AMF0 spec.

#### AMF0_TYPE_OBJECT ####
> Indicates the beginning of an AMF0 object.
> 
> Defined as `3` in §2.5 on page 5 of the AMF0 spec.

#### AMF0_TYPE_MOVIECLIP ####
> Indicates an AMF0 movie clip.
> 
> Defined as `4` in §2.6 on page 5 of the AMF0 spec.

#### AMF0_TYPE_NULL ####
> Indicates an AMF0 null value.
> 
> Defined as `5` in §2.7 on page 6 of the AMF0 spec.

#### AMF0_TYPE_UNDEFINED ####
> Indicates an AMF0 undefined value.
> 
> Defined as `6` in §2.8 on page 6 of the AMF0 spec.

#### AMF0_TYPE_REFERENCE ####
> Indicates an AMF0 reference.
> 
> Defined as `7` in §2.9 on page 6 of the AMF0 spec.

#### AMF0_TYPE_ECMA_ARRAY ####
> Indicates an AMF0 ECMA array, which has both associative and ordinal components.
> 
> Defined as `8` in §2.10 on page 6 of the AMF0 spec.

#### AMF0_TYPE_OBJECT_END ####
> Indicates the end of an AMF0 object.
> 
> Defined as `9` in §2.11 on page 6 of the AMF0 spec.

#### AMF0_TYPE_STRICT_ARRAY ####
> Indicates an AMF0 strict array, which contains only ordinal indices.
> 
> Defined as `10` in §2.12 on page 7 of the AMF0 spec.

#### AMF0_TYPE_DATE ####
> Indicates an AMF0 date.
> 
> Defined as `11` in §2.13 on page 7 of the AMF0 spec.

#### AMF0_TYPE_LONG_STRING ####
> Indicates an AMF0 long string.
> 
> Defined as `12` in §2.14 on page 7 of the AMF0 spec.

#### AMF0_TYPE_UNSUPPORTED ####
> Indicates an unsupported type.
> 
> Defined as `13` in §2.15 on page 7 of the AMF0 spec.

#### AMF0_TYPE_RECORDSET ####
> Indicates an AMF0 record set.
> 
> Defined as `14` in §2.16 on page 7 of the AMF0 spec.

#### AMF0_TYPE_XML_DOCUMENT ####
> Indicates an AMF0 XML document, which is essentially a long string.
> 
> Defined as `15` in §2.17 on page 7 of the AMF0 spec.

#### AMF0_TYPE_TYPED_OBJECT ####
> Indicates an AMF0 alias name.
> 
> Defined as `16` in §2.18 on page 8 of the AMF0 spec.

#### AMF0_TYPE_AVMPLUS ####
> Indicates that the following object is actually formatted in the AMF3 format.
> 
> Defined as `17` in §3.1 on page 8 of the AMF0 spec.

#### AMF0_TYPE_NONE ####
> Used internally.
> 

## Remarks ##
These are mainly used internally by and to interface with the AMF0 codec.
