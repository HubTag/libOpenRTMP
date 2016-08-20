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

#define AMF_SIZE_AMF_TYPE_UNDEFINED                 0
#define AMF_SIZE_AMF_TYPE_NULL                      0
#define AMF_SIZE_AMF_TYPE_BOOLEAN                   1
#define AMF_SIZE_AMF_TYPE_INTEGER                   4
#define AMF_SIZE_AMF_TYPE_INTEGER16                 2
#define AMF_SIZE_AMF_TYPE_INTEGER24                 3
#define AMF_SIZE_AMF_TYPE_DOUBLE                    8
#define AMF_SIZE_AMF_TYPE_UNSUPPORTED               0
#define AMF_SIZE_AMF_TYPE_STRING(AMF_M_STRLEN,AMF_M_BUF_LEN)   0

#define AMF_SIZE2_AMF_TYPE_UNDEFINED                AMF_SIZE_AMF_TYPE_UNDEFINED
#define AMF_SIZE2_AMF_TYPE_NULL                     AMF_SIZE_AMF_TYPE_NULL
#define AMF_SIZE2_AMF_TYPE_BOOLEAN                  AMF_SIZE_AMF_TYPE_BOOLEAN
#define AMF_SIZE2_AMF_TYPE_INTEGER                  AMF_SIZE_AMF_TYPE_INTEGER
#define AMF_SIZE2_AMF_TYPE_INTEGER16                AMF_SIZE_AMF_TYPE_INTEGER16
#define AMF_SIZE2_AMF_TYPE_INTEGER24                AMF_SIZE_AMF_TYPE_INTEGER24
#define AMF_SIZE2_AMF_TYPE_DOUBLE                   AMF_SIZE_AMF_TYPE_DOUBLE
#define AMF_SIZE2_AMF_TYPE_UNSUPPORTED              AMF_SIZE_AMF_TYPE_UNSUPPORTED
#define AMF_SIZE2_AMF_TYPE_STRING(AMF_M_STRLEN,AMF_M_BUF_LEN)  AMF_M_STRLEN


#define AMF_PASTE2(A,B) A ## B
#define AMF_PASTE(A,B) AMF_PASTE2(A,B)

#define AMF_SIZE_OF(A) AMF_PASTE(AMF_SIZE_, A)
#define AMF_SIZE_OF2(A) AMF_PASTE(AMF_SIZE2_, A)

#define AMF_SIZES2_0() 0
#define AMF_SIZES2_1(A, B) AMF_SIZE_OF2(A)
#define AMF_SIZES2_2(A, B, C, D) (AMF_SIZES2_1(A,B) + AMF_SIZES2_1(C,D))
#define AMF_SIZES2_3(A, B, C, D, E, F) (AMF_SIZES2_1(A,B) + AMF_SIZES2_2(C,D,E,F))
#define AMF_SIZES2_INTERNAL( x, A, B, C, D, E, F, FUNC, ... ) (FUNC)
#define AMF_SIZES2(...) AMF_SIZES_INTERNAL(  __VA_ARGS__,                \
                                            AMF_SIZES2_3( __VA_ARGS__ ), \
                                            AMF_SIZES2_3( __VA_ARGS__ ), \
                                            AMF_SIZES2_2( __VA_ARGS__ ), \
                                            AMF_SIZES2_2( __VA_ARGS__ ), \
                                            AMF_SIZES2_1( __VA_ARGS__ ), \
                                            AMF_SIZES2_1( __VA_ARGS__ ), \
                                            AMF_SIZES2_0(             ), \
                                            AMF_SIZES2_0(             ) )
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


#define AMF_WRITE_AMF_TYPE_UNDEFINED(AMF_M_DATA,AMF_M_OFFSET,AMF_M_VAL) do{\
}while(0)

#define AMF_WRITE_AMF_TYPE_NULL(AMF_M_DATA,AMF_M_OFFSET,AMF_M_VAL) do{\
}while(0)

#define AMF_WRITE_AMF_TYPE_BOOLEAN(AMF_M_DATA,AMF_M_OFFSET,AMF_M_VAL) do{\
    AMF_M_DATA[AMF_M_OFFSET]=AMF_M_VAL;\
    AMF_M_OFFSET += AMF_SIZE_AMF_TYPE_BOOLEAN; \
}while(0)

#define AMF_WRITE_AMF_TYPE_INTEGER(AMF_M_DATA,AMF_M_OFFSET,AMF_M_VAL) do{\
    ntoh_write_ud( AMF_M_DATA + AMF_M_OFFSET, AMF_M_VAL ); \
    AMF_M_OFFSET += AMF_SIZE_AMF_TYPE_INTEGER;\
}while(0)

#define AMF_WRITE_AMF_TYPE_INTEGER16(AMF_M_DATA,AMF_M_OFFSET,AMF_M_VAL) do{\
    ntoh_write_s( AMF_M_DATA + AMF_M_OFFSET, AMF_M_VAL ); \
    AMF_M_OFFSET += AMF_SIZE_AMF_TYPE_INTEGER16;\
}while(0)

#define AMF_WRITE_AMF_TYPE_INTEGER24(AMF_M_DATA,AMF_M_OFFSET,AMF_M_VAL) do{\
    ntoh_write_ud3( AMF_M_DATA + AMF_M_OFFSET, AMF_M_VAL ); \
    AMF_M_OFFSET += AMF_SIZE_AMF_TYPE_INTEGER24;\
}while(0)

#define AMF_WRITE_AMF_TYPE_DOUBLE(AMF_M_DATA,AMF_M_OFFSET,AMF_M_VAL) do{\
    byte AMF_M_FLIPPED[8];\
    write_double_ieee( AMF_M_FLIPPED, AMF_M_VAL );\
    ntoh_memcpy(AMF_M_DATA + AMF_M_OFFSET, AMF_M_FLIPPED, 8);\
    AMF_M_OFFSET+=AMF_SIZE_AMF_TYPE_DOUBLE;\
}while(0)

#define AMF_WRITE_AMF_TYPE_UNSUPPORTED(AMF_M_DATA,AMF_M_OFFSET,AMF_M_VAL) do{\
}while(0)

#define AMF_WRITE_AMF_TYPE_STRING2(AMF_M_DATA,AMF_M_OFFSET,AMF_M_VAL) do{\
    memcpy(AMF_M_DATA+AMF_M_OFFSET,AMF_M_VAL,MIN(BUF_LEN,AMF_LEN)); \
    AMF_M_OFFSET+=AMF_LEN;\
}while(0)

#define AMF_WRITE_AMF_TYPE_STRING(AMF_M_LEN,AMF_M_BUF_LEN) \
size_t AMF_LEN = AMF_M_LEN;\
size_t BUF_LEN = AMF_M_BUF_LEN;\
AMF_WRITE_AMF_TYPE_STRING2

#define AMF_WRITE_0(AMF_M_DATA) do{\
    AMF_OFFSET_VALUE += 0;\
}while(0)

#define AMF_WRITE_1(AMF_M_DATA,A,B) do{\
    AMF_PASTE(AMF_WRITE_,A)(AMF_M_DATA,AMF_OFFSET_VALUE,B);\
}while(0)

#define AMF_WRITE_2(AMF_M_DATA,A,B,C,D) do{\
    AMF_WRITE_1(AMF_M_DATA,A,B);\
    AMF_WRITE_1(AMF_M_DATA,C,D);\
}while(0)

#define AMF_WRITE_3(AMF_M_DATA,A,B,C,D,E,F) do{\
    AMF_WRITE_1(AMF_M_DATA,A,B);\
    AMF_WRITE_2(AMF_M_DATA,C,D,E,F);\
}while(0)

#define AMF_WRITE_INTERNAL(x, A, B, C, D, E, F, FUNC, ...) do{ \
    size_t AMF_OFFSET_VALUE = 0; \
    FUNC; \
}while(0)

#define AMF_WRITE(AMF_M_DATA,...) AMF_WRITE_INTERNAL(  __VA_ARGS__,                   \
                                            AMF_WRITE_3( (AMF_M_DATA),__VA_ARGS__ ),  \
                                            AMF_WRITE_3( (AMF_M_DATA),__VA_ARGS__ ),  \
                                            AMF_WRITE_2( (AMF_M_DATA),__VA_ARGS__ ),  \
                                            AMF_WRITE_2( (AMF_M_DATA),__VA_ARGS__ ),  \
                                            AMF_WRITE_1( (AMF_M_DATA),__VA_ARGS__ ),  \
                                            AMF_WRITE_1( (AMF_M_DATA),__VA_ARGS__ ),  \
                                            AMF_WRITE_0( (AMF_M_DATA)             ),  \
                                            AMF_WRITE_0( (AMF_M_DATA)             ) )

#define AMF0_DESCRIBE_ENCODE( AMF_M_DATA, AMF_M_DATA_LEN, AMF_M_TYPE, ... )  \
do{                                                 \
    bool AMF_M_OFFSET = AMF_M_TYPE == AMF0_TYPE_NONE ? 0 : 1;   \
    size_t AMF_M_LEN = AMF_SIZES2( __VA_ARGS__ ) + AMF_M_OFFSET;\
    if( AMF_M_DATA == nullptr ){                          \
        return AMF_SIZE(AMF_M_LEN);                       \
    }                                               \
    if( AMF_M_DATA_LEN < AMF_M_LEN ){                           \
        return AMF_ERR_INCOMPLETE;                  \
    }                                               \
    if( AMF_M_OFFSET > 0 ){                               \
        AMF_M_DATA[0] = AMF_M_TYPE;                             \
    }                                               \
    AMF_WRITE(AMF_M_DATA+AMF_M_OFFSET, __VA_ARGS__);            \
    return AMF_SIZE(AMF_M_LEN);                           \
} while(0)

#define AMF_READ_AMF_TYPE_UNDEFINED(AMF_M_DATA,AMF_M_DATA_LEN,AMF_M_OFFSET,AMF_M_VAL) do{\
}while(0)

#define AMF_READ_AMF_TYPE_NULL(AMF_M_DATA,AMF_M_DATA_LEN,AMF_M_OFFSET,AMF_M_VAL) do{\
}while(0)

#define AMF_READ_AMF_TYPE_BOOLEAN(AMF_M_DATA,AMF_M_DATA_LEN,AMF_M_OFFSET,AMF_M_VAL) do{\
    AMF_M_VAL = AMF_M_DATA[AMF_M_OFFSET];\
    AMF_M_OFFSET += AMF_SIZE_AMF_TYPE_BOOLEAN;\
}while(0)

#define AMF_READ_AMF_TYPE_INTEGER(AMF_M_DATA,AMF_M_DATA_LEN,AMF_M_OFFSET,AMF_M_VAL) do{\
    AMF_M_VAL = ntoh_read_ud( AMF_M_DATA + AMF_M_OFFSET ); \
    AMF_M_OFFSET += AMF_SIZE_AMF_TYPE_INTEGER;\
}while(0)

#define AMF_READ_AMF_TYPE_INTEGER16(AMF_M_DATA,AMF_M_DATA_LEN,AMF_M_OFFSET,AMF_M_VAL) do{\
    AMF_M_VAL = ntoh_read_s( AMF_M_DATA + AMF_M_OFFSET );\
    AMF_M_OFFSET += AMF_SIZE_AMF_TYPE_INTEGER16;\
}while(0)

#define AMF_READ_AMF_TYPE_INTEGER24(AMF_M_DATA,AMF_M_DATA_LEN,AMF_M_OFFSET,AMF_M_VAL) do{\
    AMF_M_VAL = ntoh_read_ud3( AMF_M_DATA + AMF_M_OFFSET );\
    AMF_M_OFFSET += AMF_SIZE_AMF_TYPE_INTEGER24;\
}while(0)

#define AMF_READ_AMF_TYPE_DOUBLE(AMF_M_DATA,AMF_M_DATA_LEN,AMF_M_OFFSET,AMF_M_VAL) do{\
    byte AMF_M_FLIPPED[8];\
    ntoh_memcpy(AMF_M_FLIPPED, AMF_M_DATA + AMF_M_OFFSET, AMF_SIZE_AMF_TYPE_DOUBLE);\
    AMF_M_VAL = read_double_ieee( AMF_M_FLIPPED );\
    AMF_M_OFFSET+=AMF_SIZE_AMF_TYPE_DOUBLE;\
}while(0)

#define AMF_READ_AMF_TYPE_UNSUPPORTED(AMF_M_DATA,AMF_M_DATA_LEN,AMF_M_OFFSET,AMF_M_VAL) do{\
}while(0)

#define AMF_READ_AMF_TYPE_STRING2(AMF_M_DATA,AMF_M_DATA_LEN,AMF_M_OFFSET,AMF_M_VAL) do{\
    if( AMF_M_DATA_LEN < AMF_M_LEN + AMF_LEN ){ \
        return AMF_ERR_INCOMPLETE; \
    } \
    memcpy(AMF_M_VAL,AMF_M_DATA+AMF_M_OFFSET,MIN(AMF_LEN, BUF_LEN)); \
    AMF_M_OFFSET+=AMF_LEN;\
    AMF_M_LEN += AMF_LEN; \
}while(0)

#define AMF_READ_AMF_TYPE_STRING(AMF_M_LEN,AMF_M_BUF_LEN) \
size_t AMF_LEN = AMF_M_LEN;\
size_t BUF_LEN = AMF_M_BUF_LEN;\
AMF_READ_AMF_TYPE_STRING2

#define AMF_READ_0(AMF_M_DATA,AMF_M_DATA_LEN) do{\
    AMF_OFFSET_VALUE += 0;\
}while(0)

#define AMF_READ_1(AMF_M_DATA,AMF_M_DATA_LEN,A,B) do{\
    AMF_PASTE(AMF_READ_,A)(AMF_M_DATA,AMF_M_DATA_LEN,AMF_OFFSET_VALUE,B);\
}while(0)

#define AMF_READ_2(AMF_M_DATA,AMF_M_DATA_LEN,A,B,C,D) do{\
    AMF_READ_1(AMF_M_DATA,AMF_M_DATA_LEN,A,B);\
    AMF_READ_1(AMF_M_DATA,AMF_M_DATA_LEN,C,D);\
}while(0)

#define AMF_READ_3(AMF_M_DATA,AMF_M_DATA_LEN,A,B,C,D,E,F) do{\
    AMF_READ_1(AMF_M_DATA,AMF_M_DATA_LEN,A,B);\
    AMF_READ_2(AMF_M_DATA,AMF_M_DATA_LEN,C,D,E,F);\
}while(0)

#define AMF_READ_INTERNAL(x, A, B, C, D, E, F, FUNC, ...) do{ \
    size_t AMF_OFFSET_VALUE = 0; \
    FUNC; \
}while(0)

#define AMF_READ(AMF_M_DATA,AMF_M_DATA_LEN,...) AMF_READ_INTERNAL(  __VA_ARGS__,                    \
                                            AMF_READ_3( (AMF_M_DATA),(AMF_M_DATA_LEN),__VA_ARGS__ ),  \
                                            AMF_READ_3( (AMF_M_DATA),(AMF_M_DATA_LEN),__VA_ARGS__ ),  \
                                            AMF_READ_2( (AMF_M_DATA),(AMF_M_DATA_LEN),__VA_ARGS__ ),  \
                                            AMF_READ_2( (AMF_M_DATA),(AMF_M_DATA_LEN),__VA_ARGS__ ),  \
                                            AMF_READ_1( (AMF_M_DATA),(AMF_M_DATA_LEN),__VA_ARGS__ ),  \
                                            AMF_READ_1( (AMF_M_DATA),(AMF_M_DATA_LEN),__VA_ARGS__ ),  \
                                            AMF_READ_0( (AMF_M_DATA),(AMF_M_DATA_LEN)             ),  \
                                            AMF_READ_0( (AMF_M_DATA),(AMF_M_DATA_LEN)             ) )


#define AMF0_DESCRIBE_DECODE_SIZE( AMF_M_TYPE, ... ) (AMF_SIZES( __VA_ARGS__ ) + (AMF_M_TYPE == AMF0_TYPE_NONE ? 0 : 1))

#define AMF0_DESCRIBE_DECODE_I( AMF_M_COUNT, AMF_M_DATA, AMF_M_DATA_LEN, AMF_M_TYPE, ... )  \
do{                                                 \
    bool AMF_M_OFFSET = AMF_M_TYPE == AMF0_TYPE_NONE ? 0 : 1;   \
    size_t AMF_M_LEN = (AMF_SIZES( __VA_ARGS__ ) + AMF_M_OFFSET); \
    byte AMF_M_BUFFER[50];\
    if( AMF_M_DATA_LEN < AMF_M_LEN ){ \
        return AMF_ERR_INCOMPLETE; \
    } \
    memcpy(AMF_M_BUFFER, AMF_M_DATA, AMF_M_LEN); \
    if( AMF_M_TYPE != AMF0_TYPE_NONE && *AMF_M_BUFFER != AMF_M_TYPE ){ \
        return AMF_ERR_INVALID_DATA; \
    }\
    AMF_READ(AMF_M_DATA+AMF_M_OFFSET,AMF_M_DATA_LEN, __VA_ARGS__);            \
    AMF_M_COUNT=AMF_M_LEN;\
} while(0)

#define AMF0_DESCRIBE_DECODE( AMF_M_DATA, AMF_M_DATA_LEN, AMF_M_TYPE, ... )   \
do{                        \
    size_t AMF_M_COUNT = 0;                            \
    AMF0_DESCRIBE_DECODE_I(AMF_M_COUNT, AMF_M_DATA, AMF_M_DATA_LEN, AMF_M_TYPE, __VA_ARGS__); \
    return AMF_SIZE(AMF_M_COUNT);                           \
} while(0)

#define AMF0_DESCRIBE_DECODE_PEEK( AMF_M_DATA, AMF_M_DATA_LEN, AMF_M_TYPE, ... ) do{      \
    size_t AMF_M_COUNT = 0;                                                   \
    AMF0_DESCRIBE_DECODE_I(AMF_M_COUNT, AMF_M_DATA, AMF_M_DATA_LEN, AMF_M_TYPE, __VA_ARGS__);   \
    AMF_M_COUNT = 0;\
    return AMF_SIZE(AMF_M_COUNT);                                                 \
} while(0)



#endif
