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
#include "amf.h"
#include <stdlib.h>
#include <string.h>

struct amf_value{
    unsigned char type;
};
typedef union amf_v amf_v_t;

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

typedef struct amf_v_member amf_v_member_t;

typedef struct amf_v_object{
    unsigned char type;
    uint32_t length;
    uint32_t reserve;
    uint32_t write_offset;
    amf_v_member_t *members;
} amf_v_object_t;

union amf_v{
    amf_v_number_t number;
    amf_v_boolean_t boolean;
    amf_v_string_t string;
    amf_v_object_t object;
    amf_v_null_t null;
    amf_v_undefined_t undefined;
    amf_v_reference_t reference;
    amf_v_date_t date;
    unsigned char type;
    struct amf_value value;
};

struct amf_v_member{
    size_t length;
    char *name;
    amf_v_t value;
};

struct amf_object{
    char type;
    size_t length;
    size_t reserve;
    size_t depth;
    amf_v_t *data;

    size_t ref_reserve;
    size_t ref_table_len;
    amf_v_t **ref_table;

    void *allocation;
    size_t allocation_len;

    bool member_ready;
};


void amf_free_value( amf_v_t * val){
    switch( val->type ){
        case AMF0_TYPE_AVMPLUS:
        case AMF0_TYPE_BOOLEAN:
        case AMF0_TYPE_DATE:
        case AMF0_TYPE_MOVIECLIP:
        case AMF0_TYPE_NULL:
        case AMF0_TYPE_NUMBER:
        case AMF0_TYPE_RECORDSET:
        case AMF0_TYPE_REFERENCE:
        case AMF0_TYPE_UNDEFINED:
        case AMF0_TYPE_UNSUPPORTED:

        //Complex but not supported
        case AMF0_TYPE_ECMA_ARRAY:
        case AMF0_TYPE_STRICT_ARRAY:
        case AMF0_TYPE_TYPED_OBJECT:
            break;

        case AMF0_TYPE_STRING:
        case AMF0_TYPE_LONG_STRING:
        case AMF0_TYPE_XML_DOCUMENT:
            free( val->string.data );
            break;
        case AMF0_TYPE_OBJECT:
            for( int i = 0; i < val->object.length; ++i ){
                free( val->object.members[i].name );
                amf_free_value( &val->object.members[i].value );
            }
            free( val->object.members );
            break;
        default:
            break;
    }
}

amf_t amf_create( char type ){
    amf_t ret = calloc( 1, sizeof( struct amf_object ) );
    return ret;
}

void amf_destroy( amf_t amf ){
    for( int i = 0; i < amf->length; ++i ){
        amf_free_value( &amf->data[i] );
    }
    free( amf->data );
    free( amf->ref_table );
    free( amf->allocation );
}

amf_err_t amf_write_value( amf_value_t value, byte *dest, size_t size ){
    signed char temp_b;
    double temp_d;
    size_t temp_lu;
    char *temp_pc;
    int amt = 0;
    int amt2 = 0;
    amf_v_t *val = (amf_v_t*) value;

    switch( val->type ){
        case AMF0_TYPE_AVMPLUS:
            return amf0_write_unsupported( dest, size );
        case AMF0_TYPE_BOOLEAN:
            return amf0_write_boolean( dest, size, amf_value_get_bool( value ) );
        case AMF0_TYPE_DATE:
            temp_d = amf_value_get_date( value, &temp_b );
            return amf0_write_date( dest, size, temp_b, temp_d );
        case AMF0_TYPE_MOVIECLIP:
            return amf0_write_movieclip( dest, size );
        case AMF0_TYPE_NULL:
            return amf0_write_null( dest, size );
        case AMF0_TYPE_NUMBER:
            return amf0_write_number( dest, size, amf_value_get_number( value ) );
        case AMF0_TYPE_RECORDSET:
            return amf0_write_recordset( dest, size );
        case AMF0_TYPE_REFERENCE:
            return amf0_write_reference( dest, size, amf_value_get_ref( value ) );
        case AMF0_TYPE_UNDEFINED:
            return amf0_write_undefined( dest, size );
        case AMF0_TYPE_UNSUPPORTED:
            return amf0_write_unsupported( dest, size );

        case AMF0_TYPE_ECMA_ARRAY:
            return amf0_write_ecma_array( dest, size );
        case AMF0_TYPE_STRICT_ARRAY:
            return amf0_write_strict_array( dest, size );
        case AMF0_TYPE_TYPED_OBJECT:
            return amf0_write_typed_object( dest, size );

        case AMF0_TYPE_STRING:
            temp_pc = amf_value_get_string( value, &temp_lu );
            return amf0_write_string( dest, size, temp_pc, temp_lu );
        case AMF0_TYPE_LONG_STRING:
            temp_pc = amf_value_get_string( value, &temp_lu );
            return amf0_write_long_string( dest, size, temp_pc, temp_lu );
        case AMF0_TYPE_XML_DOCUMENT:
            temp_pc = amf_value_get_string( value, &temp_lu );
            return amf0_write_xmldocument( dest, size, temp_pc, temp_lu );
        case AMF0_TYPE_OBJECT:
            amt = amf0_write_object( dest, size );
            if( amt < 0 ){
                return amt;
            }
            if( dest ){
                dest += amt;
                size -= amt;
            }

            for( int i = 0; i < val->object.length; ++i ){
                amt2 = amf0_write_prop_name( dest, size, val->object.members[i].name, val->object.members[i].length );
                if( amt2 < 0 ){
                    return amt2;
                }
                amt += amt2;
                if( dest ){
                    dest += amt2;
                    size -= amt2;
                }
                amt2 = amf_write_value( &val->object.members[i].value.value, dest, size );
                if( amt2 < 0 ){
                    return amt2;
                }
                amt += amt2;
                if( dest ){
                    dest += amt2;
                    size -= amt2;
                }
            }
            amt2 = amf0_write_prop_name( dest, size, "", 0 );
            if( amt2 < 0 ){
                return amt2;
            }
            if( dest ){
                dest += amt2;
                size -= amt2;
            }
            amt += amt2;
            amt2 = amf0_write_object_end( dest, size );
            if( amt2 < 0 ){
                return amt2;
            }
            if( dest ){
                dest += amt2;
                size -= amt2;
            }
            amt += amt2;
            return amt;
        default:
            break;
    }
    return AMF_ERR_NONE;
}

amf_err_t amf_write( amf_t amf, byte *dest, size_t size, size_t *written ){
    size_t offset = 0;
    size_t total_len = 0;
    size_t i;
    if( written ){
        i = *written;
    }
    for( i = 0; i < amf->length; ++i ){
        if( dest && offset > size ){
            break;
        }
        int result = amf_write_value( &amf->data[i].value, dest + offset, size - offset );
        if( result < 0 ){
            goto end;
        }
        total_len += result;
        if( dest ){
            offset = total_len;
        }
    }
    end:
    if( written ){
        *written = i;
    }
    return total_len;
}

amf_err_t amf_read( amf_t amf, const byte *src, size_t size, size_t *read ){
    size_t offset = 0;
    void *buffer = nullptr;
    amf_err_t result = AMF_ERR_NONE;
    int temp_d;
    uint32_t temp_d32;
    double temp_f;
    size_t temp_size;
    while( offset < size ){
        if( amf->depth > 0 ){
            if( ( result = amf0_get_prop_length( src + offset, size - offset, &temp_size )) < 0 ){
                goto aborted;
            }
            if( (result = amf_push_string_alloc( amf, &buffer, temp_size )) < 0 ){
                goto aborted;
            }
            if( (result = amf0_get_prop_name(src + offset, size - offset, buffer, temp_size ) ) < 0 ){
                goto aborted;
            }
            offset += result;
            if( (result = amf_push_member( amf, buffer ) ) < 0 ){
                goto aborted;
            }

        }
        amf0_type_t type = amf0_next_type(src + offset, size - offset);
        result = AMF_ERR_NONE;
        switch( type ){
            case AMF0_TYPE_AVMPLUS:
                break;
            case AMF0_TYPE_BOOLEAN:
                result = amf0_get_boolean( src + offset, size - offset, &temp_d );
                break;
            case AMF0_TYPE_DATE:
                result = amf0_get_date( src + offset, size - offset, &temp_d, &temp_f );
                break;
            case AMF0_TYPE_ECMA_ARRAY:
                result = amf0_get_ecma_array( src + offset, size - offset );
                break;
            case AMF0_TYPE_LONG_STRING:
                amf0_get_long_string_length( src + offset, size - offset, &temp_size );
                break;
            case AMF0_TYPE_MOVIECLIP:
                result = amf0_get_movieclip( src + offset, size - offset );
                break;
            case AMF0_TYPE_NULL:
                result = amf0_get_null( src + offset, size - offset );
                break;
            case AMF0_TYPE_NUMBER:
                result = amf0_get_number( src + offset, size - offset, &temp_f );
                break;
            case AMF0_TYPE_OBJECT:
                result = amf0_get_object( src + offset, size - offset );
                break;
            case AMF0_TYPE_OBJECT_END:
                result = amf0_get_object_end( src + offset, size - offset );
                break;
            case AMF0_TYPE_RECORDSET:
                result = amf0_get_recordset( src + offset, size - offset );
                break;
            case AMF0_TYPE_REFERENCE:
                result = amf0_get_reference( src + offset, size - offset, &temp_d32 );
                break;
            case AMF0_TYPE_STRICT_ARRAY:
                result = amf0_get_strict_array( src + offset, size - offset );
                break;
            case AMF0_TYPE_STRING:
                amf0_get_string_length( src + offset, size - offset, &temp_size );
                break;
            case AMF0_TYPE_TYPED_OBJECT:
                result = amf0_get_typed_object( src + offset, size - offset );
                break;
            case AMF0_TYPE_UNDEFINED:
                result = amf0_get_undefined( src + offset, size - offset );
                break;
            case AMF0_TYPE_UNSUPPORTED:
                result = amf0_get_unsupported( src + offset, size - offset );
                break;
            case AMF0_TYPE_XML_DOCUMENT:
                amf0_get_xmldocument_length( src + offset, size - offset, &temp_size );
                break;
            default:
                result = AMF_ERR_INVALID_DATA;
                break;
        }
        if( result < 0 ){
            goto aborted;
        }
        offset += result;
        switch( type ){
            case AMF0_TYPE_LONG_STRING:
            case AMF0_TYPE_STRING:
            case AMF0_TYPE_XML_DOCUMENT:
                result = amf_push_string_alloc( amf, &buffer, temp_size );
                break;
            default:
                result = AMF_ERR_NONE;
                break;
        }
        if( result < 0 ){
            goto aborted;
        }
        offset += result;
        switch( type ){
            case AMF0_TYPE_LONG_STRING:
                result = amf0_get_long_string( src + offset, size - offset, buffer, temp_size, nullptr );
                break;
            case AMF0_TYPE_STRING:
                result = amf0_get_string( src + offset, size - offset, buffer, temp_size, nullptr );
                break;
            case AMF0_TYPE_XML_DOCUMENT:
                result = amf0_get_xmldocument( src + offset, size - offset, buffer, temp_size, nullptr );
                break;
            default:
                result = AMF_ERR_NONE;
                break;
        }
        if( result < 0 ){
            goto aborted;
        }
        offset += result;
        switch( type ){
            case AMF0_TYPE_AVMPLUS:
                break;
            case AMF0_TYPE_BOOLEAN:
                result = amf_push_boolean( amf, temp_d );
                break;
            case AMF0_TYPE_DATE:
                result = amf_push_date( amf, temp_f, temp_d );
                break;
            case AMF0_TYPE_ECMA_ARRAY:
                result = AMF_ERR_INVALID_DATA;
                break;
            case AMF0_TYPE_LONG_STRING:
                result = amf_push_long_string( amf, buffer );
                break;
            case AMF0_TYPE_MOVIECLIP:
                result = AMF_ERR_INVALID_DATA;
                break;
            case AMF0_TYPE_NULL:
                result = amf_push_null( amf );
                break;
            case AMF0_TYPE_NUMBER:
                result = amf_push_number( amf, temp_f );
                break;
            case AMF0_TYPE_OBJECT:
                result = amf_push_object_start( amf );
                break;
            case AMF0_TYPE_OBJECT_END:
                result = amf_push_object_end( amf );
                break;
            case AMF0_TYPE_RECORDSET:
                result = AMF_ERR_INVALID_DATA;
                break;
            case AMF0_TYPE_REFERENCE:
                result = amf_push_reference( amf, temp_d32 );
                break;
            case AMF0_TYPE_STRICT_ARRAY:
                result = AMF_ERR_INVALID_DATA;
                break;
            case AMF0_TYPE_STRING:
                result = amf_push_string( amf, buffer );
                break;
            case AMF0_TYPE_TYPED_OBJECT:
                result = AMF_ERR_INVALID_DATA;
                break;
            case AMF0_TYPE_UNDEFINED:
                result = amf_push_undefined( amf );
                break;
            case AMF0_TYPE_UNSUPPORTED:
                result = amf_push_unsupported( amf );
                break;
            case AMF0_TYPE_XML_DOCUMENT:
                result = amf_push_xml( amf, buffer );
                break;
            default:
                result = AMF_ERR_INVALID_DATA;
                break;
        }
        if( result < 0 ){
            goto aborted;
        }
        offset += result;
    }
    return offset;

    aborted:
    if( read ){
        *read = offset;
    }
    return result;
}

static amf_v_t * amf_v_get_object( amf_t amf ){
    if( amf->length > 0 ){
        size_t depth = 1;
        amf_v_t *obj = amf->data + amf->length - 1;
        while( amf_value_is( &obj->value, AMF0_TYPE_OBJECT ) && obj->object.length > 0 && depth < amf->depth ){
            obj = &obj->object.members[obj->object.length-1].value;
            ++depth;
        }
        if( depth != amf->depth ){
            return nullptr;
        }
        if( amf_value_is( &obj->value, AMF0_TYPE_OBJECT ) ){
            return obj;
        }
    }
    return nullptr;
}

static amf_v_member_t * amf_v_push_member( amf_t amf ){
    amf_v_t* obj = amf_v_get_object( amf );
    if( obj ){
        if( obj->object.length + 1 >= obj->object.reserve ){
            obj->object.reserve ++;
            obj->object.reserve *= 2;
            void* data = realloc( obj->object.members, obj->object.reserve * sizeof( amf_v_member_t ) );
            if( !data ){
                return nullptr;
            }
            obj->object.members = data;
        }
        return obj->object.members + obj->object.length++;
    }
    return nullptr;
}

static amf_v_t *amf_push_item( amf_t amf ){
    if( amf->depth == 0 ){
        if( amf->length + 1 >= amf->reserve ){
            amf->reserve += 1;
            amf->reserve *= 2;
            void *data = realloc( amf->data, amf->reserve * sizeof( amf_v_t ) );
            if( !data ){
                return nullptr;
            }
            amf->data = data;
        }
        return amf->data + amf->length++;
    }
    else if( amf->length > 0 ){
        amf_v_t* obj = amf_v_get_object( amf );
        if( obj ){
            return &obj->object.members[obj->object.length - 1].value;
        }
    }
    return nullptr;
}

#define PUSH_PREP(a,name) \
    if( a->depth > 0 && !a->member_ready ){\
        return AMF_ERR_NEED_NAME;\
    }\
    a->member_ready = false;\
    amf_v_t * name = amf_push_item( a );\
    if( !name ){\
        return AMF_ERR_OOM;\
    }

#define PUSH_STR(a,str,t) \
    {\
        if( str != a->allocation ){\
            return AMF_ERR_BAD_ALLOC;\
        }\
        PUSH_PREP(a,target);\
        target->string.data = a->allocation;\
        target->string.length = a->allocation_len;\
        target->string.type = t;\
        a->allocation = nullptr;\
        a->allocation_len = 0;\
        return AMF_ERR_NONE;\
    }

amf_err_t amf_push_number( amf_t amf, double number ){
    PUSH_PREP( amf, target );
    target->number.number = number;
    target->number.type = AMF0_TYPE_NUMBER;
    return AMF_ERR_NONE;
}

amf_err_t amf_push_boolean( amf_t amf, char boolean ){
    PUSH_PREP( amf, target );
    target->boolean.boolean = boolean;
    target->boolean.type = AMF0_TYPE_BOOLEAN;
    return AMF_ERR_NONE;
}
amf_err_t amf_push_string_alloc( amf_t amf, void** destination, size_t length ){
    if( amf->allocation ){
        free( amf->allocation );
    }
    amf->allocation = malloc( length * sizeof( char ) );
    if( amf->allocation == nullptr ){
        return AMF_ERR_OOM;
    }
    amf->allocation_len = length;
    *destination = amf->allocation;
    return AMF_ERR_NONE;
}
amf_err_t amf_push_string( amf_t amf, void *str ){
    PUSH_STR( amf, str, AMF0_TYPE_STRING );
}
amf_err_t amf_push_object_start( amf_t amf ){
    PUSH_PREP( amf, target );
    amf->depth ++;
    target->object.type = AMF0_TYPE_OBJECT;
    target->object.length = 0;
    target->object.reserve = 0;
    target->object.write_offset = 0;
    target->object.members = nullptr;

    if( amf->ref_table_len + 1 > amf->ref_reserve ){
        amf->ref_reserve++;
        amf->ref_reserve *= 2;
        void* data = realloc( amf->ref_table, amf->ref_reserve * sizeof( amf_v_t* ) );
        if( !data ){
            return AMF_ERR_OOM;
        }
        amf->ref_table = data;
    }
    amf->ref_table[ amf->ref_table_len++ ] = target;
    return AMF_ERR_NONE;
}
amf_err_t amf_push_member( amf_t amf, void *str ){
    if( amf->member_ready ){
        return AMF_ERR_INCOMPLETE;
    }
    amf_v_member_t* mem = amf_v_push_member( amf );
    if( mem ){
        mem->length = amf->allocation_len;
        mem->name = amf->allocation;
        amf->allocation_len = 0;
        amf->allocation = nullptr;
        amf->member_ready = true;
        return AMF_ERR_NONE;
    }
    return AMF_ERR_OOM;
}
amf_err_t amf_push_null( amf_t amf ){
    PUSH_PREP( amf, target );
    target->null.type = AMF0_TYPE_NULL;
    return AMF_ERR_NONE;
}
amf_err_t amf_push_undefined( amf_t amf ){
    PUSH_PREP( amf, target );
    target->undefined.type = AMF0_TYPE_UNDEFINED;
    return AMF_ERR_NONE;
}
amf_err_t amf_push_unsupported( amf_t amf ){
    PUSH_PREP( amf, target );
    target->undefined.type = AMF0_TYPE_UNSUPPORTED;
    return AMF_ERR_NONE;
}
amf_err_t amf_push_reference( amf_t amf, unsigned int ref ){
    PUSH_PREP( amf, target );
    target->reference.ref = nullptr;
    target->reference.ref_num = ref;
    target->reference.type = AMF0_TYPE_REFERENCE;
    if( ref < amf->ref_table_len ){
        target->reference.ref = amf->ref_table[ref];
    }
    return AMF_ERR_NONE;
}
amf_err_t amf_push_object_end( amf_t amf ){
    if( amf->member_ready ){
        amf->member_ready = false;
        amf_v_t* obj = amf_v_get_object( amf );
        if( obj && obj->object.length > 0 ){
            obj->object.length--;
            free( obj->object.members[obj->object.length].name );
        }
    }
    if( amf->depth > 0 ){
        amf->depth--;
        return AMF_ERR_NONE;
    }
    return AMF_ERR_INVALID_DATA;
}
amf_err_t amf_push_date( amf_t amf, double timestamp, char timezone ){
    PUSH_PREP( amf, target );
    target->date.timestamp = timestamp;
    target->date.timezone = timezone;
    target->string.type = AMF0_TYPE_DATE;
    return AMF_ERR_NONE;
}
amf_err_t amf_push_long_string( amf_t amf, void *str ){
    PUSH_STR( amf, str, AMF0_TYPE_LONG_STRING );
}
amf_err_t amf_push_xml( amf_t amf, void *xml ){
    PUSH_STR( amf, xml, AMF0_TYPE_XML_DOCUMENT );
}

size_t amf_get_count( amf_t amf ){
    return amf->length;
}
amf_value_t amf_get_item( amf_t amf, size_t idx ){
    return (amf_value_t) (amf->data + idx);
}
bool amf_value_is( amf_value_t value, amf0_type_t type ){
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

bool amf_value_is_like( amf_value_t value, amf0_type_t type ){
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
            default: break;
        }
        case AMF0_TYPE_NUMBER:
        case AMF0_TYPE_DATE:
        switch( type ){
            case AMF0_TYPE_NUMBER:
            case AMF0_TYPE_DATE:
            return true;
            default: break;
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
            default: break;
        }
        break;

        //Objects
        case AMF0_TYPE_OBJECT:
        case AMF0_TYPE_TYPED_OBJECT:
        switch( type ){
            case AMF0_TYPE_OBJECT:
            case AMF0_TYPE_TYPED_OBJECT:
            return true;
            default: break;
        }
        break;

        //Arrays
        case AMF0_TYPE_ECMA_ARRAY:
        case AMF0_TYPE_STRICT_ARRAY:
        switch( type ){
            case AMF0_TYPE_ECMA_ARRAY:
            case AMF0_TYPE_STRICT_ARRAY:
            return true;
            default: break;
        }
        default:
        break;
    }
    return false;
}

double amf_value_get_number( amf_value_t target ){
    const amf_v_t *v = (const amf_v_t*) target;
    return v->number.number;
}

bool amf_value_get_bool( amf_value_t target ){
    const amf_v_t *v = (const amf_v_t*) target;
    return v->boolean.boolean;
}

char* amf_value_get_string( amf_value_t target, size_t *length ){
    const amf_v_t *v = (const amf_v_t*) target;
    if( length ){
        *length = v->string.length;
    }
    return v->string.data;
}

size_t amf_value_get_ref( amf_value_t target ){
    const amf_v_t *v = (const amf_v_t*) target;
    return v->reference.ref_num;
}

double amf_value_get_date( amf_value_t target, signed char *timezone ){
    const amf_v_t *v = (const amf_v_t*) target;
    if( timezone ){
        *timezone = v->date.timezone;
    }
    return v->date.timestamp;
}

char* amf_value_get_xml( amf_value_t target, size_t *length ){
    const amf_v_t *v = (const amf_v_t*) target;
    if( length ){
        *length = v->string.length;
    }
    return v->string.data;
}


amf_value_t amf_obj_get_value( amf_value_t target, const char *key ){
    const amf_v_t *v = (const amf_v_t*) target;
    for( int i = 0; i < v->object.length; ++i ){
        if( memcmp( key, v->object.members[i].name, v->object.members[i].length ) == 0 ){
            return &v->object.members[i].value.value;
        }
    }
    return nullptr;
}
amf_value_t amf_obj_get_value_idx( amf_value_t target, size_t idx, char **key, size_t *key_len ){
    amf_v_t *v = (amf_v_t*) target;
    if( v->type == AMF0_TYPE_REFERENCE ){
        v = v->reference.ref;
    }
    if( idx >= v->object.length ){
        return nullptr;
    }
    if( key ){
        *key = v->object.members[idx].name;
    }
    if( key_len ){
        *key_len = v->object.members[idx].length;
    }
    return &v->object.members[idx].value.value;
}

size_t amf_obj_get_count( amf_value_t target ){
    const amf_v_t *v = (const amf_v_t*) target;
    return v->object.length;
}

#include <stdio.h>

void amf_print_value_internal( amf_v_t * val, int depth ){

    switch( val->type ){
        case AMF0_TYPE_AVMPLUS:
            break;
        case AMF0_TYPE_BOOLEAN:
            printf( "Boolean: %s", val->boolean.boolean ? "true" : "false" );
            break;
        case AMF0_TYPE_DATE:
            printf( "Date: %f +%d", val->date.timestamp, val->date.timezone );
            break;
        case AMF0_TYPE_MOVIECLIP:
            printf( "Movie clip" );
            break;
        case AMF0_TYPE_NULL:
            printf("Null");
            break;
        case AMF0_TYPE_NUMBER:
            printf( "Number: %f", val->number.number );
            break;
        case AMF0_TYPE_RECORDSET:
            printf( "Recordset");
            break;
        case AMF0_TYPE_REFERENCE:
            printf( "Reference to %lu", val->reference.ref_num );
            break;
        case AMF0_TYPE_UNDEFINED:
            printf( "Undefined");
            break;
        case AMF0_TYPE_UNSUPPORTED:
            printf( "Unsupported");
            break;

        case AMF0_TYPE_ECMA_ARRAY:
            printf( "ECMA array");
            break;
        case AMF0_TYPE_STRICT_ARRAY:
            printf( "Strict array");
            break;
        case AMF0_TYPE_TYPED_OBJECT:
            printf( "Typed Object");
            break;

        case AMF0_TYPE_STRING:
            printf( "String: %s", val->string.data );
            break;
        case AMF0_TYPE_LONG_STRING:
            printf( "Long String: %s", val->string.data );
            break;
        case AMF0_TYPE_XML_DOCUMENT:
            printf( "XML document: %s", val->string.data );
            break;
        case AMF0_TYPE_OBJECT:
            printf( "Object: {\n");
            for( int i = 0; i < val->object.length; ++i ){
                for( int i = 0; i <= depth; ++i ){
                    printf("    ");
                }
                printf( "%.*s: ", (int) val->object.members[i].length, val->object.members[i].name );
                amf_print_value_internal( &val->object.members[i].value, depth+1 );
            }
            for( int i = 0; i < depth; ++i ){
                printf("    ");
            }
            printf( "}");
            break;
        default:
            break;
    }
    printf("\n");
}
void amf_print_value( amf_value_t val ){
    return amf_print_value_internal( (amf_v_t*) val, 0 );
}

void amf_print( amf_t amf ){
    for( int i = 0; i < amf_get_count( amf ); ++i ){
        amf_print_value( amf_get_item( amf, i ) );
    }
}
