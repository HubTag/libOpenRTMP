/*
    amf0_decode.c

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

#include <stdlib.h>
#include <string.h>
#include "memutil.h"
#include "amf/amf.h"
#include "amf/amf_shorthand.h"
#include "ieee754_double.h"



//Return the type of the next item in the message
amf0_type_t amf0_next_type( const byte* data, size_t data_len ){
    if( data_len < 1 ){
        return AMF0_TYPE_UNSUPPORTED;
    }
    if( data[0] <= AMF0_TYPE_AVMPLUS ){
        return data[0];
    }
    return AMF0_TYPE_UNSUPPORTED;
}

//Returns an IEEE 754 float from the data
amf_err_t amf0_get_number( const byte* data, size_t data_len, double *value ){
    AMF0_DESCRIBE_DECODE( data, data_len, AMF0_TYPE_NUMBER, AMF_TYPE_DOUBLE, *value );
}

amf_err_t amf0_get_boolean( const byte* data, size_t data_len, int *value ){
    AMF0_DESCRIBE_DECODE( data, data_len, AMF0_TYPE_BOOLEAN, AMF_TYPE_BOOLEAN, *value );
}

//String functions are used for normal and long strings, as well as XML documents
//Used to verify how much storage to allocate for the upcoming string.
amf_err_t amf0_get_string_length( const byte* data, size_t data_len, size_t *value ){
    AMF0_DESCRIBE_DECODE( data, data_len, AMF0_TYPE_STRING, AMF_TYPE_INTEGER16, *value );
}

//String functions are used for normal and long strings, as well as XML documents
amf_err_t amf0_get_string( const byte* data, size_t data_len, void *value, size_t value_len ){
    AMF0_DESCRIBE_DECODE( data, data_len, AMF0_TYPE_STRING, AMF_TYPE_STRING(value_len), value );
}

//Mostly a dummy; this is used to verify and skip an object start marker
amf_err_t amf0_get_object( const byte* data, size_t data_len ){
    AMF0_DESCRIBE_DECODE( data, data_len, AMF0_TYPE_OBJECT );
}

//If inside an object, use this to obtain the length of a property name
amf_err_t amf0_get_prop_length( const byte* data, size_t data_len, size_t *value ){
    AMF0_DESCRIBE_DECODE_PEEK( data, data_len, AMF0_TYPE_NONE, AMF_TYPE_INTEGER16, *value );
}

//If inside an object, use this to obtain a copy of the property name
amf_err_t amf0_get_prop_name( const byte* data, size_t data_len, void *value, size_t value_len ){
    AMF0_DESCRIBE_DECODE( data, data_len, AMF0_TYPE_NONE, AMF_TYPE_STRING(value_len), value );
}

//Dummy; do not use.
amf_err_t amf0_get_movieclip( const byte* data, size_t data_len ){
    emit_err("[Erroneous Data] Trying to read movieclip from AMF!");
    return AMF_ERR_INVALID_DATA;
}

//Basically a dummy; used to verify that the next item is indeed a null value.
amf_err_t amf0_get_null( const byte* data, size_t data_len ){
    AMF0_DESCRIBE_DECODE( data, data_len, AMF0_TYPE_NULL );
}


//Basically a dummy; used to verify that the next item is indeed an undefined value.
amf_err_t amf0_get_undefined( const byte* data, size_t data_len ){
    AMF0_DESCRIBE_DECODE( data, data_len, AMF0_TYPE_UNDEFINED );
}

amf_err_t amf0_get_reference( const byte* data, size_t data_len, uint32_t *value){
    AMF0_DESCRIBE_DECODE( data, data_len, AMF0_TYPE_REFERENCE, AMF_TYPE_INTEGER16, *value );
}

amf_err_t amf0_get_ecma_array( const byte* data, size_t data_len, uint32_t *num_memb ){
    AMF0_DESCRIBE_DECODE( data, data_len, AMF0_TYPE_ECMA_ARRAY, AMF_TYPE_INTEGER, *num_memb );
}

//Mostly a dummy; this is used to verify and skip an object end marker
amf_err_t amf0_get_object_end( const byte* data, size_t data_len ){
    AMF0_DESCRIBE_DECODE( data, data_len, AMF0_TYPE_OBJECT_END );
}

//Unimplemented. Will implement if necessary.
amf_err_t amf0_get_strict_array( const byte* data, size_t data_len ){
    emit_err("[Unimplemented] Trying to read Strict Array from AMF!");
    return 1;
}

//Returns a timezone offset as well as a double essentially representing a Unix timestamp
//(Resolution is 1:1 with seconds, epoch is 1970 Jan 1 00:00:00.000)
amf_err_t amf0_get_date( const byte* data, size_t data_len, int* timezone, double* timestamp ){
    AMF0_DESCRIBE_DECODE( data, data_len, AMF0_TYPE_DATE, AMF_TYPE_INTEGER16, *timezone, AMF_TYPE_DOUBLE, *timestamp );
}

amf_err_t amf0_get_unsupported( const byte* data, size_t data_len ){
    AMF0_DESCRIBE_DECODE( data, data_len, AMF0_TYPE_UNSUPPORTED );
}

amf_err_t amf0_get_long_string_length( const byte* data, size_t data_len, size_t *value){
    AMF0_DESCRIBE_DECODE( data, data_len, AMF0_TYPE_LONG_STRING, AMF_TYPE_INTEGER, *value );
}

amf_err_t amf0_get_long_string( const byte* data, size_t data_len, void *value, size_t value_len){
    AMF0_DESCRIBE_DECODE( data, data_len, AMF0_TYPE_LONG_STRING, AMF_TYPE_LONG_STRING(value_len), value );
}

amf_err_t amf0_get_xmldocument_length( const byte* data, size_t data_len, size_t *value){
    AMF0_DESCRIBE_DECODE( data, data_len, AMF0_TYPE_XML_DOCUMENT, AMF_TYPE_INTEGER, *value );
}

amf_err_t amf0_get_xmldocument( const byte* data, size_t data_len, void *value, size_t value_len){
    AMF0_DESCRIBE_DECODE( data, data_len, AMF0_TYPE_XML_DOCUMENT, AMF_TYPE_LONG_STRING(value_len), value );
}

//Unimplemented. Will implement if necessary.
amf_err_t amf0_get_recordset( const byte* data, size_t data_len ){
    emit_err("[Unimplemented] Trying to read Record Set from AMF!");
    return 1;
}
//Unimplemented. Will implement if necessary.
amf_err_t amf0_get_typed_object( const byte* data, size_t data_len ){
    emit_err("[Unimplemented] Trying to read Typed Object from AMF!");
    return 1;
}

void amf0_print( const byte* data, size_t data_len, rtmp_printer_t printer ){
    double num;
    int integer;
    uint32_t uinteger;
    char str[1000];
    int object_layer = 0;
    size_t r = 0;
    size_t len = 0;

    while( r < data_len ){
        for( int i = 0; i < object_layer; ++i ){
            printer->s("    ");
        }
        if( object_layer > 0 ){
            len = amf0_get_prop_name( data + r, data_len - r, str, 1000);
            r += len;
            printer->s("\"");
            printer->s2(str, len - 2);
            printer->s("\": ");
        }
        byte type = amf0_next_type( data + r, data_len - r );
        if( type == AMF0_TYPE_UNSUPPORTED){
            break;
        }
        switch( type ){
            case AMF0_TYPE_NUMBER:
                r += amf0_get_number( data + r, data_len - r, &num);
                printer->s("Number: ");
                printer->f( num );
                break;
            case AMF0_TYPE_BOOLEAN:
                r += amf0_get_boolean( data + r, data_len - r, &integer);
                printer->s("Boolean: ");
                printer->d( integer );
                break;
            case AMF0_TYPE_STRING:
                amf0_get_string_length( data + r, data_len - r, &len );
                r += amf0_get_string( data + r, data_len - r, str, 1000 );
                printer->s( "String: " );
                printer->s2(str, len);
                break;
            case AMF0_TYPE_OBJECT:
                r += amf0_get_object( data + r, data_len - r );
                printer->s("New Object");
                object_layer++;
                break;
            case AMF0_TYPE_MOVIECLIP:
                r += amf0_get_movieclip( data + r, data_len - r );
                break;
            case AMF0_TYPE_NULL:
                r += amf0_get_null( data + r, data_len - r);
                printer->s("Null");
                break;
            case AMF0_TYPE_UNDEFINED:
                r += amf0_get_undefined( data + r, data_len - r );
                printer->s("Undefined");
                break;
            case AMF0_TYPE_REFERENCE:
                r += amf0_get_reference( data + r, data_len - r, &uinteger);
                printer->s( "Reference to " );
                printer->u( uinteger );
                break;
            case AMF0_TYPE_ECMA_ARRAY:
                r += amf0_get_ecma_array( data + r, data_len - r, &uinteger);
                printer->s("ECMA Array, assoc len ");
                printer->u(uinteger);
                break;
            case AMF0_TYPE_OBJECT_END:
                r += amf0_get_object_end( data + r, data_len - r );
                printer->s("End Object");
                -- object_layer;
                break;
            case AMF0_TYPE_STRICT_ARRAY:
                r += amf0_get_strict_array( data + r, data_len - r );
                printer->s("Strict Array");
                break;
            case AMF0_TYPE_DATE:
                r += amf0_get_date( data + r, data_len - r, &integer, &num);
                printer->s( "Date: " );
                printer->f( num );
                printer->s( " +" );
                printer->d( integer );
                break;
            case AMF0_TYPE_LONG_STRING:
                amf0_get_long_string_length( data + r, data_len - r, &len );
                r += amf0_get_string( data + r, data_len - r, str, 1000 );
                printer->s("Long String: ");
                printer->s2(str, len);
                break;
            case AMF0_TYPE_UNSUPPORTED:
                r += amf0_get_unsupported( data + r, data_len - r );
                printer->s("Unsupported");
                break;
            case AMF0_TYPE_RECORDSET:
                r += amf0_get_recordset( data + r, data_len - r );
                printer->s("Record Set");
                break;
            case AMF0_TYPE_XML_DOCUMENT:
                amf0_get_xmldocument_length( data + r, data_len - r, &len );
                r += amf0_get_string( data + r, data_len - r, str, 1000 );
                printer->s("XML Document: ");
                printer->s2(str, len);
                break;
            case AMF0_TYPE_TYPED_OBJECT:
                r += amf0_get_typed_object( data + r, data_len - r );
                printer->s("Typed Object");
                break;

        }
        printer->s("\n");
    }
}
