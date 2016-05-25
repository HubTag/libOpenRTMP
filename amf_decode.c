/*
    amf_decode.c

    Copyright (C) 2016 Hubtag LLC.

    ----------------------------------------

    This file is part of libOpenRTMP.

    libOpenRTMP is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    libOpenRTMP is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with libOpenRTMP.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memutil.h"
#include "amf.h"
#include "data_stream.h"





#define AMF_HARVEST_LENGTH(in, len) byte buffer[len]; if( ors_data_read( in, buffer, len ) < len ){ return AMF_ERR_INCOMPLETE; }
#define AMF_PEEK_LENGTH(in, len) byte buffer[len]; if( ors_data_peek( in, buffer, len ) < len ){ return AMF_ERR_INCOMPLETE; }

//Provide safety macros that may be disabled if the user promises to be really really nice

#ifndef AMF_CONFIG_UNSAFE
#   define AMF_CHECK_TYPE(buffer, type, ret) if( *buffer != type ){ return ret; }
#else
#   define AMF_CHECK_TYPE(buffer, type, ret)
#endif // AMF_CONFIG_UNSAFE


//Return the type of the next item in the message
amf_type_t amf_next_type( ors_data_t* source ){
    byte type;
    if( ors_data_peek( source, &type, 1 ) < 1 ){
        return AMF_TYPE_UNSUPPORTED;
    }
    if( type <= AMF_TYPE_AVMPLUS ){
        return type;
    }
    return AMF_TYPE_UNSUPPORTED;
}

//Returns an IEEE 754 float from the data
amf_err_t amf_get_number( ors_data_t* source, double *value ){
    AMF_HARVEST_LENGTH(source, 9);
    AMF_CHECK_TYPE( buffer, AMF_TYPE_NUMBER, AMF_ERR_INVALID_DATA );
    ntoh_memcpy( value, buffer + 1, 8 );
    return 9;
}

amf_err_t amf_get_boolean( ors_data_t* source, int *value ){
    AMF_HARVEST_LENGTH(source, 2);
    AMF_CHECK_TYPE( buffer, AMF_TYPE_BOOLEAN, AMF_ERR_INVALID_DATA );
    *value = buffer[1];
    return 2;
}

//String functions are used for normal and long strings, as well as XML documents
//Used to verify how much storage to allocate for the upcoming string.
amf_err_t amf_get_string_length( ors_data_t* source, size_t *value ){
    byte buffer[5];
    int len = ors_data_peek( source, buffer, 5 );
    if( len < 3 ){
        return AMF_ERR_INCOMPLETE;
    }
    if( *buffer == AMF_TYPE_STRING ){
        *value = ntoh_read_us(buffer + 1);
        return 3;
    }
    if( len < 5 ){
        return AMF_ERR_INCOMPLETE;
    }
    if( *buffer == AMF_TYPE_LONG_STRING || *buffer == AMF_TYPE_XML_DOCUMENT ){
        *value = ntoh_read_ud(buffer + 1);
        return 5;
    }
    return AMF_ERR_INVALID_DATA;
}

//String functions are used for normal and long strings, as well as XML documents
amf_err_t amf_get_string( ors_data_t* source, void *value, size_t value_len ){
    if( value_len > 0 ){
        value_len--;
    }
    size_t len;
    byte scrap[5];
    int offset = amf_get_string_length(source, &len);
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
amf_err_t amf_get_object( ors_data_t* source){
    AMF_HARVEST_LENGTH(source, 1);
    AMF_CHECK_TYPE( buffer, AMF_TYPE_OBJECT, AMF_ERR_INVALID_DATA );
    return 1;
}

//If inside an object, use this to obtain the length of a property name
amf_err_t amf_get_prop_length( ors_data_t* source, size_t *value ){
    AMF_PEEK_LENGTH(source, 2);
    *value = ntoh_read_us(buffer);
    return 0;
}

//If inside an object, use this to obtain a copy of the property name
amf_err_t amf_get_prop_name( ors_data_t* source, void *value, int value_len ){
    AMF_HARVEST_LENGTH(source, 2);
    size_t len = ntoh_read_us(buffer);
    value_len = len > value_len ? value_len : len;
    ors_data_read( source, value, value_len );
    ((char*)value)[value_len] = 0;
    return 2 + len;
}

//Dummy; do not use.
amf_err_t amf_get_movieclip( ors_data_t* source ){
    emit_err("[Erroneous Data] Trying to read movieclip from AMF!");
    return AMF_ERR_INVALID_DATA;
}

//Basically a dummy; used to verify that the next item is indeed a null value.
amf_err_t amf_get_null( ors_data_t* source ){
    AMF_HARVEST_LENGTH(source, 1);
    AMF_CHECK_TYPE( buffer, AMF_TYPE_NULL, AMF_ERR_INVALID_DATA );
    return 1;
}


//Basically a dummy; used to verify that the next item is indeed an undefined value.
amf_err_t amf_get_undefined( ors_data_t* source ){
    AMF_HARVEST_LENGTH(source, 1);
    AMF_CHECK_TYPE( buffer, AMF_TYPE_UNDEFINED, AMF_ERR_INVALID_DATA );
    return 1;
}

amf_err_t amf_get_reference( ors_data_t* source, unsigned int *value){
    AMF_HARVEST_LENGTH(source, 3);
    AMF_CHECK_TYPE( buffer, AMF_TYPE_REFERENCE, AMF_ERR_INVALID_DATA );
    *value = ntoh_read_us(buffer);
    return 3;
}

//Unimplemented. Will implement if necessary.
amf_err_t amf_get_ecma_array( ors_data_t* source ){
    emit_err("[Unimplemented] Trying to read ECMA Array from AMF!");
    return 0;
}

//Mostly a dummy; this is used to verify and skip an object end marker
amf_err_t amf_get_object_end( ors_data_t* source ){
    AMF_HARVEST_LENGTH(source, 1);
    AMF_CHECK_TYPE( buffer, AMF_TYPE_OBJECT_END, AMF_ERR_INVALID_DATA );
    return 1;
}

//Unimplemented. Will implement if necessary.
amf_err_t amf_get_strict_array( ors_data_t* source ){
    emit_err("[Unimplemented] Trying to read Strict Array from AMF!");
    return 0;
}

//Returns a timezone offset as well as a double essentially representing a Unix timestamp
//(Resolution is 1:1 with seconds, epoch is 1970 Jan 1 00:00:00.000)
amf_err_t amf_get_date( ors_data_t* source, int* timezone, double* timestamp ){
    AMF_HARVEST_LENGTH(source, 11);
    AMF_CHECK_TYPE( buffer, AMF_TYPE_DATE, AMF_ERR_INVALID_DATA );
    *timezone = ntoh_read_s(buffer+1);
    ntoh_memcpy(timestamp, buffer + 3, 8);
    return 11;
}

//Dummy
amf_err_t amf_get_unsupported( ors_data_t* source ){
    emit_err("[Error] Trying to read an unsupported type from AMF!");
    return 0;
}

//Alias around amf_get_string_length
amf_err_t amf_get_long_string_length( ors_data_t* source, size_t *value){
    return amf_get_string_length(source, value);
}
//Alias around amf_get_string
amf_err_t amf_get_long_string( ors_data_t* source, void *value, size_t value_len){
    return amf_get_string(source, value, value_len);
}

//Alias around amf_get_string_length
amf_err_t amf_get_xmldocument_length( ors_data_t* source, size_t *value){
    return amf_get_string_length(source, value);
}
//Alias around amf_get_string
amf_err_t amf_get_xmldocument( ors_data_t* source, void *value, size_t value_len){
    return amf_get_string(source, value, value_len);
}

//Unimplemented. Will implement if necessary.
amf_err_t amf_get_recordset( ors_data_t* source ){
    emit_err("[Unimplemented] Trying to read Record Set from AMF!");
    return 0;
}
//Unimplemented. Will implement if necessary.
amf_err_t amf_get_typed_object( ors_data_t* source ){
    emit_err("[Unimplemented] Trying to read Typed Object from AMF!");
    return 0;
}

void amf_print( ors_data_t* data ){
    double num;
    int integer;
    unsigned int uinteger;
    char str[1000];
    int object_layer = 0;

    while( ors_data_get_remainder(data) > 0 ){
        for( int i = 0; i < object_layer; ++i ){
            printf("    ");
        }
        if( object_layer > 0 ){
            amf_get_prop_name( data, str, 1000);
            printf("\"%s\": ", str);
        }
        byte type = amf_next_type( data );
        if( type == AMF_TYPE_UNSUPPORTED){
            break;
        }
        switch( type ){
            case AMF_TYPE_NUMBER:
                amf_get_number( data, &num);
                printf("Number: %f\n", num);
                break;
            case AMF_TYPE_BOOLEAN:
                amf_get_boolean( data, &integer);
                printf("Boolean: %d\n", integer);
                break;
            case AMF_TYPE_STRING:
                amf_get_string( data, str, 1000);
                printf("String: %s\n", str);
                break;
            case AMF_TYPE_OBJECT:
                amf_get_object( data );
                printf("New Object\n");
                object_layer++;
                break;
            case AMF_TYPE_MOVIECLIP:
                amf_get_movieclip( data );
                break;
            case AMF_TYPE_NULL:
                amf_get_null( data);
                printf("Null\n");
                break;
            case AMF_TYPE_UNDEFINED:
                amf_get_undefined( data );
                printf("Undefined\n");
                break;
            case AMF_TYPE_REFERENCE:
                amf_get_reference( data, &uinteger);
                printf("Reference to %u\n", uinteger );
                break;
            case AMF_TYPE_ECMA_ARRAY:
                amf_get_ecma_array( data);
                printf("ECMA Array\n");
                break;
            case AMF_TYPE_OBJECT_END:
                amf_get_object_end( data );
                printf("End Object\n");
                -- object_layer;
                break;
            case AMF_TYPE_STRICT_ARRAY:
                amf_get_strict_array( data );
                printf("Strict Array\n");
                break;
            case AMF_TYPE_DATE:
                amf_get_date( data, &integer, &num);
                printf("Date: %f +%d \n", num, integer);
                break;
            case AMF_TYPE_LONG_STRING:
                amf_get_string( data, str, 1000 );
                printf("Long String: %s\n", str);
                break;
            case AMF_TYPE_UNSUPPORTED:
                amf_get_unsupported( data );
                printf("Unsupported\n");
                break;
            case AMF_TYPE_RECORDSET:
                amf_get_recordset( data );
                printf("Record Set\n");
                break;
            case AMF_TYPE_XML_DOCUMENT:
                amf_get_string( data, str, 1000);
                printf("XML Document: %s\n", str);
                break;
            case AMF_TYPE_TYPED_OBJECT:
                amf_get_typed_object( data );
                printf("Typed Object\n");
                break;

        }
    }
}

#include <time.h>

int main(){
    ors_data_t data = ors_data_create_memsnk( 500 );

    amf_write_boolean( data, 1 );
    amf_write_boolean( data, 0 );
    char *str = "Well hello there, everyone!";
    amf_write_string( data, str, strlen( str ) );
    amf_write_object( data );
        str = "Item 1";
        amf_write_prop_name( data, str, strlen( str ) );
        amf_write_number( data, 1337 );
        str = "Foobar";
        amf_write_prop_name( data, str, strlen( str ) );
        amf_write_null( data );
        str = "Nested";
        amf_write_prop_name( data, str, strlen( str ) );
        amf_write_object( data );
            str = "The Time";
            amf_write_prop_name( data, str, strlen( str ) );
            amf_write_date( data, 0, time(0) );
            str = "Name";
            amf_write_prop_name( data, str, strlen( str ) );
            str = "Droogie";
            amf_write_string( data, str, strlen( str ) );
            amf_write_prop_name( data, "", 0 );
            amf_write_object_end( data );
        str = "Item 3";
        amf_write_prop_name( data, str, strlen( str ) );
        amf_write_number( data, 123456.789 );
        amf_write_prop_name( data, "", 0 );
    amf_write_object_end( data );
    str = "Last";
    amf_write_string( data, str, strlen( str ) );

    ors_data_seek( data, 0, ORS_SEEK_START );
    amf_print( data );
    ors_data_close( data );
    ors_data_destroy( data );
    return 0;
}
