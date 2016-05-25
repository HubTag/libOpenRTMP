/*
//Return the type of the next item in the message
amf_type_t amf_next_type( const byte *buffer, size_t length ){
    AMF_CHECK_LENGTH(length, 1, AMF_TYPE_UNDEFINED);
    if( *buffer <= AMF_TYPE_AVMPLUS ){
        return *buffer;
    }
    return AMF_TYPE_UNSUPPORTED;
}

//Returns an IEEE 754 float from the data
amf_err_t amf_get_number( const byte *buffer, size_t length, double *value ){
    AMF_CHECK_LENGTH(length, 9, AMF_ERR_INCOMPLETE);
    AMF_CHECK_TYPE( buffer, AMF_TYPE_NUMBER, AMF_ERR_INVALID_DATA );
    ntoh_memcpy( value, buffer + 1, 8 );
    return 9;
}

amf_err_t amf_get_boolean( const byte *buffer, size_t length, int *value ){
    AMF_CHECK_LENGTH(length, 2, AMF_ERR_INCOMPLETE);
    AMF_CHECK_TYPE( buffer, AMF_TYPE_BOOLEAN, AMF_ERR_INVALID_DATA );
    *value = buffer[1];
    return 2;
}

//String functions are used for normal and long strings, as well as XML documents
//Used to verify how much storage to allocate for the upcoming string.
amf_err_t amf_get_string_length( const byte *buffer, size_t length, size_t *value ){
    AMF_CHECK_LENGTH(length, 1, AMF_ERR_INCOMPLETE);
    if( *buffer == AMF_TYPE_STRING ){
        AMF_CHECK_LENGTH(length, 3, AMF_ERR_INCOMPLETE);
        *value = ntoh_read_us(buffer + 1);
        return 3;
    }
    if( *buffer == AMF_TYPE_LONG_STRING || *buffer == AMF_TYPE_XML_DOCUMENT ){
        AMF_CHECK_LENGTH(length, 5, AMF_ERR_INCOMPLETE);
        *value = ntoh_read_ud(buffer + 1);
        return 5;
    }
    return AMF_ERR_INVALID_DATA;
}

//String functions are used for normal and long strings, as well as XML documents
amf_err_t amf_get_string( const byte *buffer, size_t length, char *value, int value_len ){
    size_t len;
    int offset = amf_get_string_length(buffer, length, &len);
    if( offset < 0 ){
        return offset;
    }
    value_len = len > value_len ? value_len : len;
    memcpy( value, buffer + offset, value_len );
    value[value_len] = 0;
    return offset + len;
}

//Mostly a dummy; this is used to verify and skip an object start marker
amf_err_t amf_get_object( const byte *buffer, size_t length){
    AMF_CHECK_LENGTH(length, 1, AMF_ERR_INCOMPLETE);
    AMF_CHECK_TYPE( buffer, AMF_TYPE_OBJECT, AMF_ERR_INVALID_DATA );
    return 1;
}

//If inside an object, use this to obtain the length of a property name
amf_err_t amf_get_prop_length( const byte *buffer, size_t length, size_t *value ){
    AMF_CHECK_LENGTH(length, 2, AMF_ERR_INCOMPLETE);
    *value = ntoh_read_us(buffer);
    return 0;
}

//If inside an object, use this to obtain a copy of the property name
amf_err_t amf_get_prop_name( const byte *buffer, size_t length, char* value, int value_len ){
    AMF_CHECK_LENGTH(length, 2, AMF_ERR_INCOMPLETE);
    size_t len = ntoh_read_us(buffer);
    if( len >= length + 2 ){
        return AMF_ERR_INVALID_DATA;
    }
    value_len = len > value_len ? value_len : len;
    memcpy( value, buffer + 2, value_len );
    value[value_len] = 0;
    return 2 + len;
}

//Dummy; do not use.
amf_err_t amf_get_movieclip( const byte *buffer, size_t length ){
    emit_err("[Erroneous Data] Trying to read movieclip from AMF!");
    return AMF_ERR_INVALID_DATA;
}

//Basically a dummy; used to verify that the next item is indeed a null value.
amf_err_t amf_get_null( const byte *buffer, size_t length){
    AMF_CHECK_LENGTH(length, 1, AMF_ERR_INCOMPLETE);
    AMF_CHECK_TYPE( buffer, AMF_TYPE_NULL, AMF_ERR_INVALID_DATA );
    return 1;
}


//Basically a dummy; used to verify that the next item is indeed an undefined value.
amf_err_t amf_get_undefined( const byte *buffer, size_t length){
    AMF_CHECK_LENGTH(length, 1, AMF_ERR_INCOMPLETE);
    AMF_CHECK_TYPE( buffer, AMF_TYPE_UNDEFINED, AMF_ERR_INVALID_DATA );
    return 1;
}

amf_err_t amf_get_reference( const byte *buffer, size_t length, unsigned int *value){
    AMF_CHECK_LENGTH(length, 3, AMF_ERR_INCOMPLETE);
    AMF_CHECK_TYPE( buffer, AMF_TYPE_REFERENCE, AMF_ERR_INVALID_DATA );
    *value = ntoh_read_us(buffer);
    return 3;
}

//Unimplemented. Will implement if necessary.
amf_err_t amf_get_ecma_array( const byte *buffer, size_t length){
    emit_err("[Unimplemented] Trying to read ECMA Array from AMF!");
    return 0;
}

//Mostly a dummy; this is used to verify and skip an object end marker
amf_err_t amf_get_object_end( const byte *buffer, size_t length){
    AMF_CHECK_LENGTH(length, 1, AMF_ERR_INCOMPLETE);
    AMF_CHECK_TYPE( buffer, AMF_TYPE_OBJECT_END, AMF_ERR_INVALID_DATA );
    return 1;
}

//Unimplemented. Will implement if necessary.
amf_err_t amf_get_strict_array( const byte *buffer, size_t length){
    emit_err("[Unimplemented] Trying to read Strict Array from AMF!");
    return 0;
}

//Returns a timezone offset as well as a double essentially representing a Unix timestamp
//(Resolution is 1:1 with seconds, epoch is 1970 Jan 1 00:00:00.000)
amf_err_t amf_get_date( const byte *buffer, size_t length, int* timezone, double* timestamp ){
    AMF_CHECK_LENGTH(length, 11, AMF_ERR_INCOMPLETE);
    AMF_CHECK_TYPE( buffer, AMF_TYPE_DATE, AMF_ERR_INVALID_DATA );
    *timezone = ntoh_read_s(buffer+1);
    ntoh_memcpy(timestamp, buffer + 3, 8);
    return 11;
}

//Dummy
amf_err_t amf_get_unsupported( const byte *buffer, size_t length){
    emit_err("[Error] Trying to read an unsupported type from AMF!");
    return 0;
}

//Alias around amf_get_string_length
amf_err_t amf_get_long_string_length( const byte *buffer, size_t length, size_t *value){
    return amf_get_string_length(buffer, length, value);
}
//Alias around amf_get_string
amf_err_t amf_get_long_string( const byte *buffer, size_t length, char* value, size_t value_len){
    return amf_get_string(buffer, length, value, value_len);
}

//Alias around amf_get_string_length
amf_err_t amf_get_xmldocument_length( const byte *buffer, size_t length, size_t *value){
    return amf_get_string_length(buffer, length, value);
}
//Alias around amf_get_string
amf_err_t amf_get_xmldocument( const byte *buffer, size_t length, char* value, size_t value_len){
    return amf_get_string(buffer, length, value, value_len);
}

//Unimplemented. Will implement if necessary.
amf_err_t amf_get_recordset( const byte *buffer, size_t length){
    emit_err("[Unimplemented] Trying to read Record Set from AMF!");
    return 0;
}
//Unimplemented. Will implement if necessary.
amf_err_t amf_get_typed_object( const byte *buffer, size_t length){
    emit_err("[Unimplemented] Trying to read Typed Object from AMF!");
    return 0;
}
*/
