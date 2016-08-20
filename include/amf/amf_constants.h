/*
    amf_constants.h

    Copyright (C) 2016 Hubtag LLC.

    ----------------------------------------

    This file is part of libOpenRTMP.

    libOpenRTMP is free software: you can redistribute it and/or modify
    it under the terms of version 3 of the GNU Affero General Public License
    as published by the Free Software Foundation.

    libOpenRTMP is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with libOpenRTMP. If not, see <http://www.gnu.org/licenses/>.


    ----------------------------------------

    All references in this file are relevant to the official AMF0 and AMF3 specifications.

    AMF0:
        Download: http://wwwimages.adobe.com/content/dam/Adobe/en/devnet/amf/pdf/amf0-file-format-specification.pdf
        Information Page: http://www.adobe.com/devnet/swf.html
        CRC32: f74e4446
        Date: June 29th, 2016

    AMF3:
        Download: http://wwwimages.adobe.com/content/dam/Adobe/en/devnet/amf/pdf/amf-file-format-spec.pdf
        Information Page: http://www.adobe.com/devnet/swf.html
        CRC32: 8e55cb19
        Date: June 29th, 2016

*/

#ifndef RTMP_H_AMF_CONSTANTS_H
#define RTMP_H_AMF_CONSTANTS_H

#include "rtmp/rtmp_types.h"

#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif

#define AMF_MAX_KEY 1000

typedef enum AMF_ERR {
    AMF_ERR_NONE,
    AMF_ERR_INVALID_DATA = -10,
    AMF_ERR_INCOMPLETE,
    AMF_ERR_NEED_NAME,
    AMF_ERR_OOM,
    AMF_ERR_BAD_ALLOC
} amf_err_t;

#define AMF_SIZE(n) ((amf_err_t)(n))

//AMF0 Type Markers             See AMF0 Specification
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
    AMF0_TYPE_NUMBER_INT,       //Not a part of the spec, for internal use
    AMF0_TYPE_NONE,
    AMF0_TYPE_COMPLEX,
    AMF0_TYPE_ECMA_ARRAY_ASSOC_END
} amf0_type_t;

//AMF3 Type Markers             See AMF3 Specification
typedef enum AMF3_TYPE {
    AMF3_TYPE_UNDEFINED,        //Page 6, §3.2
    AMF3_TYPE_NULL,             //Page 6, §3.3
    AMF3_TYPE_FALSE,            //Page 7, §3.4
    AMF3_TYPE_TRUE,             //Page 7, §3.5
    AMF3_TYPE_INTEGER,          //Page 7, §3.6
    AMF3_TYPE_DOUBLE,           //Page 7, §3.7
    AMF3_TYPE_STRING,           //Page 7, §3.8
    AMF3_TYPE_XML_DOCUMENT,     //Page 8, §3.9
    AMF3_TYPE_DATE,             //Page 8, §3.10
    AMF3_TYPE_ARRAY,            //Page 9, §3.11
    AMF3_TYPE_OBJECT,           //Page 9, §3.12
    AMF3_TYPE_XML,              //Page 11, §3.13
    AMF3_TYPE_BYTE_ARRAY,       //Page 11, §3.14
    AMF3_TYPE_VECTOR_INT,       //Page 12, §3.15
    AMF3_TYPE_VECTOR_UINT,      //Page 12, §3.15
    AMF3_TYPE_VECTOR_DOUBLE,    //Page 12, §3.15
    AMF3_TYPE_VECTOR_OBJECT,    //Page 12, §3.15
    AMF3_TYPE_DICTIONARY        //Page 13, §3.16
} amf3_type_t;

//AMF Object Type Markers
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
    AMF_TYPE_LONG_STRING,
} amf_type_t;


#define AMF_SIZE_AMF_TYPE_UNDEFINED 0
#define AMF_SIZE_AMF_TYPE_NULL 0
#define AMF_SIZE_AMF_TYPE_BOOLEAN 1
#define AMF_SIZE_AMF_TYPE_INTEGER 4
#define AMF_SIZE_AMF_TYPE_INTEGER16 2
#define AMF_SIZE_AMF_TYPE_INTEGER24 3
#define AMF_SIZE_AMF_TYPE_DOUBLE 8
#define AMF_SIZE_AMF_TYPE_UNSUPPORTED 0
#define AMF_SIZE_AMF_TYPE_STRING(amf_len) (2+amf_len)
#define AMF_SIZE_AMF_TYPE_LONG_STRING(amf_len) (4+amf_len)


#define AMF_PASTE2(A,B) A ## B
#define AMF_PASTE(A,B) AMF_PASTE2(A,B)

#define AMF_SIZE_OF(A) AMF_PASTE(AMF_SIZE_, A)

#define AMF_SIZES_0() 0
#define AMF_SIZES_1(A, B) AMF_SIZE_OF(A)
#define AMF_SIZES_2(A, B, C, D) (AMF_SIZES_1(A,B) + AMF_SIZES_1(C,D))
#define AMF_SIZES_3(A, B, C, D, E, F) (AMF_SIZES_1(A,B) + AMF_SIZES_2(C,D,E,F))
#define AMF_SIZES_INTERNAL( x, A, B, C, D, E, F, FUNC, ... ) (FUNC)
#define AMF_SIZES(...) AMF_SIZES_INTERNAL(  __VA_ARGS__,                \
                                            AMF_SIZES_3( __VA_ARGS__ ), \
                                            AMF_SIZES_3( __VA_ARGS__ ), \
                                            AMF_SIZES_2( __VA_ARGS__ ), \
                                            AMF_SIZES_2( __VA_ARGS__ ), \
                                            AMF_SIZES_1( __VA_ARGS__ ), \
                                            AMF_SIZES_1( __VA_ARGS__ ), \
                                            AMF_SIZES_0(             ), \
                                            AMF_SIZES_0(             ) )


#define AMF_WRITE_AMF_TYPE_UNDEFINED(data,offset,val) do{\
}while(0)

#define AMF_WRITE_AMF_TYPE_NULL(data,offset,val) do{\
}while(0)

#define AMF_WRITE_AMF_TYPE_BOOLEAN(data,offset,val) do{\
    data[offset]=val;\
    offset += AMF_SIZE_AMF_TYPE_BOOLEAN; \
}while(0)

#define AMF_WRITE_AMF_TYPE_INTEGER(data,offset,val) do{\
    ntoh_write_ud( data + offset, val ); \
    offset += AMF_SIZE_AMF_TYPE_INTEGER;\
}while(0)

#define AMF_WRITE_AMF_TYPE_INTEGER16(data,offset,val) do{\
    ntoh_write_s( data + offset, val ); \
    offset += AMF_SIZE_AMF_TYPE_INTEGER16;\
}while(0)

#define AMF_WRITE_AMF_TYPE_INTEGER24(data,offset,val) do{\
    ntoh_write_ud3( data + offset, val ); \
    offset += AMF_SIZE_AMF_TYPE_INTEGER24;\
}while(0)

#define AMF_WRITE_AMF_TYPE_DOUBLE(data,offset,val) do{\
    byte flipped[8];\
    write_double_ieee( flipped, val );\
    ntoh_memcpy(data + offset, flipped, 8);\
    offset+=AMF_SIZE_AMF_TYPE_DOUBLE;\
}while(0)

#define AMF_WRITE_AMF_TYPE_UNSUPPORTED(data,offset,val) do{\
}while(0)

#define AMF_WRITE_AMF_TYPE_STRING2(data,offset,val) do{\
    ntoh_write_us( data + offset, AMF_LEN ); \
    offset += AMF_SIZE_AMF_TYPE_INTEGER16;\
    memcpy(data+offset,val,AMF_LEN); \
    offset+=AMF_LEN;\
}while(0)

#define AMF_WRITE_AMF_TYPE_STRING(len) size_t AMF_LEN = len; AMF_WRITE_AMF_TYPE_STRING2

#define AMF_WRITE_AMF_TYPE_LONG_STRING2(data,offset,val) do{\
    ntoh_write_ud( data + offset, AMF_LEN ); \
    offset += AMF_SIZE_AMF_TYPE_INTEGER;\
    memcpy(data+offset,val,AMF_LEN); \
    offset+=AMF_LEN;\
}while(0)

#define AMF_WRITE_AMF_TYPE_LONG_STRING(len) size_t AMF_LEN = len; AMF_WRITE_AMF_TYPE_LONG_STRING2

#define AMF_WRITE_0(data) do{\
    AMF_OFFSET_VALUE += 0;\
}while(0)

#define AMF_WRITE_1(data,A,B) do{\
    AMF_PASTE(AMF_WRITE_,A)(data,AMF_OFFSET_VALUE,B);\
}while(0)

#define AMF_WRITE_2(data,A,B,C,D) do{\
    AMF_WRITE_1(data,A,B);\
    AMF_WRITE_1(data,C,D);\
}while(0)

#define AMF_WRITE_3(data,A,B,C,D,E,F) do{\
    AMF_WRITE_1(data,A,B);\
    AMF_WRITE_2(data,C,D,E,F);\
}while(0)

#define AMF_WRITE_INTERNAL(x, A, B, C, D, E, F, FUNC, ...) do{ \
    size_t AMF_OFFSET_VALUE = 0; \
    FUNC; \
}while(0)

#define AMF_WRITE(data,...) AMF_WRITE_INTERNAL(  __VA_ARGS__,                   \
                                            AMF_WRITE_3( (data),__VA_ARGS__ ),  \
                                            AMF_WRITE_3( (data),__VA_ARGS__ ),  \
                                            AMF_WRITE_2( (data),__VA_ARGS__ ),  \
                                            AMF_WRITE_2( (data),__VA_ARGS__ ),  \
                                            AMF_WRITE_1( (data),__VA_ARGS__ ),  \
                                            AMF_WRITE_1( (data),__VA_ARGS__ ),  \
                                            AMF_WRITE_0( (data)             ),  \
                                            AMF_WRITE_0( (data)             ) )

#define AMF0_DESCRIBE_ENCODE( data, data_len, type, ... )  \
do{                                                 \
    bool offset = type == AMF0_TYPE_NONE ? 0 : 1;   \
    size_t len = AMF_SIZES( __VA_ARGS__ ) + offset; \
    if( data == nullptr ){                          \
        return AMF_SIZE(len);                       \
    }                                               \
    if( data_len < len ){                           \
        return AMF_ERR_INCOMPLETE;                  \
    }                                               \
    if( offset > 0 ){                               \
        data[0] = type;                             \
    }                                               \
    AMF_WRITE(data+offset, __VA_ARGS__);            \
    return AMF_SIZE(len);                           \
} while(0)

#define AMF_READ_AMF_TYPE_UNDEFINED(data,offset,val) do{\
}while(0)

#define AMF_READ_AMF_TYPE_NULL(data,offset,val) do{\
}while(0)

#define AMF_READ_AMF_TYPE_BOOLEAN(data,offset,val) do{\
    val = data[offset];\
    offset += AMF_SIZE_AMF_TYPE_BOOLEAN;\
}while(0)

#define AMF_READ_AMF_TYPE_INTEGER(data,offset,val) do{\
    val = ntoh_read_ud( data + offset ); \
    offset += AMF_SIZE_AMF_TYPE_INTEGER;\
}while(0)

#define AMF_READ_AMF_TYPE_INTEGER16(data,offset,val) do{\
    val = ntoh_read_s( data + offset );\
    offset += AMF_SIZE_AMF_TYPE_INTEGER16;\
}while(0)

#define AMF_READ_AMF_TYPE_INTEGER24(data,offset,val) do{\
    val = ntoh_read_ud3( data + offset );\
    offset += AMF_SIZE_AMF_TYPE_INTEGER24;\
}while(0)

#define AMF_READ_AMF_TYPE_DOUBLE(data,offset,val) do{\
    byte flipped[8];\
    ntoh_memcpy(flipped, data + offset, AMF_SIZE_AMF_TYPE_DOUBLE);\
    val = read_double_ieee( flipped );\
    offset+=AMF_SIZE_AMF_TYPE_DOUBLE;\
}while(0)

#define AMF_READ_AMF_TYPE_UNSUPPORTED(data,offset,val) do{\
}while(0)
#include <stdio.h>
#define AMF_READ_AMF_TYPE_STRING2(data,offset,val) do{\
    size_t len = ntoh_read_us( data + offset ); \
    offset += AMF_SIZE_AMF_TYPE_INTEGER16;\
    memcpy(val,data+offset,MIN(len,AMF_BUFLEN)); \
    offset+=len;\
}while(0)

#define AMF_READ_AMF_TYPE_STRING(buflen) size_t AMF_BUFLEN = buflen; AMF_READ_AMF_TYPE_STRING2

#define AMF_READ_AMF_TYPE_LONG_STRING2(data,offset,val) do{\
    size_t len = ntoh_read_ud( data + offset ); \
    offset += AMF_SIZE_AMF_TYPE_INTEGER;\
    memcpy(val,data+offset,MIN(len,AMF_BUFLEN)); \
    offset+=len;\
}while(0)

#define AMF_READ_AMF_TYPE_LONG_STRING(buflen) size_t AMF_BUFLEN = buflen; AMF_READ_AMF_TYPE_LONG_STRING2

#define AMF_READ_0(data) do{\
    AMF_OFFSET_VALUE += 0;\
}while(0)

#define AMF_READ_1(data,A,B) do{\
    AMF_PASTE(AMF_READ_,A)(data,AMF_OFFSET_VALUE,B);\
}while(0)

#define AMF_READ_2(data,A,B,C,D) do{\
    AMF_READ_1(data,A,B);\
    AMF_READ_1(data,C,D);\
}while(0)

#define AMF_READ_3(data,A,B,C,D,E,F) do{\
    AMF_READ_1(data,A,B);\
    AMF_READ_2(data,C,D,E,F);\
}while(0)

#define AMF_READ_INTERNAL(x, A, B, C, D, E, F, FUNC, ...) do{ \
    size_t AMF_OFFSET_VALUE = 0; \
    FUNC; \
}while(0)

#define AMF_READ(data,...) AMF_READ_INTERNAL(  __VA_ARGS__,                    \
                                            AMF_READ_3( (data),__VA_ARGS__ ),  \
                                            AMF_READ_3( (data),__VA_ARGS__ ),  \
                                            AMF_READ_2( (data),__VA_ARGS__ ),  \
                                            AMF_READ_2( (data),__VA_ARGS__ ),  \
                                            AMF_READ_1( (data),__VA_ARGS__ ),  \
                                            AMF_READ_1( (data),__VA_ARGS__ ),  \
                                            AMF_READ_0( (data)             ),  \
                                            AMF_READ_0( (data)             ) )


#define AMF0_DESCRIBE_DECODE_SIZE( type, ... ) (AMF_SIZES( __VA_ARGS__ ) + (type == AMF0_TYPE_NONE ? 0 : 1))

#define AMF0_DESCRIBE_DECODE_I( count, data, data_len, type, ... )  \
do{                                                 \
    bool offset = type == AMF0_TYPE_NONE ? 0 : 1;   \
    size_t len = AMF0_DESCRIBE_DECODE_SIZE(type, __VA_ARGS__); \
    byte buffer[AMF0_DESCRIBE_DECODE_SIZE(type, __VA_ARGS__)];\
    if( data_len < len ){ \
        return AMF_ERR_INCOMPLETE; \
    } \
    memcpy(buffer, data, len); \
    count=len;\
    if( type != AMF0_TYPE_NONE && *buffer != type ){ \
        return AMF_ERR_INVALID_DATA; \
    }\
    AMF_READ(data+offset, __VA_ARGS__);            \
} while(0)

#define AMF0_DESCRIBE_DECODE( data, data_len, type, ... )   \
do{                        \
    size_t count = 0;                            \
    AMF0_DESCRIBE_DECODE_I(count, data, data_len, type, __VA_ARGS__); \
    return AMF_SIZE(count);                           \
} while(0)

#define AMF0_DESCRIBE_DECODE_PEEK( data, data_len, type, ... ) do{      \
    size_t count = 0;                                                   \
    AMF0_DESCRIBE_DECODE_I(count, data, data_len, type, __VA_ARGS__);   \
    return AMF_SIZE(0);                                                 \
} while(0)



#define AMF_ARG_AMF_TYPE_UNDEFINED(name)
#define AMF_ARG_AMF_TYPE_NULL(name)
#define AMF_ARG_AMF_TYPE_BOOLEAN(name) , int name
#define AMF_ARG_AMF_TYPE_INTEGER(name) , int name
#define AMF_ARG_AMF_TYPE_INTEGER16(name) , short name
#define AMF_ARG_AMF_TYPE_INTEGER24(name) , int name
#define AMF_ARG_AMF_TYPE_DOUBLE(name) , double name
#define AMF_ARG_AMF_TYPE_UNSUPPORTED(name)
#define AMF_ARG_AMF_TYPE_STRING(name) , void* name, size_t AMF_PASTE(name, _len)
#define AMF_ARG_AMF_TYPE_LONG_STRING(name) , void* name, size_t AMF_PASTE(name, _len)

#define AMF_DUMMY_AMF_TYPE_UNDEFINED(name) int name = 0
#define AMF_DUMMY_AMF_TYPE_NULL(name) int name = 0
#define AMF_DUMMY_AMF_TYPE_BOOLEAN(name)
#define AMF_DUMMY_AMF_TYPE_INTEGER(name)
#define AMF_DUMMY_AMF_TYPE_INTEGER16(name)
#define AMF_DUMMY_AMF_TYPE_INTEGER24(name)
#define AMF_DUMMY_AMF_TYPE_DOUBLE(name)
#define AMF_DUMMY_AMF_TYPE_UNSUPPORTED(name) int name = 0
#define AMF_DUMMY_AMF_TYPE_STRING(name)
#define AMF_DUMMY_AMF_TYPE_LONG_STRING(name)

#define AMF_PASS_AMF_TYPE_UNDEFINED(name) AMF_TYPE_UNDEFINED, name
#define AMF_PASS_AMF_TYPE_NULL(name) AMF_TYPE_NULL, name
#define AMF_PASS_AMF_TYPE_BOOLEAN(name) AMF_TYPE_BOOLEAN, name
#define AMF_PASS_AMF_TYPE_INTEGER(name) AMF_TYPE_INTEGER, name
#define AMF_PASS_AMF_TYPE_INTEGER16(name) AMF_TYPE_INTEGER16, name
#define AMF_PASS_AMF_TYPE_INTEGER24(name) AMF_TYPE_INTEGER24, name
#define AMF_PASS_AMF_TYPE_DOUBLE(name) AMF_TYPE_DOUBLE, name
#define AMF_PASS_AMF_TYPE_UNSUPPORTED(name) AMF_TYPE_UNSUPPORTED, name
#define AMF_PASS_AMF_TYPE_STRING(name) AMF_TYPE_STRING(name_len), name
#define AMF_PASS_AMF_TYPE_LONG_STRING(name) AMF_TYPE_LONG_STRING(name_len), name

#define AMF_ARGP_AMF_TYPE_UNDEFINED(name)
#define AMF_ARGP_AMF_TYPE_NULL(name)
#define AMF_ARGP_AMF_TYPE_BOOLEAN(name) , int *name
#define AMF_ARGP_AMF_TYPE_INTEGER(name) , int *name
#define AMF_ARGP_AMF_TYPE_INTEGER16(name) , short *name
#define AMF_ARGP_AMF_TYPE_INTEGER24(name) , int *name
#define AMF_ARGP_AMF_TYPE_DOUBLE(name) , double *name
#define AMF_ARGP_AMF_TYPE_UNSUPPORTED(name)
#define AMF_ARGP_AMF_TYPE_STRING(name) , void* name, size_t AMF_PASTE(name, _len)
#define AMF_ARGP_AMF_TYPE_LONG_STRING(name) , void* name, size_t AMF_PASTE(name, _len)

#define AMF_PASSP_AMF_TYPE_UNDEFINED(name) AMF_TYPE_UNDEFINED, name
#define AMF_PASSP_AMF_TYPE_NULL(name) AMF_TYPE_NULL, name
#define AMF_PASSP_AMF_TYPE_BOOLEAN(name) AMF_TYPE_BOOLEAN, *name
#define AMF_PASSP_AMF_TYPE_INTEGER(name) AMF_TYPE_INTEGER, *name
#define AMF_PASSP_AMF_TYPE_INTEGER16(name) AMF_TYPE_INTEGER16, *name
#define AMF_PASSP_AMF_TYPE_INTEGER24(name) AMF_TYPE_INTEGER24, name
#define AMF_PASSP_AMF_TYPE_DOUBLE(name) AMF_TYPE_DOUBLE, name
#define AMF_PASSP_AMF_TYPE_UNSUPPORTED(name) AMF_TYPE_UNSUPPORTED, name
#define AMF_PASSP_AMF_TYPE_STRING(name) AMF_TYPE_STRING(name_len), name
#define AMF_PASSP_AMF_TYPE_LONG_STRING(name) AMF_TYPE_LONG_STRING(name_len), name

#define AMF_ARG_0( data, data_len, type ) ) { \
    AMF0_DESCRIBE_ENCODE( data, data_len, type );\
}

#define AMF_ARG_1(data, data_len, type, A) \
AMF_PASTE(AMF_ARG_, A)(argA) ) {\
    AMF_PASTE(AMF_DUMMY_,A)(argA);    \
    AMF0_DESCRIBE_ENCODE( data, data_len, type, \
                            AMF_PASTE(AMF_PASS_,A)(argA));\
}

#define AMF_ARG_2(data, data_len, type, A,B) \
AMF_PASTE(AMF_ARG_, A)(argA),    \
AMF_PASTE(AMF_ARG_, B)(argB) ) {\
    AMF_PASTE(AMF_DUMMY_,A)(argA);    \
    AMF_PASTE(AMF_DUMMY_,B)(argB);    \
    AMF0_DESCRIBE_ENCODE( data, data_len, type, \
                            AMF_PASTE(AMF_PASS_,A)(argA),\
                            AMF_PASTE(AMF_PASS_,B)(argB));\
}

#define AMF_ARG_3(data, data_len, type, A,B,C) \
AMF_PASTE(AMF_ARG_, A)(argA),    \
AMF_PASTE(AMF_ARG_, B)(argB),    \
AMF_PASTE(AMF_ARG_, C)(argC) ) {\
    AMF_PASTE(AMF_DUMMY_,A)(argA);    \
    AMF_PASTE(AMF_DUMMY_,B)(argB);    \
    AMF_PASTE(AMF_DUMMY_,C)(argC);    \
    AMF0_DESCRIBE_ENCODE( data, data_len, type, \
                            AMF_PASTE(AMF_PASS_,A)(argA),\
                            AMF_PASTE(AMF_PASS_,B)(argB),\
                            AMF_PASTE(AMF_PASS_,C)(argC));\
}

#define AMF_ARG_INTERNAL(A, B, C, FUNC, ...) FUNC

#define AMF_ARG(data, data_len, type,...) AMF_ARG_INTERNAL(  __VA_ARGS__,              \
                                            AMF_ARG_3( data, data_len, type, __VA_ARGS__ ),  \
                                            AMF_ARG_2( data, data_len, type, __VA_ARGS__ ),  \
                                            AMF_ARG_1( data, data_len, type, __VA_ARGS__ ),  \
                                            AMF_ARG_0( data, data_len, type              ) )
#define AMF0_DESCRIBE( name, type, ... )\
amf_err_t AMF_PASTE( amf0_write_, name ) (byte * data, size_t data_len AMF_ARG(data, data_len, type, __VA_ARGS__)


#endif
