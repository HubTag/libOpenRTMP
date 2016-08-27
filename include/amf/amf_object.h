/*
    amf_object.h

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

#ifndef RTMP_H_AMF_OBJECT_H
#define RTMP_H_AMF_OBJECT_H

#include "amf_constants.h"
#include "rtmp/rtmp_types.h"
#include <stdarg.h>

typedef union amf_value * amf_value_t;
typedef struct amf_object * amf_t;

amf_t amf_create( char type );
amf_t amf_reference( amf_t other );

void amf_destroy( amf_t amf );

amf_err_t amf_write( amf_t amf, byte *dest, size_t size, size_t *written );
amf_err_t amf_read( amf_t amf, const byte *src, size_t size, size_t *read );
amf_err_t amf_write_value( amf_value_t value, byte *dest, size_t size );

amf_err_t amf_push_double( amf_t amf, double number );
amf_err_t amf_push_integer( amf_t amf, int32_t number );
amf_err_t amf_push_boolean( amf_t amf, char boolean );
amf_err_t amf_push_string( amf_t amf, const void *str );
amf_err_t amf_push_object_start( amf_t amf );
amf_err_t amf_push_ecma_start( amf_t amf, uint32_t assoc_members );
amf_err_t amf_push_member( amf_t amf, const void *str );
amf_err_t amf_push_null( amf_t amf );
amf_err_t amf_push_undefined( amf_t amf );
amf_err_t amf_push_unsupported( amf_t amf );
amf_err_t amf_push_reference( amf_t amf, unsigned int ref );
amf_err_t amf_push_object_end( amf_t amf );
amf_err_t amf_push_date( amf_t amf, double timestamp, char timezone );
amf_err_t amf_push_long_string( amf_t amf, const void *str );
amf_err_t amf_push_xml( amf_t amf, const void *xml );




//Used to prepare a buffer allocation for all string data, that being:
//Strings
//Long strings
//Member names
//XML documents
amf_err_t amf_push_string_alloc( amf_t amf, void** destination, size_t length );

//Return number of items in the AMF object
size_t amf_get_count( amf_t amf );

//Returns an item by index
amf_value_t amf_get_item( amf_t amf, const size_t idx );


//Returns true if the given value is of the specified type.
//Reference values will return true if the type is reference
//or if the type matches the referenced value's type.
bool amf_value_is( amf_value_t value, const amf_type_t type );

//Returns true if the type of the specified value is compatible
//with the given type. Compatibility means that the given value
//may be read as the given type.
//Compatibility is specified by the following table. Types are
//automatically compatible with themselves.
//
// Actual Type      | Compatible Types
//------------------|--------------------------------------------
// number           | date
// boolean          |
// string           | long string, xml document
// object           | typed object
// movieclip        |
// null             | number, boolean, object
// undefined        |
// reference        | based on type of referenced value
// ECMA array       | strict array
// strict array     | ECMA array
// date             | number
// long string      | string, xml document
// unsupported      |
// recordset        |
// XML document     | string, long string
// typed object     | object
//
bool amf_value_is_like( amf_value_t value, const amf_type_t type );

double amf_value_get_double( amf_value_t target );
int32_t amf_value_get_integer( amf_value_t target );
bool amf_value_get_bool( amf_value_t target );
const char* amf_value_get_string( amf_value_t target, size_t *length );
size_t amf_value_get_ref( amf_value_t target );
double amf_value_get_date( amf_value_t target, signed char *timezone );
const char* amf_value_get_xml( amf_value_t target, size_t *length );


//Get member from object by key.
amf_value_t amf_obj_get_value( amf_value_t target, const char *key );

//Get member from object by index. Key is returned if the arguments are not null.
amf_value_t amf_obj_get_value_idx( amf_value_t target, size_t idx, char **key, size_t *key_len );

//Get the number of members in object
size_t amf_obj_get_count( amf_value_t target );

//Same as with the obj functions, but with arrays
//Associative values
amf_value_t amf_arr_get_assoc_value( amf_value_t target, const char *key );
amf_value_t amf_arr_get_assoc_value_idx( amf_value_t target, size_t idx, char **key, size_t *key_len );
size_t amf_arr_get_assoc_count( const amf_value_t target );
//Ordinal values
amf_value_t amf_arr_get_ord_value( amf_value_t target, const char *key );
amf_value_t amf_arr_get_ord_value_idx( amf_value_t target, size_t idx, char **key, size_t *key_len );
size_t amf_arr_get_ord_count( const amf_value_t target );



void amf_print_value( amf_value_t val );
void amf_print( amf_t val );

#define AMF_PRIV_NULL_0() AMF_TYPE_NULL
#define AMF_PRIV_NULL_1(A) AMF_TYPE_NULL, A
#define AMF_PRIV_NULL_I(x,A,FUNC, ...)  FUNC
#define AMF_PRIV_NULL(...)  AMF_PRIV_NULL_I(__VA_ARGS__, AMF_PRIV_NULL_1(__VA_ARGS__),AMF_PRIV_NULL_0(__VA_ARGS__) )
#define AMF_NULL(...) AMF_PRIV_NULL(__VA_ARGS__)

#define AMF_ARR_ORD(...) AMF_TYPE_ECMA_ARRAY_ASSOC_END, "", __VA_ARGS__
#define AMF_ARR(...) AMF_TYPE_ARRAY, __VA_ARGS__, AMF0_TYPE_OBJECT_END, ""
#define AMF_DBL(...) AMF_TYPE_DOUBLE, __VA_ARGS__
#define AMF_INT(...) AMF_TYPE_INTEGER, __VA_ARGS__
#define AMF_STR(...) AMF_TYPE_STRING, __VA_ARGS__
#define AMF_BOOL(...) AMF_TYPE_BOOLEAN, __VA_ARGS__
#define AMF_OBJ(...) AMF_TYPE_OBJECT, __VA_ARGS__ , AMF_TYPE_OBJECT_END, ""
#define AMF(...) __VA_ARGS__ , AMF_TYPE_NONE

amf_err_t amf_push_simple_list( amf_t amf, va_list list );
amf_err_t amf_push_simple( amf_t amf, ... );

#endif
