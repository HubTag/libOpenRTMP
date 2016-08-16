/*
    amf.h

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

#ifndef RTMP_H_AMF_H
#define RTMP_H_AMF_H

#include "rtmp/rtmp_types.h"
#include "amf_constants.h"
#include "data_stream.h"
#include "rtmp_debug.h"

amf_err_t amf0_write_number( byte* data, size_t len, double value );
amf_err_t amf0_write_boolean( byte* data, size_t len, int value );
amf_err_t amf0_write_string( byte* data, size_t len, const void *value, size_t value_len );
amf_err_t amf0_write_object( byte* data, size_t len);
amf_err_t amf0_write_prop_name( byte* data, size_t len, const void *value, size_t value_len );
amf_err_t amf0_write_movieclip( byte* data, size_t len );
amf_err_t amf0_write_null( byte* data, size_t len );
amf_err_t amf0_write_undefined( byte* data, size_t len );
amf_err_t amf0_write_reference( byte* data, size_t len, uint32_t value);
amf_err_t amf0_write_ecma_array( byte* data, size_t len, uint32_t array_len );
amf_err_t amf0_write_object_end( byte* data, size_t len );
amf_err_t amf0_write_strict_array( byte* data, size_t len );
amf_err_t amf0_write_date( byte* data, size_t len, int timezone, double timestamp );
amf_err_t amf0_write_unsupported( byte* data, size_t len );
amf_err_t amf0_write_long_string( byte* data, size_t len, const void *value, size_t value_len);
amf_err_t amf0_write_xmldocument( byte* data, size_t len, const void *value, size_t value_len);
amf_err_t amf0_write_recordset( byte* data, size_t len );
amf_err_t amf0_write_typed_object( byte* data, size_t len );
amf_err_t amf0_write_continue( byte* data, size_t len, const void *value, size_t value_len );



amf0_type_t amf0_next_type( const byte* data, size_t len );
amf_err_t amf0_get_number( const byte* data, size_t len, double *value );
amf_err_t amf0_get_boolean( const byte* data, size_t len, int *value );
amf_err_t amf0_get_string_length( const byte* data, size_t len, size_t *value );
amf_err_t amf0_get_string( const byte* data, size_t len, void *value, size_t value_len, size_t *continuation );
amf_err_t amf0_get_object( const byte* data, size_t len );
amf_err_t amf0_get_prop_length( const byte* data, size_t len, size_t *value );
amf_err_t amf0_get_prop_name( const byte* data, size_t len, void *value, size_t value_len );
amf_err_t amf0_get_movieclip( const byte* data, size_t len );
amf_err_t amf0_get_null( const byte* data, size_t len );
amf_err_t amf0_get_undefined( const byte* data, size_t len );
amf_err_t amf0_get_reference( const byte* data, size_t len, uint32_t *value);
amf_err_t amf0_get_ecma_array( const byte* data, size_t len, uint32_t *num_memb );
amf_err_t amf0_get_object_end( const byte* data, size_t len );
amf_err_t amf0_get_strict_array( const byte* data, size_t len );
amf_err_t amf0_get_date( const byte* data, size_t len, int* timezone, double* timestamp );
amf_err_t amf0_get_unsupported( const byte* data, size_t len );
amf_err_t amf0_get_long_string_length( const byte* data, size_t len, size_t *value);
amf_err_t amf0_get_long_string( const byte* data, size_t len, void *value, size_t value_len, size_t *continuation);
amf_err_t amf0_get_xmldocument_length( const byte* data, size_t len, size_t *value);
amf_err_t amf0_get_xmldocument( const byte* data, size_t len, void *value, size_t value_len, size_t *continuation);
amf_err_t amf0_get_recordset( const byte* data, size_t len );
amf_err_t amf0_get_typed_object( const byte* data, size_t len );
amf_err_t amf0_get_continue( const byte* data, size_t len, void *value, size_t value_len, size_t *continuation );

void amf0_print( const byte* data, size_t len, rtmp_printer_t printer );

/*
amf_err_t amf3_write_undefined( byte *data, size_t len );
amf_err_t amf3_write_null( byte *data, size_t len );
amf_err_t amf3_write_false( byte *data, size_t len );
amf_err_t amf3_write_true( byte *data, size_t len );
amf_err_t amf3_write_int( byte *data, size_t len, int value );
amf_err_t amf3_write_double( byte *data, size_t len, double value );
amf_err_t amf3_write_string( byte *data, size_t len, byte *str, size_t str_len );
amf_err_t amf3_write_xmldocument( byte *data, size_t len, byte *str, size_t str_len);
amf_err_t amf3_write_date( byte *data, size_t len, double timestamp );
amf_err_t amf3_write_array_begin( byte *data, size_t len, size_t dense_len );
amf_err_t amf3_write_array_mid( byte *data, size_t len, size_t assoc_len );
amf_err_t amf3_write_array_key( byte *data, size_t len, byte *str, size_t str_len );
amf_err_t amf3_write_object( byte *data, size_t len, );
amf_err_t amf3_write_xml( byte *data, size_t len, );
amf_err_t amf3_write_array_byte( byte *data, size_t len, );
amf_err_t amf3_write_vec_int( byte *data, size_t len, );
amf_err_t amf3_write_vec_uint( byte *data, size_t len, );
amf_err_t amf3_write_vec_double( byte *data, size_t len, );
amf_err_t amf3_write_vec_object( byte *data, size_t len, );
amf_err_t amf3_write_dict( byte *data, size_t len, );
amf_err_t amf3_write_reference( byte *data, size_t len, amf3_type_t type, size_t index );


amf_err_t amf3_get_undefined( byte *data, size_t len );
amf_err_t amf3_get_null( byte *data, size_t len );
amf_err_t amf3_get_false( byte *data, size_t len );
amf_err_t amf3_get_true( byte *data, size_t len );
amf_err_t amf3_get_int( byte *data, size_t len, int *value );
amf_err_t amf3_get_double( byte *data, size_t len, double *value );
amf_err_t amf3_get_string_length( byte *data, size_t len, size_t *length );
amf_err_t amf3_get_string( byte *data, size_t len, byte *str, size_t str_len, size_t *continuation );
amf_err_t amf3_get_xmldocument_length( byte *data, size_t len, size_t *length );
amf_err_t amf3_get_xmldocument( byte *data, size_t len, byte *str, size_t str_len, size_t *continuation );
amf_err_t amf3_get_date( byte *data, size_t len, double *timestamp );
amf_err_t amf3_get_array( byte *data, size_t len, );
amf_err_t amf3_get_object( byte *data, size_t len, );
amf_err_t amf3_get_xml( byte *data, size_t len, );
amf_err_t amf3_get_array_byte( byte *data, size_t len, );
amf_err_t amf3_get_vec_int( byte *data, size_t len, );
amf_err_t amf3_get_vec_uint( byte *data, size_t len, );
amf_err_t amf3_get_vec_double( byte *data, size_t len, );
amf_err_t amf3_get_vec_object( byte *data, size_t len, );
amf_err_t amf3_get_dict( byte *data, size_t len, );
//If AMF_ERR_NOT_A_REFERENCE is returned, then obviously the value is not a reference.
amf_err_t amf3_get_reference( byte *data, size_t len, amf3_type_t *type, size_t *index );*/

#endif
