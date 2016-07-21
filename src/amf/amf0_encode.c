/*
    amf0_encode.c

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memutil.h"
#include "amf/amf.h"
#include "data_stream.h"

//Returns an IEEE 754 float from the data
amf_err_t amf0_write_number( byte* data, size_t data_len, double value ){
    if( data == nullptr ){
        return 9;
    }
    if( data_len < 9 ){
        return AMF_ERR_INCOMPLETE;
    }
    data[0] = AMF0_TYPE_NUMBER;
    byte flipped[8];
    write_double_ieee( flipped, value );
    ntoh_memcpy(data + 1, flipped, 8);
    return 9;
}

amf_err_t amf0_write_boolean( byte* data, size_t data_len, int value ){
    if( data == nullptr ){
        return 2;
    }
    data[0] = AMF0_TYPE_BOOLEAN;
    data[1] = value & 255;
    return 2;
}

//String functions are used for all strings.
static amf_err_t amf0_write_string_internal( byte* data, size_t data_len, byte type, void *value, size_t value_len ){
    if( data == nullptr ){
        if( value_len > 0xFFFF || type == AMF0_TYPE_XML_DOCUMENT ){
            return 5 + value_len;
        }
        return 3 + value_len;
    }
    if( data_len < 3 ){
        return AMF_ERR_INCOMPLETE;
    }
    int len = 3;
    if( value_len > 0xFFFF || type == AMF0_TYPE_XML_DOCUMENT ){
        if( data_len < 5 ){
            return AMF_ERR_INCOMPLETE;
        }
        data[0] = type == AMF0_TYPE_XML_DOCUMENT ? AMF0_TYPE_XML_DOCUMENT : AMF0_TYPE_LONG_STRING;
        ntoh_write_ud( data + 1, value_len );
        len = 5;
    }
    else{
        data[0] = AMF0_TYPE_STRING;
        ntoh_write_us( data + 1, value_len );
    }
    if( value_len + len > data_len ){
        return AMF_ERR_INCOMPLETE;
    }
    value_len = value_len + len <= data_len ? value_len : data_len - len;

    memcpy( data + len, value, value_len );

    return value_len + len;
}

//String functions are used for normal and long strings.
amf_err_t amf0_write_string( byte* data, size_t data_len, void *value, size_t value_len ){
    return amf0_write_string_internal( data, data_len, AMF0_TYPE_STRING, value, value_len );
}

//Emit an object start marker
amf_err_t amf0_write_object( byte* data, size_t data_len ){
    if( data == nullptr ){
        return 1;
    }
    if( data_len < 1 ){
        return AMF_ERR_INCOMPLETE;
    }
    data[0] = AMF0_TYPE_OBJECT;
    return 1;
}

//If inside an object, use this to obtain a copy of the property name
amf_err_t amf0_write_prop_name( byte* data, size_t data_len, void *value, size_t value_len ){
    if( data == nullptr ){
        return 2 + value_len;
    }
    if( data_len < 2 ){
        return AMF_ERR_INCOMPLETE;
    }
    ntoh_write_us( data, value_len );

    value_len = value_len + 2 <= data_len ? value_len : data_len - 2;

    memcpy( data + 2, value, value_len );
    return 2 + value_len;
}

//Dummy; do not use.
amf_err_t amf0_write_movieclip( byte* data, size_t data_len ){
    emit_err("[Erroneous Data] Trying to write movieclip to AMF!");
    return AMF_ERR_INVALID_DATA;
}

//Basically a dummy; used to verify that the next item is indeed a null value.
amf_err_t amf0_write_null( byte* data, size_t data_len ){
    if( data == nullptr ){
        return 1;
    }
    if( data_len < 1 ){
        return AMF_ERR_INCOMPLETE;
    }
    data[0] = AMF0_TYPE_NULL;
    return 1;
}


//Basically a dummy; used to verify that the next item is indeed an undefined value.
amf_err_t amf0_write_undefined( byte* data, size_t data_len ){
    if( data == nullptr ){
        return 1;
    }
    if( data_len < 1 ){
        return AMF_ERR_INCOMPLETE;
    }
    data[0] = AMF0_TYPE_UNDEFINED;
    return 1;
}

amf_err_t amf0_write_reference( byte* data, size_t data_len, uint32_t value){
    if( data == nullptr ){
        return 3;
    }
    if( data_len < 3 ){
        return AMF_ERR_INCOMPLETE;
    }
    data[0] = AMF0_TYPE_REFERENCE;
    ntoh_write_us( data + 1, value );
    return 3;
}

//Unimplemented. Will implement if necessary.
amf_err_t amf0_write_ecma_array( byte* data, size_t data_len ){
    emit_err("[Unimplemented] Trying to write ECMA Array to AMF!");
    return 0;
}

//Mostly a dummy; this is used to verify and skip an object end marker
amf_err_t amf0_write_object_end( byte* data, size_t data_len ){
    if( data == nullptr ){
        return 1;
    }
    if( data_len < 1 ){
        return AMF_ERR_INCOMPLETE;
    }
    data[0] = AMF0_TYPE_OBJECT_END;
    return 1;
}

//Unimplemented. Will implement if necessary.
amf_err_t amf0_write_strict_array( byte* data, size_t data_len ){
    emit_err("[Unimplemented] Trying to write Strict Array to AMF!");
    return 0;
}

//Returns a timezone offset as well as a double essentially representing a Unix timestamp
//(Resolution is 1:1 with seconds, epoch is 1970 Jan 1 00:00:00.000)
amf_err_t amf0_write_date( byte* data, size_t data_len, int timezone, double timestamp ){
    if( data == nullptr ){
        return 11;
    }
    if( data_len < 11 ){
        return AMF_ERR_INCOMPLETE;
    }
    data[0] = AMF0_TYPE_DATE;
    ntoh_write_s( data + 1, timezone );
    byte reversed[8];
    write_double_ieee( reversed, timestamp );
    ntoh_memcpy( data + 3, reversed, 8 );
    return 11;
}

//Dummy
amf_err_t amf0_write_unsupported( byte* data, size_t data_len ){
    emit_err("[Error] Trying to write an unsupported type to AMF!");
    return 0;
}

//Alias around amf0_write_string_internal
amf_err_t amf0_write_long_string( byte* data, size_t data_len, void *value, size_t value_len){
    return amf0_write_string_internal( data, data_len, AMF0_TYPE_LONG_STRING, value, value_len );
}

//Alias around amf0_write_string_internal
amf_err_t amf0_write_xmldocument( byte* data, size_t data_len, void *value, size_t value_len){
    return amf0_write_string_internal( data, data_len, AMF0_TYPE_XML_DOCUMENT, value, value_len );
}

//Unimplemented. Will implement if necessary.
amf_err_t amf0_write_recordset( byte* data, size_t data_len ){
    emit_err("[Unimplemented] Trying to write Record Set to AMF!");
    return 0;
}
//Unimplemented. Will implement if necessary.
amf_err_t amf0_write_typed_object( byte* data, size_t data_len ){
    emit_err("[Unimplemented] Trying to write Typed Object to AMF!");
    return 0;
}

amf_err_t amf0_write_continue( byte* data, size_t data_len, void *value, size_t value_len ){
    value_len = value_len <= data_len ? value_len : data_len;
    memcpy( data, value, value_len );
    return value_len;
}
