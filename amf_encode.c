#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memutil.h"
#include "amf.h"
#include "data_stream.h"

//Returns an IEEE 754 float from the data
amf_err_t amf_write_number( ors_data_t* dest, double value ){
    byte data[sizeof(double)+1];
    data[0] = AMF_TYPE_NUMBER;
    ntoh_memcpy( data + 1, &value, sizeof( double ) );
    ors_data_write( dest, data, sizeof(double) + 1);
    return 9;
}

amf_err_t amf_write_boolean( ors_data_t* dest, int value ){
    byte data[2];
    data[0] = AMF_TYPE_BOOLEAN;
    data[1] = value & 255;
    ors_data_write( dest, data, 2);
    return 2;
}

//String functions are used for all strings.
static amf_err_t amf_write_string_internal( ors_data_t* dest, byte type, void *value, size_t value_len ){
    byte header[5];
    int len = 3;
    header[0] = AMF_TYPE_STRING;
    if( value_len > 0xFFFF || type == AMF_TYPE_XML_DOCUMENT ){
        header[0] = type == AMF_TYPE_XML_DOCUMENT ? AMF_TYPE_XML_DOCUMENT : AMF_TYPE_LONG_STRING;
        ntoh_write_ud( header + 1, value_len );
        len = 5;
    }
    else{
        ntoh_write_us( header + 1, value_len );
    }
    ors_data_write( dest, header, len );
    ors_data_write( dest, value, value_len );
    return value_len + len;
}

//String functions are used for normal and long strings.
amf_err_t amf_write_string( ors_data_t* dest, void *value, size_t value_len ){
    return amf_write_string_internal( dest, AMF_TYPE_STRING, value, value_len );
}

//Emit an object start marker
amf_err_t amf_write_object( ors_data_t* dest){
    byte data[1];
    data[0] = AMF_TYPE_OBJECT;
    ors_data_write( dest, data, 1);
    return 1;
}

//If inside an object, use this to obtain a copy of the property name
amf_err_t amf_write_prop_name( ors_data_t* dest, void *value, size_t value_len ){
    byte len[2];
    ntoh_write_us( len, value_len );
    ors_data_write( dest, len, 2 );
    ors_data_write( dest, value, value_len );
    return 2 + value_len;
}

//Dummy; do not use.
amf_err_t amf_write_movieclip( ors_data_t* dest ){
    emit_err("[Erroneous Data] Trying to write movieclip to AMF!");
    return AMF_ERR_INVALID_DATA;
}

//Basically a dummy; used to verify that the next item is indeed a null value.
amf_err_t amf_write_null( ors_data_t* dest ){
    byte data[1];
    data[0] = AMF_TYPE_NULL;
    ors_data_write( dest, data, 1);
    return 1;
}


//Basically a dummy; used to verify that the next item is indeed an undefined value.
amf_err_t amf_write_undefined( ors_data_t* dest ){
    byte data[1];
    data[0] = AMF_TYPE_UNDEFINED;
    ors_data_write( dest, data, 1);
    return 1;
}

amf_err_t amf_write_reference( ors_data_t* dest, unsigned int value){
    byte data[3];
    data[0] = AMF_TYPE_REFERENCE;
    ntoh_write_us( data + 1, value );
    ors_data_write( dest, data, 3);
    return 3;
}

//Unimplemented. Will implement if necessary.
amf_err_t amf_write_ecma_array( ors_data_t* dest ){
    emit_err("[Unimplemented] Trying to write ECMA Array to AMF!");
    return 0;
}

//Mostly a dummy; this is used to verify and skip an object end marker
amf_err_t amf_write_object_end( ors_data_t* dest ){
    byte data[1];
    data[0] = AMF_TYPE_OBJECT_END;
    ors_data_write( dest, data, 1);
    return 1;
}

//Unimplemented. Will implement if necessary.
amf_err_t amf_write_strict_array( ors_data_t* dest ){
    emit_err("[Unimplemented] Trying to write Strict Array to AMF!");
    return 0;
}

//Returns a timezone offset as well as a double essentially representing a Unix timestamp
//(Resolution is 1:1 with seconds, epoch is 1970 Jan 1 00:00:00.000)
amf_err_t amf_write_date( ors_data_t* dest, int timezone, double timestamp ){
    byte data[sizeof(double)+3];
    data[0] = AMF_TYPE_DATE;
    ntoh_write_s( data + 1, timezone );
    ntoh_memcpy( data + 3, &timestamp, sizeof( timestamp ) );
    ors_data_write( dest, data, sizeof(double) + 3);
    return sizeof(double)+3;
}

//Dummy
amf_err_t amf_write_unsupported( ors_data_t* dest ){
    emit_err("[Error] Trying to write an unsupported type to AMF!");
    return 0;
}

//Alias around amf_write_string_internal
amf_err_t amf_write_long_string( ors_data_t* dest, void *value, size_t value_len){
    return amf_write_string_internal( dest, AMF_TYPE_LONG_STRING, value, value_len );
}

//Alias around amf_write_string_internal
amf_err_t amf_write_xmldocument( ors_data_t* dest, void *value, size_t value_len){
    return amf_write_string_internal( dest, AMF_TYPE_XML_DOCUMENT, value, value_len );
}

//Unimplemented. Will implement if necessary.
amf_err_t amf_write_recordset( ors_data_t* dest ){
    emit_err("[Unimplemented] Trying to write Record Set to AMF!");
    return 0;
}
//Unimplemented. Will implement if necessary.
amf_err_t amf_write_typed_object( ors_data_t* dest ){
    emit_err("[Unimplemented] Trying to write Typed Object to AMF!");
    return 0;
}
