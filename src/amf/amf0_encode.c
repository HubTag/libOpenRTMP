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
#include "ieee754_double.h"

//Returns an IEEE 754 float from the data
amf_err_t amf0_write_number( byte* data, size_t data_len, double value ){
    AMF0_DESCRIBE_ENCODE( data, data_len, AMF0_TYPE_NUMBER, AMF_TYPE_DOUBLE, value );
}

amf_err_t amf0_write_boolean( byte* data, size_t data_len, int value ){
    AMF0_DESCRIBE_ENCODE( data, data_len, AMF0_TYPE_BOOLEAN, AMF_TYPE_BOOLEAN, value&255 );
}

//String functions are used for normal and long strings.
amf_err_t amf0_write_string( byte* data, size_t data_len, const void *value, size_t value_len ){
    if( value_len > 0xFFFF ){
        AMF0_DESCRIBE_ENCODE( data, data_len, AMF0_TYPE_LONG_STRING, AMF_TYPE_LONG_STRING(value_len), value );
    }
    else{
        AMF0_DESCRIBE_ENCODE( data, data_len, AMF0_TYPE_STRING, AMF_TYPE_STRING(value_len), value );
    }
}

//Emit an object start marker
amf_err_t amf0_write_object( byte* data, size_t data_len ){
    AMF0_DESCRIBE_ENCODE( data, data_len, AMF0_TYPE_OBJECT );
}

//If inside an object, use this to obtain a copy of the property name
amf_err_t amf0_write_prop_name( byte* data, size_t data_len, const void *value, size_t value_len ){
    AMF0_DESCRIBE_ENCODE( data, data_len, AMF0_TYPE_NONE, AMF_TYPE_STRING(value_len), value );
}

//Dummy; do not use.
amf_err_t amf0_write_movieclip( byte* data, size_t data_len ){
    emit_err("[Erroneous Data] Trying to write movieclip to AMF!");
    return AMF_ERR_INVALID_DATA;
}

//Basically a dummy; used to verify that the next item is indeed a null value.
amf_err_t amf0_write_null( byte* data, size_t data_len ){
    AMF0_DESCRIBE_ENCODE( data, data_len, AMF0_TYPE_NULL );
}


//Basically a dummy; used to verify that the next item is indeed an undefined value.
amf_err_t amf0_write_undefined( byte* data, size_t data_len ){
    AMF0_DESCRIBE_ENCODE( data, data_len, AMF0_TYPE_UNDEFINED );
}

amf_err_t amf0_write_reference( byte* data, size_t data_len, uint32_t value){
    AMF0_DESCRIBE_ENCODE( data, data_len, AMF0_TYPE_REFERENCE, AMF_TYPE_INTEGER16, value );
}

//Unimplemented. Will implement if necessary.
amf_err_t amf0_write_ecma_array( byte* data, size_t data_len, uint32_t array_len ){
    AMF0_DESCRIBE_ENCODE( data, data_len, AMF0_TYPE_ECMA_ARRAY, AMF_TYPE_INTEGER, array_len );
}

//Mostly a dummy; this is used to verify and skip an object end marker
amf_err_t amf0_write_object_end( byte* data, size_t data_len ){
    AMF0_DESCRIBE_ENCODE( data, data_len, AMF0_TYPE_OBJECT_END );
}

//Unimplemented. Will implement if necessary.
amf_err_t amf0_write_strict_array( byte* data, size_t data_len ){
    emit_err("[Unimplemented] Trying to write Strict Array to AMF!");
    return 0;
}

//Returns a timezone offset as well as a double essentially representing a Unix timestamp
//(Resolution is 1:1 with seconds, epoch is 1970 Jan 1 00:00:00.000)
amf_err_t amf0_write_date( byte* data, size_t data_len, int timezone, double timestamp ){
    AMF0_DESCRIBE_ENCODE( data, data_len, AMF0_TYPE_DATE, AMF_TYPE_INTEGER16, timezone, AMF_TYPE_DOUBLE, timestamp );
}

//Dummy
amf_err_t amf0_write_unsupported( byte* data, size_t data_len ){
    emit_err("[Error] Trying to write an unsupported type to AMF!");
    return 0;
}

//Alias around amf0_write_string_internal
amf_err_t amf0_write_long_string( byte* data, size_t data_len, const void *value, size_t value_len){
    return amf0_write_string( data, data_len, value, value_len );
}

//Alias around amf0_write_string_internal
amf_err_t amf0_write_xmldocument( byte* data, size_t data_len, const void *value, size_t value_len){
    AMF0_DESCRIBE_ENCODE( data, data_len, AMF0_TYPE_XML_DOCUMENT, AMF_TYPE_LONG_STRING(value_len), value );
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

