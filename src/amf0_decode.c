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
#include "amf.h"
#include "data_stream.h"





#define AMF0_HARVEST_LENGTH(in, len) byte buffer[len]; if( ors_data_read( in, buffer, len ) < len ){ return AMF_ERR_INCOMPLETE; }
#define AMF0_PEEK_LENGTH(in, len) byte buffer[len]; if( ors_data_peek( in, buffer, len ) < len ){ return AMF_ERR_INCOMPLETE; }

//Provide safety macros that may be disabled if the user promises to be really really nice

#ifndef AMF0_CONFIG_UNSAFE
#   define AMF0_CHECK_TYPE(buffer, type, ret) if( *buffer != type ){ return ret; }
#else
#   define AMF0_CHECK_TYPE(buffer, type, ret)
#endif // AMF0_CONFIG_UNSAFE


//Return the type of the next item in the message
amf0_type_t amf0_next_type( ors_data_t* source ){
    byte type;
    if( ors_data_peek( source, &type, 1 ) < 1 ){
        return AMF0_TYPE_UNSUPPORTED;
    }
    if( type <= AMF0_TYPE_AVMPLUS ){
        return type;
    }
    return AMF0_TYPE_UNSUPPORTED;
}

//Returns an IEEE 754 float from the data
amf_err_t amf0_get_number( ors_data_t* source, double *value ){
    AMF0_HARVEST_LENGTH(source, 9);
    AMF0_CHECK_TYPE( buffer, AMF0_TYPE_NUMBER, AMF_ERR_INVALID_DATA );
    ntoh_memcpy( value, buffer + 1, 8 );
    return 9;
}

amf_err_t amf0_get_boolean( ors_data_t* source, int *value ){
    AMF0_HARVEST_LENGTH(source, 2);
    AMF0_CHECK_TYPE( buffer, AMF0_TYPE_BOOLEAN, AMF_ERR_INVALID_DATA );
    *value = buffer[1];
    return 2;
}

//String functions are used for normal and long strings, as well as XML documents
//Used to verify how much storage to allocate for the upcoming string.
amf_err_t amf0_get_string_length( ors_data_t* source, size_t *value ){
    byte buffer[5];
    int len = ors_data_peek( source, buffer, 5 );
    if( len < 3 ){
        return AMF_ERR_INCOMPLETE;
    }
    if( *buffer == AMF0_TYPE_STRING ){
        *value = ntoh_read_us(buffer + 1);
        return 3;
    }
    if( len < 5 ){
        return AMF_ERR_INCOMPLETE;
    }
    if( *buffer == AMF0_TYPE_LONG_STRING || *buffer == AMF0_TYPE_XML_DOCUMENT ){
        *value = ntoh_read_ud(buffer + 1);
        return 5;
    }
    return AMF_ERR_INVALID_DATA;
}

//String functions are used for normal and long strings, as well as XML documents
amf_err_t amf0_get_string( ors_data_t* source, void *value, size_t value_len ){
    if( value_len > 0 ){
        value_len--;
    }
    size_t len;
    byte scrap[5];
    int offset = amf0_get_string_length(source, &len);
    if( offset < 0 ){
        return offset;
    }
    value_len = len > value_len ? value_len : len;
    ors_data_read( source, scrap, offset );
    ors_data_read( source, value, value_len );
    ((char*)value)[value_len] = 0;
    return offset + len;
}

//Mostly a dummy; this is used to verify and skip an object start marker
amf_err_t amf0_get_object( ors_data_t* source){
    AMF0_HARVEST_LENGTH(source, 1);
    AMF0_CHECK_TYPE( buffer, AMF0_TYPE_OBJECT, AMF_ERR_INVALID_DATA );
    return 1;
}

//If inside an object, use this to obtain the length of a property name
amf_err_t amf0_get_prop_length( ors_data_t* source, size_t *value ){
    AMF0_PEEK_LENGTH(source, 2);
    *value = ntoh_read_us(buffer);
    return 0;
}

//If inside an object, use this to obtain a copy of the property name
amf_err_t amf0_get_prop_name( ors_data_t* source, void *value, int value_len ){
    AMF0_HARVEST_LENGTH(source, 2);
    size_t len = ntoh_read_us(buffer);
    value_len = len > value_len ? value_len : len;
    ors_data_read( source, value, value_len );
    ((char*)value)[value_len] = 0;
    return 2 + len;
}

//Dummy; do not use.
amf_err_t amf0_get_movieclip( ors_data_t* source ){
    emit_err("[Erroneous Data] Trying to read movieclip from AMF!");
    return AMF_ERR_INVALID_DATA;
}

//Basically a dummy; used to verify that the next item is indeed a null value.
amf_err_t amf0_get_null( ors_data_t* source ){
    AMF0_HARVEST_LENGTH(source, 1);
    AMF0_CHECK_TYPE( buffer, AMF0_TYPE_NULL, AMF_ERR_INVALID_DATA );
    return 1;
}


//Basically a dummy; used to verify that the next item is indeed an undefined value.
amf_err_t amf0_get_undefined( ors_data_t* source ){
    AMF0_HARVEST_LENGTH(source, 1);
    AMF0_CHECK_TYPE( buffer, AMF0_TYPE_UNDEFINED, AMF_ERR_INVALID_DATA );
    return 1;
}

amf_err_t amf0_get_reference( ors_data_t* source, uint32_t *value){
    AMF0_HARVEST_LENGTH(source, 3);
    AMF0_CHECK_TYPE( buffer, AMF0_TYPE_REFERENCE, AMF_ERR_INVALID_DATA );
    *value = ntoh_read_us(buffer);
    return 3;
}

//Unimplemented. Will implement if necessary.
amf_err_t amf0_get_ecma_array( ors_data_t* source ){
    emit_err("[Unimplemented] Trying to read ECMA Array from AMF!");
    return 0;
}

//Mostly a dummy; this is used to verify and skip an object end marker
amf_err_t amf0_get_object_end( ors_data_t* source ){
    AMF0_HARVEST_LENGTH(source, 1);
    AMF0_CHECK_TYPE( buffer, AMF0_TYPE_OBJECT_END, AMF_ERR_INVALID_DATA );
    return 1;
}

//Unimplemented. Will implement if necessary.
amf_err_t amf0_get_strict_array( ors_data_t* source ){
    emit_err("[Unimplemented] Trying to read Strict Array from AMF!");
    return 0;
}

//Returns a timezone offset as well as a double essentially representing a Unix timestamp
//(Resolution is 1:1 with seconds, epoch is 1970 Jan 1 00:00:00.000)
amf_err_t amf0_get_date( ors_data_t* source, int* timezone, double* timestamp ){
    AMF0_HARVEST_LENGTH(source, 11);
    AMF0_CHECK_TYPE( buffer, AMF0_TYPE_DATE, AMF_ERR_INVALID_DATA );
    *timezone = ntoh_read_s(buffer+1);
    ntoh_memcpy(timestamp, buffer + 3, 8);
    return 11;
}

//Dummy
amf_err_t amf0_get_unsupported( ors_data_t* source ){
    emit_err("[Error] Trying to read an unsupported type from AMF!");
    return 0;
}

//Alias around amf0_get_string_length
amf_err_t amf0_get_long_string_length( ors_data_t* source, size_t *value){
    return amf0_get_string_length(source, value);
}
//Alias around amf0_get_string
amf_err_t amf0_get_long_string( ors_data_t* source, void *value, size_t value_len){
    return amf0_get_string(source, value, value_len);
}

//Alias around amf0_get_string_length
amf_err_t amf0_get_xmldocument_length( ors_data_t* source, size_t *value){
    return amf0_get_string_length(source, value);
}
//Alias around amf0_get_string
amf_err_t amf0_get_xmldocument( ors_data_t* source, void *value, size_t value_len){
    return amf0_get_string(source, value, value_len);
}

//Unimplemented. Will implement if necessary.
amf_err_t amf0_get_recordset( ors_data_t* source ){
    emit_err("[Unimplemented] Trying to read Record Set from AMF!");
    return 0;
}
//Unimplemented. Will implement if necessary.
amf_err_t amf0_get_typed_object( ors_data_t* source ){
    emit_err("[Unimplemented] Trying to read Typed Object from AMF!");
    return 0;
}

void amf0_print( ors_data_t* data, size_t len, rtmp_printer_t printer ){
    double num;
    int integer;
    uint32_t uinteger;
    char str[1000];
    int object_layer = 0;
    size_t r = 0;

    while( !ors_data_eof(data) && r < len ){
        for( int i = 0; i < object_layer; ++i ){
            printer->s("    ");
        }
        if( object_layer > 0 ){
            r += amf0_get_prop_name( data, str, 1000);
            printer->s("\"");
            printer->s(str);
            printer->s("\": ");
        }
        byte type = amf0_next_type( data );
        if( type == AMF0_TYPE_UNSUPPORTED){
            break;
        }
        switch( type ){
            case AMF0_TYPE_NUMBER:
                r += amf0_get_number( data, &num);
                printer->s("Number: ");
                printer->f( num );
                break;
            case AMF0_TYPE_BOOLEAN:
                r += amf0_get_boolean( data, &integer);
                printer->s("Boolean: ");
                printer->d( integer );
                break;
            case AMF0_TYPE_STRING:
                r += amf0_get_string( data, str, 1000);
                printer->s( "String: " );
                printer->s( str );
                break;
            case AMF0_TYPE_OBJECT:
                r += amf0_get_object( data );
                printer->s("New Object");
                object_layer++;
                break;
            case AMF0_TYPE_MOVIECLIP:
                r += amf0_get_movieclip( data );
                break;
            case AMF0_TYPE_NULL:
                r += amf0_get_null( data);
                printer->s("Null");
                break;
            case AMF0_TYPE_UNDEFINED:
                r += amf0_get_undefined( data );
                printer->s("Undefined");
                break;
            case AMF0_TYPE_REFERENCE:
                r += amf0_get_reference( data, &uinteger);
                printer->s( "Reference to " );
                printer->u( uinteger );
                break;
            case AMF0_TYPE_ECMA_ARRAY:
                r += amf0_get_ecma_array( data);
                printer->s("ECMA Array");
                break;
            case AMF0_TYPE_OBJECT_END:
                r += amf0_get_object_end( data );
                printer->s("End Object");
                -- object_layer;
                break;
            case AMF0_TYPE_STRICT_ARRAY:
                r += amf0_get_strict_array( data );
                printer->s("Strict Array");
                break;
            case AMF0_TYPE_DATE:
                r += amf0_get_date( data, &integer, &num);
                printer->s( "Date: " );
                printer->f( num );
                printer->s( " +" );
                printer->d( integer );
                break;
            case AMF0_TYPE_LONG_STRING:
                r += amf0_get_string( data, str, 1000 );
                printer->s("Long String: ");
                printer->s(str);
                break;
            case AMF0_TYPE_UNSUPPORTED:
                r += amf0_get_unsupported( data );
                printer->s("Unsupported");
                break;
            case AMF0_TYPE_RECORDSET:
                r += amf0_get_recordset( data );
                printer->s("Record Set");
                break;
            case AMF0_TYPE_XML_DOCUMENT:
                r += amf0_get_string( data, str, 1000);
                printer->s("XML Document: ");
                printer->s(str);
                break;
            case AMF0_TYPE_TYPED_OBJECT:
                r += amf0_get_typed_object( data );
                printer->s("Typed Object");
                break;

        }
        printer->s("\n");
    }
}
