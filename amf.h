#pragma once

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
    AMF_ERR_INVALID_DATA = -10,
    AMF_ERR_INCOMPLETE
} amf_err_t;


#include "data_stream.h"

amf_err_t amf_write_number( ors_data_t* dest, double value );
amf_err_t amf_write_boolean( ors_data_t* dest, int value );
amf_err_t amf_write_string( ors_data_t* dest, void *value, size_t value_len );
amf_err_t amf_write_object( ors_data_t* dest);
amf_err_t amf_write_prop_name( ors_data_t* dest, void *value, size_t value_len );
amf_err_t amf_write_movieclip( ors_data_t* dest );
amf_err_t amf_write_null( ors_data_t* dest );
amf_err_t amf_write_undefined( ors_data_t* dest );
amf_err_t amf_write_reference( ors_data_t* dest, unsigned int value);
amf_err_t amf_write_ecma_array( ors_data_t* dest );
amf_err_t amf_write_object_end( ors_data_t* dest );
amf_err_t amf_write_strict_array( ors_data_t* dest );
amf_err_t amf_write_date( ors_data_t* dest, int timezone, double timestamp );
amf_err_t amf_write_unsupported( ors_data_t* dest );
amf_err_t amf_write_long_string( ors_data_t* dest, void *value, size_t value_len);
amf_err_t amf_write_xmldocument( ors_data_t* dest, void *value, size_t value_len);
amf_err_t amf_write_recordset( ors_data_t* dest );
amf_err_t amf_write_typed_object( ors_data_t* dest );



amf_type_t amf_next_type( ors_data_t* source );
amf_err_t amf_get_number( ors_data_t* source, double *value );
amf_err_t amf_get_boolean( ors_data_t* source, int *value );
amf_err_t amf_get_string_length( ors_data_t* source, size_t *value );
amf_err_t amf_get_string( ors_data_t* source, void *value, size_t value_len );
amf_err_t amf_get_object( ors_data_t* source);
amf_err_t amf_get_prop_length( ors_data_t* source, size_t *value );
amf_err_t amf_get_prop_name( ors_data_t* source, void *value, int value_len );
amf_err_t amf_get_movieclip( ors_data_t* source );
amf_err_t amf_get_null( ors_data_t* source );
amf_err_t amf_get_undefined( ors_data_t* source );
amf_err_t amf_get_reference( ors_data_t* source, unsigned int *value);
amf_err_t amf_get_ecma_array( ors_data_t* source );
amf_err_t amf_get_object_end( ors_data_t* source );
amf_err_t amf_get_strict_array( ors_data_t* source );
amf_err_t amf_get_date( ors_data_t* source, int* timezone, double* timestamp );
amf_err_t amf_get_unsupported( ors_data_t* source );
amf_err_t amf_get_long_string_length( ors_data_t* source, size_t *value);
amf_err_t amf_get_long_string( ors_data_t* source, void *value, size_t value_len);
amf_err_t amf_get_xmldocument_length( ors_data_t* source, size_t *value);
amf_err_t amf_get_xmldocument( ors_data_t* source, void *value, size_t value_len);
amf_err_t amf_get_recordset( ors_data_t* source );
amf_err_t amf_get_typed_object( ors_data_t* source );

void amf_print( ors_data_t* data );
