#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void emit_err(const char* err){
    printf("%s\n", err);
}

typedef unsigned char byte;
typedef unsigned long size_t;

typedef enum AMF_TYPE{
    AMF_TYPE_NUMBER,
    AMF_TYPE_BOOLEAN,
    AMF_TYPE_STRING,
    AMF_TYPE_OBJECT,
    AMF_TYPE_MOVIECLIP,
    AMF_TYPE_NULL,
    AMF_TYPE_UNDEFINED,
    AMF_TYPE_REFERENCE,
    AMF_TYPE_ECMA_ARRAY,
    AMF_TYPE_OBJECT_END,
    AMF_TYPE_STRICT_ARRAY,
    AMF_TYPE_DATE,
    AMF_TYPE_LONG_STRING,
    AMF_TYPE_UNSUPPORTED,
    AMF_TYPE_RECORDSET,
    AMF_TYPE_XML_DOCUMENT,
    AMF_TYPE_TYPED_OBJECT,
    AMF_TYPE_AVMPLUS
} amf_type_t;

typedef enum AMF_ERR {
    AMF_ERR_NONE,
    AMF_ERR_INVALID_DATA = -1
} amf_err_t;

//memcpy that will reverse byte order if the machine is little endian
static void ntoh_memcpy(void *dst, const void *src, size_t len){
    #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        byte *output = dst;
        const byte *input = src;
        while( len --> 0 ){
            *(output++) = input[len];
        }
    #elif __BYTE_ORDER__ == __ORDER_PDP_ENDIAN__
        #error "Intentionally unsupported"
    #else
        memcpy( dst, src, len );
    #endif
}

//Read a short from a buffer based on endianess
static short ntoh_read_s(const void *src){
    short value;
    ntoh_memcpy( &value, src, 2 );
    return value;
}

//Read an int from a buffer based on endianess
static int ntoh_read_d(const void *src){
    int value;
    ntoh_memcpy( &value, src, 4 );
    return value;
}

//Read a short from a buffer based on endianess
static unsigned short ntoh_read_us(const void *src){
    unsigned short value;
    ntoh_memcpy( &value, src, 2 );
    return value;
}

//Read an int from a buffer based on endianess
static unsigned int ntoh_read_ud(const void *src){
    unsigned int value;
    ntoh_memcpy( &value, src, 4 );
    return value;
}

//Provide safety macros that may be disabled if the user promises to be really really nice

#ifndef AMF_CONFIG_UNSAFE
#   define AMF_CHECK_LENGTH(lenvar, len, ret) if( length < len ){ return ret; }
#   define AMF_CHECK_TYPE(buffer, type, ret) if( *buffer != type ){ return ret; }
#else
#   define AMF_CHECK_LENGTH(lenvar, len, ret)
#   define AMF_CHECK_TYPE(buffer, type, ret)
#endif // AMF_CONFIG_UNSAFE

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
    AMF_CHECK_LENGTH(length, 9, AMF_ERR_INVALID_DATA);
    AMF_CHECK_TYPE( buffer, AMF_TYPE_NUMBER, AMF_ERR_INVALID_DATA );
    ntoh_memcpy( value, buffer + 1, 8 );
    return 9;
}

amf_err_t amf_get_boolean( const byte *buffer, size_t length, int *value ){
    AMF_CHECK_LENGTH(length, 2, AMF_ERR_INVALID_DATA);
    AMF_CHECK_TYPE( buffer, AMF_TYPE_BOOLEAN, AMF_ERR_INVALID_DATA );
    *value = buffer[1];
    return 2;
}

//String functions are used for normal and long strings, as well as XML documents
//Used to verify how much storage to allocate for the upcoming string.
amf_err_t amf_get_string_length( const byte *buffer, size_t length, size_t *value ){
    AMF_CHECK_LENGTH(length, 1, AMF_ERR_INVALID_DATA);
    if( *buffer == AMF_TYPE_STRING ){
        AMF_CHECK_LENGTH(length, 3, AMF_ERR_INVALID_DATA);
        *value = ntoh_read_us(buffer + 1);
        return 3;
    }
    if( *buffer == AMF_TYPE_LONG_STRING || *buffer == AMF_TYPE_XML_DOCUMENT ){
        AMF_CHECK_LENGTH(length, 5, AMF_ERR_INVALID_DATA);
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
    AMF_CHECK_LENGTH(length, 1, AMF_ERR_INVALID_DATA);
    AMF_CHECK_TYPE( buffer, AMF_TYPE_OBJECT, AMF_ERR_INVALID_DATA );
    return 1;
}

//If inside an object, use this to obtain the length of a property name
amf_err_t amf_get_prop_length( const byte *buffer, size_t length, size_t *value ){
    AMF_CHECK_LENGTH(length, 2, AMF_ERR_INVALID_DATA);
    *value = ntoh_read_us(buffer);
    return 0;
}

//If inside an object, use this to obtain a copy of the property name
amf_err_t amf_get_prop_name( const byte *buffer, size_t length, char* value, int value_len ){
    AMF_CHECK_LENGTH(length, 2, AMF_ERR_INVALID_DATA);
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
    AMF_CHECK_LENGTH(length, 1, AMF_ERR_INVALID_DATA);
    AMF_CHECK_TYPE( buffer, AMF_TYPE_NULL, AMF_ERR_INVALID_DATA );
    return 1;
}


//Basically a dummy; used to verify that the next item is indeed an undefined value.
amf_err_t amf_get_undefined( const byte *buffer, size_t length){
    AMF_CHECK_LENGTH(length, 1, AMF_ERR_INVALID_DATA);
    AMF_CHECK_TYPE( buffer, AMF_TYPE_UNDEFINED, AMF_ERR_INVALID_DATA );
    return 1;
}

amf_err_t amf_get_reference( const byte *buffer, size_t length, unsigned int *value){
    AMF_CHECK_LENGTH(length, 3, AMF_ERR_INVALID_DATA);
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
    AMF_CHECK_LENGTH(length, 1, AMF_ERR_INVALID_DATA);
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
    AMF_CHECK_LENGTH(length, 11, AMF_ERR_INVALID_DATA);
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



int main(){
    byte test_data[] = {
        0x02, 0x00, 0x0d, 0x40, 0x73, 0x65, 0x74, 0x44, 0x61, 0x74, 0x61, 0x46, 0x72, 0x61, 0x6d,
        0x65, 0x02, 0x00, 0x0a, 0x6f, 0x6e, 0x4d, 0x65, 0x74, 0x61, 0x44, 0x61, 0x74, 0x61, 0x03,
        0x00, 0x08, 0x64, 0x75, 0x72, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x66, 0x69, 0x6c, 0x65, 0x53, 0x69, 0x7a, 0x65, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x77, 0x69, 0x64, 0x74, 0x68,
        0x00, 0x40, 0x9e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x68, 0x65, 0x69, 0x67,
        0x68, 0x74, 0x00, 0x40, 0x90, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x76, 0x69,
        0x64, 0x65, 0x6f, 0x63, 0x6f, 0x64, 0x65, 0x63, 0x69, 0x64, 0x02, 0x00, 0x04, 0x61, 0x76,
        0x63, 0x31, 0x00, 0x0d, 0x76, 0x69, 0x64, 0x65, 0x6f, 0x64, 0x61, 0x74, 0x61, 0x72, 0x61,
        0x74, 0x65, 0x00, 0x40, 0x8f, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x66, 0x72,
        0x61, 0x6d, 0x65, 0x72, 0x61, 0x74, 0x65, 0x00, 0x40, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x0c, 0x61, 0x75, 0x64, 0x69, 0x6f, 0x63, 0x6f, 0x64, 0x65, 0x63, 0x69, 0x64,
        0x02, 0x00, 0x04, 0x6d, 0x70, 0x34, 0x61, 0x00, 0x0d, 0x61, 0x75, 0x64, 0x69, 0x6f, 0x64,
        0x61, 0x74, 0x61, 0x72, 0x61, 0x74, 0x65, 0x00, 0x40, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x0f, 0x61, 0x75, 0x64, 0x69, 0x6f, 0x73, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x72,
        0x61, 0x74, 0x65, 0x00, 0x40, 0xe7, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x61,
        0x75, 0x64, 0x69, 0x6f, 0x73, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x73, 0x69, 0x7a, 0x65, 0x00,
        0x40, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0d, 0x61, 0x75, 0x64, 0x69, 0x6f,
        0x63, 0x68, 0x61, 0x6e, 0x6e, 0x65, 0x6c, 0x73, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x06, 0x73, 0x74, 0x65, 0x72, 0x65, 0x6f, 0x01, 0x01, 0x00, 0x07, 0x65,
        0x6e, 0x63, 0x6f, 0x64, 0x65, 0x72, 0x02, 0x00, 0x21, 0x4f, 0x70, 0x65, 0x6e, 0x20, 0x42,
        0x72, 0x6f, 0x61, 0x64, 0x63, 0x61, 0x73, 0x74, 0x65, 0x72, 0x20, 0x53, 0x6f, 0x66, 0x74,
        0x77, 0x61, 0x72, 0x65, 0x20, 0x76, 0x30, 0x2e, 0x36, 0x35, 0x37, 0x62, 0x00, 0x00, 0x09
    };
    size_t test_data_len = sizeof( test_data ) / sizeof( test_data[0] );
    byte* data = test_data;
    size_t len = test_data_len;

    double num;
    int integer;
    char str[1000];
    int object_layer = 0;
    size_t offset = 0;

    while( offset < test_data_len ){
        for( int i = 0; i < object_layer; ++i ){
            printf("    ");
        }
        if( object_layer > 0 ){
            offset += amf_get_prop_name( data + offset, len - offset, str, 1000);
            printf("\"%s\": ", str);
        }
        byte type = amf_next_type( data + offset, len - offset );
        switch( type ){
            case AMF_TYPE_NUMBER:
                offset += amf_get_number( data + offset, len - offset, &num);
                printf("Number: %f\n", num);
                break;
            case AMF_TYPE_BOOLEAN:
                offset += amf_get_boolean( data + offset, len - offset, &integer);
                printf("Boolean: %d\n", integer);
                break;
            case AMF_TYPE_STRING:
                offset += amf_get_string( data + offset, len - offset, str, 1000);
                printf("String: %s\n", str);
                break;
            case AMF_TYPE_OBJECT:
                offset += amf_get_object( data + offset, len - offset );
                printf("New Object\n");
                object_layer++;
                break;
            case AMF_TYPE_MOVIECLIP:
                offset += amf_get_movieclip( data + offset, len - offset );
                break;
            case AMF_TYPE_NULL:
                offset += amf_get_null( data + offset, len - offset);
                printf("Null\n");
                break;
            case AMF_TYPE_UNDEFINED:
                offset += amf_get_undefined( data + offset, len - offset);
                printf("Undefined\n");
                break;
            case AMF_TYPE_REFERENCE:
                offset += amf_get_reference( data + offset, len - offset, &integer);
                printf("Reference to %d\n", integer );
                break;
            case AMF_TYPE_ECMA_ARRAY:
                offset += amf_get_ecma_array( data + offset, len - offset);
                printf("ECMA Array\n");
                break;
            case AMF_TYPE_OBJECT_END:
                offset += amf_get_object_end( data + offset, len - offset);
                printf("End Object\n");
                -- object_layer;
                break;
            case AMF_TYPE_STRICT_ARRAY:
                offset += amf_get_strict_array( data + offset, len - offset);
                printf("Strict Array\n");
                break;
            case AMF_TYPE_DATE:
                offset += amf_get_date( data + offset, len - offset, &integer, &num);
                printf("Date: %f +%d \n", num, integer);
                break;
            case AMF_TYPE_LONG_STRING:
                offset += amf_get_string( data + offset, len - offset, str, 1000);
                printf("Long String: %s\n", str);
                break;
            case AMF_TYPE_UNSUPPORTED:
                offset += amf_get_unsupported( data + offset, len - offset);
                printf("Unsupported\n");
                break;
            case AMF_TYPE_RECORDSET:
                offset += amf_get_recordset( data + offset, len - offset);
                printf("Record Set\n");
                break;
            case AMF_TYPE_XML_DOCUMENT:
                offset += amf_get_string( data + offset, len - offset, str, 1000);
                printf("XML Document: %s\n", str);
                break;
            case AMF_TYPE_TYPED_OBJECT:
                offset += amf_get_typed_object( data + offset, len - offset);
                printf("Typed Object\n");
                break;
        }
    }
}
