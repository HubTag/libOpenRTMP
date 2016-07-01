/*
    amf.h

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

#pragma once
#include "amf_constants.h"
#include "data_stream.h"
#include "rtmp_debug.h"

amf_err_t amf0_write_number( ors_data_t* dest, double value );
amf_err_t amf0_write_boolean( ors_data_t* dest, int value );
amf_err_t amf0_write_string( ors_data_t* dest, void *value, size_t value_len );
amf_err_t amf0_write_object( ors_data_t* dest);
amf_err_t amf0_write_prop_name( ors_data_t* dest, void *value, size_t value_len );
amf_err_t amf0_write_movieclip( ors_data_t* dest );
amf_err_t amf0_write_null( ors_data_t* dest );
amf_err_t amf0_write_undefined( ors_data_t* dest );
amf_err_t amf0_write_reference( ors_data_t* dest, unsigned int value);
amf_err_t amf0_write_ecma_array( ors_data_t* dest );
amf_err_t amf0_write_object_end( ors_data_t* dest );
amf_err_t amf0_write_strict_array( ors_data_t* dest );
amf_err_t amf0_write_date( ors_data_t* dest, int timezone, double timestamp );
amf_err_t amf0_write_unsupported( ors_data_t* dest );
amf_err_t amf0_write_long_string( ors_data_t* dest, void *value, size_t value_len);
amf_err_t amf0_write_xmldocument( ors_data_t* dest, void *value, size_t value_len);
amf_err_t amf0_write_recordset( ors_data_t* dest );
amf_err_t amf0_write_typed_object( ors_data_t* dest );



amf0_type_t amf0_next_type( ors_data_t* source );
amf_err_t amf0_get_number( ors_data_t* source, double *value );
amf_err_t amf0_get_boolean( ors_data_t* source, int *value );
amf_err_t amf0_get_string_length( ors_data_t* source, size_t *value );
amf_err_t amf0_get_string( ors_data_t* source, void *value, size_t value_len );
amf_err_t amf0_get_object( ors_data_t* source);
amf_err_t amf0_get_prop_length( ors_data_t* source, size_t *value );
amf_err_t amf0_get_prop_name( ors_data_t* source, void *value, int value_len );
amf_err_t amf0_get_movieclip( ors_data_t* source );
amf_err_t amf0_get_null( ors_data_t* source );
amf_err_t amf0_get_undefined( ors_data_t* source );
amf_err_t amf0_get_reference( ors_data_t* source, unsigned int *value);
amf_err_t amf0_get_ecma_array( ors_data_t* source );
amf_err_t amf0_get_object_end( ors_data_t* source );
amf_err_t amf0_get_strict_array( ors_data_t* source );
amf_err_t amf0_get_date( ors_data_t* source, int* timezone, double* timestamp );
amf_err_t amf0_get_unsupported( ors_data_t* source );
amf_err_t amf0_get_long_string_length( ors_data_t* source, size_t *value);
amf_err_t amf0_get_long_string( ors_data_t* source, void *value, size_t value_len);
amf_err_t amf0_get_xmldocument_length( ors_data_t* source, size_t *value);
amf_err_t amf0_get_xmldocument( ors_data_t* source, void *value, size_t value_len);
amf_err_t amf0_get_recordset( ors_data_t* source );
amf_err_t amf0_get_typed_object( ors_data_t* source );

void amf0_print( ors_data_t* data, size_t len, rtmp_printer_t printer );

int read_var_int(ors_data_t* source, int *value);
int write_var_int(ors_data_t* dest, int value);
