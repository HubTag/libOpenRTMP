/*
    amf_shorthand.h

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

*/

#ifndef RTMP_H_AMF_SHORTHAND_H
#define RTMP_H_AMF_SHORTHAND_H

#include "rtmp/rtmp_types.h"

#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif

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



#endif
