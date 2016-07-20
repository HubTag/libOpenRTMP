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

#pragma once

#include "../rtmp/rtmp_types.h"
#include "amf_constants.h"
#include "../data_stream.h"
#include "../rtmp_debug.h"

amf_err_t amf0_write_number( byte* data, size_t len, double value );
amf_err_t amf0_write_boolean( byte* data, size_t len, int value );
amf_err_t amf0_write_string( byte* data, size_t len, void *value, size_t value_len );
amf_err_t amf0_write_object( byte* data, size_t len);
amf_err_t amf0_write_prop_name( byte* data, size_t len, void *value, size_t value_len );
amf_err_t amf0_write_movieclip( byte* data, size_t len );
amf_err_t amf0_write_null( byte* data, size_t len );
amf_err_t amf0_write_undefined( byte* data, size_t len );
amf_err_t amf0_write_reference( byte* data, size_t len, uint32_t value);
amf_err_t amf0_write_ecma_array( byte* data, size_t len );
amf_err_t amf0_write_object_end( byte* data, size_t len );
amf_err_t amf0_write_strict_array( byte* data, size_t len );
amf_err_t amf0_write_date( byte* data, size_t len, int timezone, double timestamp );
amf_err_t amf0_write_unsupported( byte* data, size_t len );
amf_err_t amf0_write_long_string( byte* data, size_t len, void *value, size_t value_len);
amf_err_t amf0_write_xmldocument( byte* data, size_t len, void *value, size_t value_len);
amf_err_t amf0_write_recordset( byte* data, size_t len );
amf_err_t amf0_write_typed_object( byte* data, size_t len );
amf_err_t amf0_write_continue( byte* data, size_t len, void *value, size_t value_len );



amf0_type_t amf0_next_type( const byte* data, size_t len );
amf_err_t amf0_get_number( const byte* data, size_t len, double *value );
amf_err_t amf0_get_boolean( const byte* data, size_t len, int *value );
amf_err_t amf0_get_string_length( const byte* data, size_t len, size_t *value );
amf_err_t amf0_get_string( const byte* data, size_t len, void *value, size_t value_len, size_t *continuation );
amf_err_t amf0_get_object( const byte* data, size_t len );
amf_err_t amf0_get_prop_length( const byte* data, size_t len, size_t *value );
amf_err_t amf0_get_prop_name( const byte* data, size_t len, void *value, int value_len );
amf_err_t amf0_get_movieclip( const byte* data, size_t len );
amf_err_t amf0_get_null( const byte* data, size_t len );
amf_err_t amf0_get_undefined( const byte* data, size_t len );
amf_err_t amf0_get_reference( const byte* data, size_t len, uint32_t *value);
amf_err_t amf0_get_ecma_array( const byte* data, size_t len );
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
