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

#include "amf_object.h"

struct amf_value{
    unsigned char type;
};

typedef struct amf_v_number{
    unsigned char type;
    double number;
} amf_v_number_t;

typedef struct amf_v_boolean{
    unsigned char type;
    bool boolean;
} amf_v_boolean_t;

typedef struct amf_v_string{
    unsigned char type;
    size_t length;
    char *data;
} amf_v_string_t;

typedef struct amf_v_member{
    size_t length;
    char *name;
    amf_value *value;
} amf_v_member_t;

typedef struct amf_v_object{
    unsigned char type;
    size_t length;
    amf_v_member_t *members;
} amf_v_object_t;

typedef struct amf_v_null{
    unsigned char type;
} amf_v_null_t;

typedef struct amf_v_undefined{
    unsigned char type;
} amf_v_undefined_t;

typedef struct amf_v_reference{
    unsigned char type;
    size_t ref_num;
    amf_v_t *ref;
} amf_v_reference_t;

typedef struct amf_v_date{
    unsigned char type;
    double timestamp;
    signed char timezone;
} amf_v_date_t;

typedef union {
    amf_v_number_t number;
    amf_v_boolean_t boolean;
    amf_v_string_t string;
    amf_v_object_t object;
    amf_v_null_t null;
    amf_v_undefined_t undefined;
    amf_v_reference_t reference;
    amf_v_date_t date;
    unsigned char type;
} amf_v_t;

struct amf_object{
    char type;
    size_t length;
    size_t depth;
    amf_v_t *data;

    size_t ref_table_len;
    amf_v_t *ref_table;
};

amf_t amf_create( char type ){

}

void amf_destroy( amf_t amf ){

}

amf_err_t amf_push_number( amf_t amf, double number ){

}
amf_err_t amf_push_boolean( amf_t amf, char boolean ){

}
amf_err_t amf_push_string_alloc( amf_t amf, void** destination, size_t length ){

}
amf_err_t amf_push_string( amf_t amf, void *str ){

}
amf_err_t amf_push_object_start( amf_t amf ){

}
amf_err_t amf_push_member( amf_t amf, void *str ){

}
amf_err_t amf_push_null( amf_t amf ){

}
amf_err_t amf_push_undefined( amf_t amf ){

}
amf_err_t amf_push_reference( amf_t amf, unsigned int ref ){

}
amf_err_t amf_push_object_end( amf_t amf ){

}
amf_err_t amf_push_date( amf_t amf, double timestamp, char timezone ){

}
amf_err_t amf_push_long_string( amf_t amf, void *str ){

}
amf_err_t amf_push_xml( amf_t amf, void *xml ){

}

size_t amf_get_count( amf_t amf ){

}
amf_value_t amf_get_item( amf_t amf, size_t idx ){

}
bool amf_value_is( amf_value_t value, amf_type_t type ){
    if( type == value->type ){
        return true;
    }
    if( value->type == AMF0_TYPE_REFERENCE ){
        amf_v_reference_t *ref = (amf_v_reference_t *) value;
        if( ref->ref->type == type ){
            return true;
        }
    }
    return false;
}

bool amf_value_is_like( amf_value_t value, amf_type_t type ){
    if( amf_value_is( value, type ) ){
        return true;
    }
    if( value->type == AMF0_TYPE_REFERENCE ){
        amf_v_reference_t *ref = (amf_v_reference_t *) value;
        value = (amf_value_t)ref->ref;
    }
    switch( value->type ){
        //Numerics
        case AMF0_TYPE_NULL:
        switch( type ){
            case AMF0_TYPE_OBJECT:
            case AMF0_TYPE_TYPED_OBJECT:
            return true;
        }
        case AMF0_TYPE_NUMBER:
        case AMF0_TYPE_DATE:
        case AMF0_TYPE_BOOLEAN:
        switch( type ){
            case AMF0_TYPE_BOOLEAN:
            case AMF0_TYPE_DATE:
            return true;
        }
        break;

        //Strings
        case AMF0_TYPE_STRING:
        case AMF0_TYPE_LONG_STRING:
        case AMF0_TYPE_XML_DOCUMENT:
        switch( type ){
            case AMF0_TYPE_STRING:
            case AMF0_TYPE_LONG_STRING:
            case AMF0_TYPE_XML_DOCUMENT:
            return true;
        }
        break;

        //Objects
        case AMF0_TYPE_OBJECT:
        case AMF0_TYPE_TYPED_OBJECT:
        switch( type ){
            case AMF0_TYPE_OBJECT:
            case AMF0_TYPE_TYPED_OBJECT:
            return true;
        }
        break;

        //Arrays
        case AMF0_TYPE_ECMA_ARRAY:
        case AMF0_TYPE_STRICT_ARRAY:
        switch( type ){
            case AMF0_TYPE_ECMA_ARRAY:
            case AMF0_TYPE_STRICT_ARRAY:
            return true;
        }
        break;
    }
    return false;
}

amf_err_t amf_value_get_number( amf_value_t target, double *value ){

}
amf_err_t amf_value_get_bool( amf_value_t target, bool *value ){

}
amf_err_t amf_value_get_string( amf_value_t target, char** value, size_t *length ){

}
amf_err_t amf_value_get_object( amf_value_t target, amf_value_t *value ){

}
amf_err_t amf_value_get_ref( amf_value_t target, size_t *ref ){

}
amf_err_t amf_value_get_date( amf_value_t target, double *timestamp, signed char *timezone ){

}
amf_err_t amf_value_get_xml( amf_value_t target, char** value, size_t *length ){

}

amf_err_t amf_obj_get_value( amf_value_t target, const char *key, amf_value_t *value ){

}
amf_err_t amf_obj_get_value_idx( amf_value_t target, size_t idx, char **key, size_t *key_len, amf_value_t *value ){

}
size_t amf_obj_get_count( amf_value_t target ){

}
