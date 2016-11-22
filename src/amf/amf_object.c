/*
    amf_object.c

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

#include <openrtmp/amf/amf_object.h>
#include <openrtmp/amf/amf.h>
#include <openrtmp/util/memutil.h>
#include <openrtmp/util/vec.h>
#include <stdlib.h>
#include <string.h>


typedef struct amf_v_dbl{
    amf_type_t type;
    double number;
} amf_v_double_t;

typedef struct amf_v_int{
    amf_type_t type;
    int32_t number;
} amf_v_int_t;

typedef struct amf_v_boolean{
    amf_type_t type;
    bool boolean;
} amf_v_boolean_t;

typedef struct amf_v_string{
    amf_type_t type;
    size_t length;
    char *data;
} amf_v_string_t;



typedef struct amf_v_null{
    amf_type_t type;
} amf_v_null_t;

typedef struct amf_v_undefined{
    amf_type_t type;
} amf_v_undefined_t;

typedef struct amf_v_reference{
    amf_type_t type;
    size_t ref_num;
    amf_value_t ref;
} amf_v_reference_t;

typedef struct amf_v_date{
    amf_type_t type;
    double timestamp;
    int16_t timezone;
} amf_v_date_t;

typedef struct amf_v_member amf_v_member_t;

typedef struct amf_v_assoc {
    amf_type_t type;
    VEC_DECLARE(amf_v_member_t) members;
} amf_v_assoc_t;

typedef struct amf_v_object{
    // Must match amf_v_assoc_t
    amf_type_t type;
    VEC_DECLARE(amf_v_member_t) members;
} amf_v_object_t;

typedef struct amf_v_array{
    // Must match amf_v_assoc_t
    amf_type_t type;
    VEC_DECLARE(amf_v_member_t) assoc;
    uint32_t assoc_len;
    VEC_DECLARE(amf_v_member_t) ordinal;
} amf_v_array_t;

union amf_value{
    amf_v_double_t dbl;
    amf_v_int_t integer;
    amf_v_boolean_t boolean;
    amf_v_string_t string;
    amf_v_object_t object;
    amf_v_null_t null;
    amf_v_undefined_t undefined;
    amf_v_reference_t reference;
    amf_v_date_t date;
    amf_v_array_t array;
    amf_v_assoc_t associative;
    amf_type_t type;
};

struct amf_v_member{
    size_t length;
    char *name;
    union amf_value value;
};

struct amf_object{
    amf_type_t type;
    size_t references;
    size_t depth;
    VEC_DECLARE(union amf_value) items;

    VEC_DECLARE(amf_value_t) ref_table;

    void *allocation;
    size_t allocation_len;

    bool member_ready;
};

void amf_free_value( amf_value_t val);

static void amf_free_member( amf_v_member_t member ){
    free( member.name );
    amf_free_value( &member.value );
}

void amf_free_value( amf_value_t val){
    switch( val->type ){
        case AMF_TYPE_AVMPLUS:
        case AMF_TYPE_BOOLEAN:
        case AMF_TYPE_DATE:
        case AMF_TYPE_MOVIECLIP:
        case AMF_TYPE_NULL:
        case AMF_TYPE_DOUBLE:
        case AMF_TYPE_INTEGER:
        case AMF_TYPE_RECORDSET:
        case AMF_TYPE_REFERENCE:
        case AMF_TYPE_UNDEFINED:
        case AMF_TYPE_UNSUPPORTED:

        //Complex but not supported
        case AMF_TYPE_TYPED_OBJECT:
            break;

        case AMF_TYPE_STRING:
        case AMF_TYPE_XML_DOCUMENT:
            free( val->string.data );
            break;
        case AMF_TYPE_OBJECT:
            VEC_DESTROY_DTOR( val->object.members, amf_free_member );
            break;
        case AMF_TYPE_ARRAY:
            VEC_DESTROY_DTOR( val->array.assoc, amf_free_member );
            VEC_DESTROY_DTOR( val->array.ordinal, amf_free_member );
            break;
        default:
            break;
    }
}


amf_t amf_create( char type ){
    amf_t ret = ezalloc( ret );
    if( !ret ){
        return ret;
    }
    VEC_INIT(ret->items);
    VEC_INIT(ret->ref_table);
    if( !ret->items ){
        free(ret);
        return nullptr;
    }
    if( !ret->ref_table ){
        VEC_DESTROY( ret->items );
        free(ret);
        return nullptr;
    }

    return ret;
}

amf_t amf_reference( amf_t other ){
    other->references++;
    return other;
}

void amf_destroy( amf_t amf ){
    if( !amf ){
        return;
    }
    if( amf->references == 0 ){
        for( size_t i = 0; i < VEC_SIZE(amf->items); ++i ){
            amf_free_value( &amf->items[i] );
        }
        VEC_DESTROY( amf->items );
        VEC_DESTROY( amf->ref_table );
        free( amf->allocation );
        free( amf );
    }
    else{
        amf->references --;
    }
}


amf_err_t amf_write_value( amf_value_t value, byte *dest, size_t size );

#define DO(a) do{                       \
    int wrote_internal = (a);           \
    if( wrote_internal < 0 ){           \
        return AMF_SIZE(wrote_internal);\
    }                                   \
    wrote += wrote_internal;            \
    if( dest ){                         \
        dest += wrote_internal;         \
        size -= wrote_internal;         \
    }                                   \
}while(0)

static amf_err_t amf_write_value_obj(amf_value_t val, byte *dest, size_t size){
    int wrote = 0;
    DO(amf0_write_object( dest, size ));
    for( size_t i = 0; i < VEC_SIZE(val->object.members); ++i ){
        DO( amf0_write_prop_name( dest, size, val->object.members[i].name, val->object.members[i].length ) );
        DO( amf_write_value( &VEC_AT(val->object.members, i).value, dest, size ) );
    }
    DO(amf0_write_prop_name( dest, size, "", 0 ));
    DO(amf0_write_object_end( dest, size ));
    return AMF_SIZE(wrote);
}

static amf_err_t amf_write_value_ecma(amf_value_t val, byte *dest, size_t size){
    int wrote = 0;
    DO( amf0_write_ecma_array( dest, size, VEC_SIZE( val->array.assoc ) ) );
    for( size_t i = 0; i < VEC_SIZE(val->array.assoc); ++i ){
        DO( amf0_write_prop_name( dest, size, val->array.assoc[i].name, val->array.assoc[i].length ) );
        DO( amf_write_value( &VEC_AT(val->array.assoc, i).value, dest, size ) );
    }
    for( size_t i = 0; i < VEC_SIZE(val->array.ordinal); ++i ){
        DO( amf0_write_prop_name( dest, size, val->array.ordinal[i].name, val->array.ordinal[i].length ) );
        DO( amf_write_value( &VEC_AT(val->array.ordinal, i).value, dest, size ) );
    }
    DO(amf0_write_prop_name( dest, size, "", 0 ));
    DO(amf0_write_object_end( dest, size ));
    return AMF_SIZE(wrote);
}


#undef DO

amf_err_t amf_write_value( amf_value_t value, byte *dest, size_t size ){
    double temp_d;
    size_t temp_lu;
    int16_t temp_s;
    const char *temp_pc;



    switch( value->type ){
        case AMF_TYPE_AVMPLUS:
            return amf0_write_unsupported( dest, size );
        case AMF_TYPE_BOOLEAN:
            return amf0_write_boolean( dest, size, amf_value_get_bool( value ) );
        case AMF_TYPE_DATE:
            temp_d = amf_value_get_date( value, &temp_s );
            return amf0_write_date( dest, size, temp_s, temp_d );
        case AMF_TYPE_MOVIECLIP:
            return amf0_write_movieclip( dest, size );
        case AMF_TYPE_NULL:
            return amf0_write_null( dest, size );
        case AMF_TYPE_DOUBLE:
            return amf0_write_number( dest, size, amf_value_get_double( value ) );
        case AMF_TYPE_INTEGER:
            return amf0_write_number( dest, size, amf_value_get_integer( value ) );
        case AMF_TYPE_RECORDSET:
            return amf0_write_recordset( dest, size );
        case AMF_TYPE_REFERENCE:
            return amf0_write_reference( dest, size, amf_value_get_ref( value ) );
        case AMF_TYPE_UNDEFINED:
            return amf0_write_undefined( dest, size );
        case AMF_TYPE_UNSUPPORTED:
            return amf0_write_unsupported( dest, size );

        case AMF_TYPE_TYPED_OBJECT:
            return amf0_write_typed_object( dest, size );

        case AMF_TYPE_STRING:
            temp_pc = amf_value_get_string( value, &temp_lu );
            return amf0_write_string( dest, size, temp_pc, temp_lu );
        case AMF_TYPE_XML_DOCUMENT:
            temp_pc = amf_value_get_string( value, &temp_lu );
            return amf0_write_xmldocument( dest, size, temp_pc, temp_lu );
        case AMF_TYPE_ARRAY:
            return amf_write_value_ecma( value, dest, size );
        case AMF_TYPE_OBJECT:
            return amf_write_value_obj( value, dest, size );
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
    for( i = 0; i < VEC_SIZE(amf->items); ++i ){
        if( dest && offset > size ){
            break;
        }
        int result = amf_write_value( &amf->items[i], dest + offset, size - offset );
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
    return AMF_SIZE(total_len);
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
            result = result < 0 ? result : amf0_get_prop_length( src + offset, size - offset, &temp_size );
            result = result < 0 ? result : amf_push_string_alloc( amf, &buffer, temp_size );
            result = result < 0 ? result : amf0_get_prop_name(src + offset, size - offset, buffer, temp_size );
            offset += result < 0 ? 0 : result;
            result = result < 0 ? result : amf_push_member( amf, buffer );
            if( result < 0 ){
                goto aborted;
            }
        }
        amf0_type_t type = amf0_next_type(src + offset, size - offset);
        result = AMF_ERR_NONE;
        switch( type ){
            case AMF0_TYPE_AVMPLUS:                                                                                             break;
            case AMF0_TYPE_BOOLEAN:         result =    amf0_get_boolean( src + offset, size - offset, &temp_d );               break;
            case AMF0_TYPE_DATE:            result =    amf0_get_date( src + offset, size - offset, &temp_d, &temp_f );         break;
            case AMF0_TYPE_ECMA_ARRAY:      result =    amf0_get_ecma_array( src + offset, size - offset, &temp_d32 );          break;
            case AMF0_TYPE_LONG_STRING:                 amf0_get_long_string_length( src + offset, size - offset, &temp_size ); break;
            case AMF0_TYPE_MOVIECLIP:       result =    amf0_get_movieclip( src + offset, size - offset );                      break;
            case AMF0_TYPE_NULL:            result =    amf0_get_null( src + offset, size - offset );                           break;
            case AMF0_TYPE_NUMBER:          result =    amf0_get_number( src + offset, size - offset, &temp_f );                break;
            case AMF0_TYPE_OBJECT:          result =    amf0_get_object( src + offset, size - offset );                         break;
            case AMF0_TYPE_OBJECT_END:      result =    amf0_get_object_end( src + offset, size - offset );                     break;
            case AMF0_TYPE_RECORDSET:       result =    amf0_get_recordset( src + offset, size - offset );                      break;
            case AMF0_TYPE_REFERENCE:       result =    amf0_get_reference( src + offset, size - offset, &temp_d32 );           break;
            case AMF0_TYPE_STRICT_ARRAY:    result =    amf0_get_strict_array( src + offset, size - offset );                   break;
            case AMF0_TYPE_STRING:                      amf0_get_string_length( src + offset, size - offset, &temp_size );      break;
            case AMF0_TYPE_TYPED_OBJECT:    result =    amf0_get_typed_object( src + offset, size - offset );                   break;
            case AMF0_TYPE_UNDEFINED:       result =    amf0_get_undefined( src + offset, size - offset );                      break;
            case AMF0_TYPE_UNSUPPORTED:     result =    amf0_get_unsupported( src + offset, size - offset );                    break;
            case AMF0_TYPE_XML_DOCUMENT:                amf0_get_xmldocument_length( src + offset, size - offset, &temp_size ); break;
            default:                        result =    AMF_ERR_INVALID_DATA;                                                   break;
        }
        if( result < 0 ){
            goto aborted;
        }
        offset += result;
        switch( type ){
            case AMF0_TYPE_LONG_STRING:
            case AMF0_TYPE_STRING:
            case AMF0_TYPE_XML_DOCUMENT:    result = amf_push_string_alloc( amf, &buffer, temp_size );  break;
            default:                        result = AMF_ERR_NONE;                                      break;
        }
        if( result < 0 ){
            goto aborted;
        }
        offset += result;
        switch( type ){
            case AMF0_TYPE_LONG_STRING:     result = amf0_get_long_string( src + offset, size - offset, buffer, temp_size );            break;
            case AMF0_TYPE_STRING:          result = amf0_get_string( src + offset, size - offset, buffer, temp_size );                 break;
            case AMF0_TYPE_XML_DOCUMENT:    result = amf0_get_xmldocument( src + offset, size - offset, buffer, temp_size );            break;
            default:                        result = AMF_ERR_NONE;                                                                      break;
        }
        if( result < 0 ){
            goto aborted;
        }
        offset += result;
        switch( type ){
            case AMF0_TYPE_AVMPLUS:                                                         break;
            case AMF0_TYPE_BOOLEAN:         result = amf_push_boolean( amf, temp_d );       break;
            case AMF0_TYPE_DATE:            result = amf_push_date( amf, temp_f, temp_d );  break;
            case AMF0_TYPE_ECMA_ARRAY:      result = amf_push_ecma_start( amf, temp_d32 );  break;
            case AMF0_TYPE_LONG_STRING:     result = amf_push_long_string( amf, buffer );   break;
            case AMF0_TYPE_MOVIECLIP:       result = AMF_ERR_INVALID_DATA;                  break;
            case AMF0_TYPE_NULL:            result = amf_push_null( amf );                  break;
            case AMF0_TYPE_NUMBER:          result = amf_push_double( amf, temp_f );        break;
            case AMF0_TYPE_OBJECT:          result = amf_push_object_start( amf );          break;
            case AMF0_TYPE_OBJECT_END:      result = amf_push_object_end( amf );            break;
            case AMF0_TYPE_RECORDSET:       result = AMF_ERR_INVALID_DATA;                  break;
            case AMF0_TYPE_REFERENCE:       result = amf_push_reference( amf, temp_d32 );   break;
            case AMF0_TYPE_STRICT_ARRAY:    result = AMF_ERR_INVALID_DATA;                  break;
            case AMF0_TYPE_STRING:          result = amf_push_string( amf, buffer );        break;
            case AMF0_TYPE_TYPED_OBJECT:    result = AMF_ERR_INVALID_DATA;                  break;
            case AMF0_TYPE_UNDEFINED:       result = amf_push_undefined( amf );             break;
            case AMF0_TYPE_UNSUPPORTED:     result = amf_push_unsupported( amf );           break;
            case AMF0_TYPE_XML_DOCUMENT:    result = amf_push_xml( amf, buffer );           break;
            default:                        result = AMF_ERR_INVALID_DATA;                  break;
        }
        if( result < 0 ){
            goto aborted;
        }
        offset += result;
    }
    return AMF_SIZE(offset);

    aborted:
    if( read ){
        *read = offset;
    }
    return AMF_SIZE(result);
}

static amf_value_t amf_v_get_object( amf_t amf ){
    if( VEC_SIZE(amf->items) > 0 ){
        size_t depth = 1;
        amf_value_t obj = &VEC_BACK(amf->items);
        while( amf_value_is( obj, AMF_TYPE_COMPLEX ) && depth < amf->depth ){
            if( amf_value_is(obj, AMF_TYPE_OBJECT ) ){
                if( VEC_SIZE(obj->object.members) == 0 ){
                    break;
                }
                obj = &VEC_BACK(obj->object.members).value;
                ++depth;
            }
            if( amf_value_is(obj, AMF_TYPE_ARRAY ) ){
                size_t size = VEC_SIZE(obj->array.ordinal) + VEC_SIZE(obj->array.assoc);
                if( size == 0 ){
                    break;
                }
                if( size <= obj->array.assoc_len ){
                    obj = &VEC_BACK(obj->array.assoc).value;
                }
                else{
                    obj = &VEC_BACK(obj->array.ordinal).value;
                }
                ++depth;
            }

        }
        if( depth != amf->depth ){
            return nullptr;
        }
        if( amf_value_is( obj, AMF_TYPE_COMPLEX ) ){
            return obj;
        }
    }
    return nullptr;
}

static amf_v_member_t * amf_v_push_member( amf_t amf ){
    amf_value_t obj = amf_v_get_object( amf );
    if( amf_value_is( obj, AMF_TYPE_OBJECT ) ){
        return VEC_PUSH( obj->object.members );
    }
    if( amf_value_is( obj, AMF_TYPE_ARRAY ) ){
        if( VEC_SIZE( obj->array.assoc ) < obj->array.assoc_len ){
            return VEC_PUSH( obj->array.assoc );
        }
        return VEC_PUSH( obj->array.ordinal );
    }
    return nullptr;
}

static amf_value_t amf_push_item( amf_t amf ){
    if( amf->depth == 0 ){
        VEC_DECLARE( union amf_value ) old = amf->items;
        amf_value_t ret = VEC_PUSH(amf->items);
        if( amf->items != old ){
            //Move the reference table entries.
            for( size_t i = 0; i < VEC_SIZE(amf->ref_table); ++i ){
                size_t idx = amf->ref_table[i] - old;
                if( idx < VEC_SIZE(amf->items) ){
                    amf->ref_table[i] = amf->items + idx;
                }
            }
        }
        return ret;
    }
    else if( VEC_SIZE(amf->items) > 0 ){
        amf_value_t obj = amf_v_get_object( amf );
        if( obj && amf_value_is( obj, AMF_TYPE_OBJECT ) ){
            return &VEC_BACK(obj->object.members).value;
        }
        else if( obj && amf_value_is( obj, AMF_TYPE_ARRAY ) ){
            if( VEC_SIZE( obj->array.assoc ) + VEC_SIZE( obj->array.ordinal ) <= obj->array.assoc_len ){
                return &VEC_BACK( obj->array.assoc ).value;
            }
            return &VEC_BACK( obj->array.ordinal ).value;
        }
    }
    return nullptr;
}

#define PUSH_PREP(a,name) \
    if( a->depth > 0 && !a->member_ready ){\
        return AMF_ERR_NEED_NAME;\
    }\
    a->member_ready = false;\
    amf_value_t name = amf_push_item( a );\
    if( !name ){\
        return AMF_ERR_OOM;\
    }

static amf_err_t push_str( amf_t a, const void * str, amf_type_t t) {
    if( str != a->allocation ){
        size_t len = strlen( (const char *)str );
        void * ptr = malloc( len + 1 );
        if( !ptr ){
            return AMF_ERR_OOM;
        }
        memcpy( ptr, str, len + 1 );
        PUSH_PREP(a,target);
        target->string.data = (char*) ptr;
        target->string.length = len;
        target->string.type = t;
        return AMF_ERR_NONE;
    }
    else{
        PUSH_PREP(a,target);
        target->string.data = (char*) a->allocation;
        target->string.length = a->allocation_len;
        target->string.type = t;
        a->allocation = nullptr;
        a->allocation_len = 0;
        return AMF_ERR_NONE;
    }
}

amf_err_t amf_push_double( amf_t amf, double number ){
    PUSH_PREP( amf, target );
    target->dbl.number = number;
    target->dbl.type = AMF_TYPE_DOUBLE;
    return AMF_ERR_NONE;
}

amf_err_t amf_push_integer( amf_t amf, int32_t number ){
    PUSH_PREP( amf, target );
    target->integer.number = number;
    target->integer.type = AMF_TYPE_INTEGER;
    return AMF_ERR_NONE;
}

amf_err_t amf_push_boolean( amf_t amf, char boolean ){
    PUSH_PREP( amf, target );
    target->boolean.boolean = boolean;
    target->boolean.type = AMF_TYPE_BOOLEAN;
    return AMF_ERR_NONE;
}
amf_err_t amf_push_string_alloc( amf_t amf, void** destination, size_t length ){
    if( amf->allocation ){
        free( amf->allocation );
    }
    amf->allocation = malloc( length * sizeof( char ) + 1 );
    if( amf->allocation == nullptr ){
        return AMF_ERR_OOM;
    }
    amf->allocation_len = length;
    ((byte*)amf->allocation)[length] = 0;
    *destination = amf->allocation;
    return AMF_ERR_NONE;
}
amf_err_t amf_push_string( amf_t amf, const void *str ){
    return push_str( amf, str, AMF_TYPE_STRING );
}
amf_err_t amf_push_object_start( amf_t amf ){
    PUSH_PREP( amf, target );
    amf->depth ++;
    target->object.type = AMF_TYPE_OBJECT;
    VEC_INIT(target->object.members);

    amf_value_t* temp = VEC_PUSH(amf->ref_table);
    if( !temp ){
        return AMF_ERR_OOM;
    }
    *temp = target;
    return AMF_ERR_NONE;
}
amf_err_t amf_push_member( amf_t amf, const void *str ){
    if( amf->member_ready ){
        return AMF_ERR_INCOMPLETE;
    }
    amf_v_member_t* mem = amf_v_push_member( amf );
    if( mem ){
        if( str == amf->allocation ){
            mem->length = amf->allocation_len;
            mem->name = (char*) amf->allocation;
            mem->value.type = AMF_TYPE_UNDEFINED;
            amf->allocation_len = 0;
            amf->allocation = nullptr;
            amf->member_ready = true;
            return AMF_ERR_NONE;
        }
        size_t len = strlen( str );
        mem->name = malloc( len );
        if( mem->name == nullptr ){
            return AMF_ERR_OOM;
        }
        mem->length = len;
        mem->value.type = AMF_TYPE_UNDEFINED;
        memcpy( mem->name, str, len );
        amf->member_ready = true;
        return AMF_ERR_NONE;
    }
    return AMF_ERR_OOM;
}
amf_err_t amf_push_ecma_start( amf_t amf, uint32_t assoc_members ){
    PUSH_PREP( amf, target );
    amf->depth ++;
    target->array.type = AMF_TYPE_ARRAY;
    target->array.assoc_len = assoc_members;
    VEC_INIT(target->array.assoc);
    VEC_INIT(target->array.ordinal);

    amf_value_t* temp = VEC_PUSH(amf->ref_table);
    if( !temp ){
        return AMF_ERR_OOM;
    }
    *temp = target;
    return AMF_ERR_NONE;
}
amf_err_t amf_push_null( amf_t amf ){
    PUSH_PREP( amf, target );
    target->null.type = AMF_TYPE_NULL;
    return AMF_ERR_NONE;
}
amf_err_t amf_push_undefined( amf_t amf ){
    PUSH_PREP( amf, target );
    target->undefined.type = AMF_TYPE_UNDEFINED;
    return AMF_ERR_NONE;
}
amf_err_t amf_push_unsupported( amf_t amf ){
    PUSH_PREP( amf, target );
    target->undefined.type = AMF_TYPE_UNSUPPORTED;
    return AMF_ERR_NONE;
}


amf_err_t amf_push_reference( amf_t amf, unsigned int ref ){
    PUSH_PREP( amf, target );
    target->reference.ref = nullptr;
    target->reference.ref_num = ref;
    target->reference.type = AMF_TYPE_REFERENCE;
    if( ref < VEC_SIZE(amf->ref_table) ){
        target->reference.ref = amf->ref_table[ref];
    }
    return AMF_ERR_NONE;
}
amf_err_t amf_push_object_end( amf_t amf ){
    if( amf->member_ready ){
        amf->member_ready = false;
        amf_value_t obj = amf_v_get_object( amf );
        if( obj ){
            if( amf_value_is( obj, AMF_TYPE_OBJECT ) && VEC_SIZE(obj->object.members) > 0 ){
                free( VEC_BACK(obj->object.members).name );
                VEC_POP( obj->object.members );
            }
            else if( amf_value_is( obj, AMF_TYPE_ARRAY ) ){
                if( VEC_SIZE(obj->array.ordinal) > 0 ){
                    free( VEC_BACK(obj->array.ordinal).name );
                    VEC_POP( obj->array.ordinal );
                }
                else if( VEC_SIZE(obj->array.assoc) > 0 ){
                    free( VEC_BACK(obj->array.assoc).name );
                    VEC_POP( obj->array.assoc );
                }
            }
        }
    }
    if( amf->depth > 0 ){
        amf->depth--;
        return AMF_ERR_NONE;
    }
    return AMF_ERR_INVALID_DATA;
}
amf_err_t amf_push_assoc_end( amf_t amf ){
    amf_value_t obj = amf_v_get_object( amf );
    if( obj && amf_value_is( obj, AMF_TYPE_ARRAY ) ){
        if( amf->member_ready ){
            amf->member_ready = false;

            if( VEC_SIZE(obj->array.assoc) > 0 ){
                free( VEC_BACK(obj->array.assoc).name );
                VEC_POP( obj->array.assoc );
            }

        }
        obj->array.assoc_len = VEC_SIZE(obj->array.assoc);
        return AMF_ERR_NONE;
    }
    return AMF_ERR_INVALID_DATA;
}
amf_err_t amf_push_date( amf_t amf, double timestamp, int16_t timezone ){
    PUSH_PREP( amf, target );
    target->date.timestamp = timestamp;
    target->date.timezone = timezone;
    target->string.type = AMF_TYPE_DATE;
    return AMF_ERR_NONE;
}
amf_err_t amf_push_long_string( amf_t amf, const void *str ){
    return push_str( amf, str, AMF_TYPE_STRING );
}
amf_err_t amf_push_xml( amf_t amf, const void *xml ){
    return push_str( amf, xml, AMF_TYPE_XML_DOCUMENT );
}


size_t amf_get_count( amf_t amf ){
    return VEC_SIZE(amf->items);
}
amf_value_t amf_get_item( amf_t amf, size_t idx ){
    return amf->items + idx;
}

amf_value_t amf_dereference(amf_value_t target, bool recurse) {
    amf_value_t iter = target;
    while( iter->type == AMF_TYPE_REFERENCE ){
        iter = iter->reference.ref;
        if( !iter || iter == target || !recurse ){
            break;
        }
    }
    return iter;
}

bool amf_value_is( amf_value_t value, amf_type_t type ){
    if( type == value->type ){
        return true;
    }
    if( type == AMF_TYPE_COMPLEX ){
        switch( value->type ){
            case AMF_TYPE_OBJECT:
            case AMF_TYPE_ARRAY:
            case AMF_TYPE_RECORDSET:
            case AMF_TYPE_TYPED_OBJECT:
            case AMF_TYPE_VECTOR_DOUBLE:
            case AMF_TYPE_VECTOR_INT:
            case AMF_TYPE_VECTOR_OBJECT:
            case AMF_TYPE_VECTOR_UINT:
            case AMF_TYPE_BYTE_ARRAY:
            case AMF_TYPE_MOVIECLIP:
                return true;
            default:
                break;
        }
    }
    if( type == AMF_TYPE_ASSOCIATIVE ){
        switch( value->type ){
            case AMF_TYPE_OBJECT:
            case AMF_TYPE_ARRAY:
            case AMF_TYPE_TYPED_OBJECT:
                return true;
            default:
                break;
        }
    }
    /*if( value->type == AMF_TYPE_REFERENCE ){
        if( value->reference.ref->type != AMF_TYPE_REFERENCE ){
            return amf_value_is( value->reference.ref, type );
        }
    }*/
    return false;
}

bool amf_value_is_like( amf_value_t value, amf_type_t type ){
    if( amf_value_is( value, type ) ){
        return true;
    }
    if( value->type == AMF_TYPE_REFERENCE ){
        value = value->reference.ref;
    }
    switch( value->type ){
        //Numerics
        case AMF_TYPE_NULL:
        switch( type ){
            case AMF_TYPE_OBJECT:
            return true;
            default: break;
        }
        case AMF_TYPE_DOUBLE:
        case AMF_TYPE_INTEGER:
        case AMF_TYPE_DATE:
        switch( type ){
            case AMF_TYPE_DOUBLE:
            case AMF_TYPE_INTEGER:
            case AMF_TYPE_DATE:
            return true;
            default: break;
        }
        break;

        //Strings
        case AMF_TYPE_STRING:
        case AMF_TYPE_XML_DOCUMENT:
        switch( type ){
            case AMF_TYPE_STRING:
            case AMF_TYPE_XML_DOCUMENT:
            return true;
            default: break;
        }
        break;

        //Objects
        case AMF_TYPE_OBJECT:
        case AMF_TYPE_TYPED_OBJECT:
        switch( type ){
            case AMF_TYPE_OBJECT:
            case AMF_TYPE_TYPED_OBJECT:
            case AMF_TYPE_ASSOCIATIVE:
            return true;
            default: break;
        }
        break;

        //Arrays
        case AMF_TYPE_ARRAY:
        switch( type ){
            case AMF_TYPE_ARRAY:
            case AMF_TYPE_ASSOCIATIVE:
            return true;
            default: break;
        }
        default:
        break;
    }
    return false;
}

double amf_value_get_double( amf_value_t target ){
    if( target->type == AMF_TYPE_DOUBLE ){
        return target->dbl.number;
    }
    return target->integer.number;
}

int32_t amf_value_get_integer( amf_value_t target ){
    if( target->type == AMF_TYPE_INTEGER ){
        return target->integer.number;
    }
    return target->dbl.number;
}


bool amf_value_get_bool( amf_value_t target ){
    return target->boolean.boolean;
}

const char* amf_value_get_string( amf_value_t target, size_t *length ){
    if( length ){
        *length = target->string.length;
    }
    return target->string.data;
}

size_t amf_value_get_ref( amf_value_t target ){
    return target->reference.ref_num;
}

double amf_value_get_date( amf_value_t target, int16_t *timezone ){
    if( timezone ){
        *timezone = target->date.timezone;
    }
    return target->date.timestamp;
}

const char* amf_value_get_xml( amf_value_t target, size_t *length ){
    if( length ){
        *length = target->string.length;
    }
    return target->string.data;
}


static amf_value_t amf_assoc_get_value_arr( VEC_DECLARE(amf_v_member_t) arr, const char *key ){
    for( size_t i = 0; i < VEC_SIZE(arr); ++i ){
        if( memcmp( key, VEC_AT(arr, i).name, VEC_AT(arr, i).length ) == 0 ){
            return &VEC_AT(arr, i).value;
        }
    }
    return nullptr;
}

static amf_value_t amf_assoc_get_value_idx_arr( VEC_DECLARE(amf_v_member_t) arr, size_t idx, char **key, size_t *key_len ){
    if( idx >= VEC_SIZE(arr) ){
        return nullptr;
    }
    if( key ){
        *key = VEC_AT(arr, idx).name;
    }
    if( key_len ){
        *key_len = arr[idx].length;
    }
    return &arr[idx].value;
}

static size_t amf_assoc_get_count_arr( VEC_DECLARE(amf_v_member_t) arr ){
    return VEC_SIZE(arr);
}

amf_value_t amf_assoc_get_value( amf_value_t target, const char *key ){
    return amf_assoc_get_value_arr( target->associative.members, key );
}

amf_value_t amf_assoc_get_value_idx( amf_value_t target, size_t idx, char **key, size_t *key_len ){
    return amf_assoc_get_value_idx_arr( target->associative.members, idx, key, key_len );
}

size_t amf_assoc_get_count( const amf_value_t target ){
    return amf_assoc_get_count_arr( target->associative.members );
}

amf_value_t amf_obj_get_value( amf_value_t target, const char *key ){
    if( target->type == AMF_TYPE_NULL ){
        return target;
    }
    return amf_assoc_get_value_arr( target->object.members, key );
}
amf_value_t amf_obj_get_value_idx( amf_value_t target, size_t idx, char **key, size_t *key_len ){
    if( target->type == AMF_TYPE_NULL ){
        return target;
    }
    return amf_assoc_get_value_idx_arr( target->object.members, idx, key, key_len );
}
size_t amf_obj_get_count( const amf_value_t target ){
    if( target->type == AMF_TYPE_NULL ){
        return 0;
    }
    return amf_assoc_get_count_arr( target->object.members );
}

amf_value_t amf_arr_get_assoc_value( amf_value_t target, const char *key ){
    if( target->type == AMF_TYPE_NULL ){
        return 0;
    }
    return amf_assoc_get_value_arr( target->array.assoc, key );
}
amf_value_t amf_arr_get_assoc_value_idx( amf_value_t target, size_t idx, char **key, size_t *key_len ){
    if( target->type == AMF_TYPE_NULL ){
        return 0;
    }
    return amf_assoc_get_value_idx_arr( target->array.assoc, idx, key, key_len );
}
size_t amf_arr_get_assoc_count( const amf_value_t target ){
    if( target->type == AMF_TYPE_NULL ){
        return 0;
    }
    return amf_assoc_get_count_arr( target->array.assoc );
}

amf_value_t amf_arr_get_ord_value( amf_value_t target, const char *key ){
    if( target->type == AMF_TYPE_NULL ){
        return 0;
    }
    return amf_assoc_get_value_arr( target->array.ordinal, key );
}
amf_value_t amf_arr_get_ord_value_idx( amf_value_t target, size_t idx, char **key, size_t *key_len ){
    if( target->type == AMF_TYPE_NULL ){
        return 0;
    }
    return amf_assoc_get_value_idx_arr( target->array.ordinal, idx, key, key_len );
}
size_t amf_arr_get_ord_count( const amf_value_t target ){
    if( target->type == AMF_TYPE_NULL ){
        return 0;
    }
    return amf_assoc_get_count_arr( target->array.ordinal );
}


#include <stdio.h>

void amf_print_value_internal( amf_value_t val, size_t depth ){

    switch( val->type ){
        case AMF_TYPE_AVMPLUS:
            break;
        case AMF_TYPE_BOOLEAN:
            printf( "Boolean: %s", val->boolean.boolean ? "true" : "false" );
            break;
        case AMF_TYPE_DATE:
            printf( "Date: %f +%d", val->date.timestamp, val->date.timezone );
            break;
        case AMF_TYPE_MOVIECLIP:
            printf( "Movie clip" );
            break;
        case AMF_TYPE_NULL:
            printf("Null");
            break;
        case AMF_TYPE_DOUBLE:
            printf( "Double: %f", val->dbl.number );
            break;
        case AMF_TYPE_INTEGER:
            printf( "Integer: %d", val->integer.number );
            break;
        case AMF_TYPE_RECORDSET:
            printf( "Recordset");
            break;
        case AMF_TYPE_REFERENCE:
            printf( "Reference to %lu", val->reference.ref_num );
            break;
        case AMF_TYPE_UNDEFINED:
            printf( "Undefined");
            break;
        case AMF_TYPE_UNSUPPORTED:
            printf( "Unsupported");
            break;

        case AMF_TYPE_TYPED_OBJECT:
            printf( "Typed Object");
            break;

        case AMF_TYPE_STRING:
            printf( "String: %s", val->string.data );
            break;
        case AMF_TYPE_XML_DOCUMENT:
            printf( "XML document: %s", val->string.data );
            break;

        case AMF_TYPE_ARRAY:
            printf( "Array: [\n");
            for( size_t i = 0; i < VEC_SIZE(val->array.assoc); ++i ){
                for( size_t i = 0; i <= depth; ++i ){
                    printf("    ");
                }
                printf( "%.*s: ", (int) val->array.assoc[i].length, val->array.assoc[i].name );
                amf_print_value_internal( &val->array.assoc[i].value, depth+1 );
            }
            for( size_t i = 0; i <= depth; ++i ){
                printf("    ");
            }
            printf( "End Assoc.\n" );
            for( size_t i = 0; i < VEC_SIZE(val->array.ordinal); ++i ){
                for( size_t i = 0; i <= depth; ++i ){
                    printf("    ");
                }
                printf( "%.*s: ", (int) val->array.ordinal[i].length, val->array.ordinal[i].name );
                amf_print_value_internal( &val->array.ordinal[i].value, depth+1 );
            }
            for( size_t i = 0; i < depth; ++i ){
                printf("    ");
            }
            printf( "]");
            break;
        case AMF_TYPE_OBJECT:
            printf( "Object: {\n");
            for( size_t i = 0; i < VEC_SIZE(val->object.members); ++i ){
                for( size_t i = 0; i <= depth; ++i ){
                    printf("    ");
                }
                printf( "%.*s: ", (int) val->object.members[i].length, val->object.members[i].name );
                amf_print_value_internal( &val->object.members[i].value, depth+1 );
            }
            for( size_t i = 0; i < depth; ++i ){
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
    amf_print_value_internal( val, 0 );
}

void amf_print( amf_t amf ){
    for( size_t i = 0; i < amf_get_count( amf ); ++i ){
        amf_print_value( amf_get_item( amf, i ) );
    }
}




static amf_err_t amf_push_simple_list_memb( amf_t amf, const char * restrict member_name ){
    size_t len;
    void *buffer;
    amf_err_t ret = AMF_ERR_NONE;
    if( member_name ){
        len = strlen( member_name );
        ret = amf_push_string_alloc( amf, &buffer, len );
        if( ret != AMF_ERR_NONE ){
            return ret;
        }
        memcpy( buffer, member_name, len );
        ret = amf_push_member( amf, buffer );
        if( ret != AMF_ERR_NONE ){
            return ret;
        }
    }
    return ret;
}
static amf_err_t amf_push_simple_list_str( amf_t amf, const char * restrict member_name, const char * restrict member_value ){
    size_t len;
    void *buffer;
    amf_err_t ret = amf_push_simple_list_memb( amf, member_name );
    len = strlen( member_value );
    ret = amf_push_string_alloc( amf, &buffer, len );
    if( ret != AMF_ERR_NONE ){
        return ret;
    }
    memcpy( buffer, member_value, len );
    return amf_push_string( amf, buffer );
}

static amf_err_t amf_push_simple_list_bool( amf_t amf, const char * restrict member_name, int member_value ){
    amf_err_t ret = amf_push_simple_list_memb( amf, member_name );
    if( ret != AMF_ERR_NONE ){
        return ret;
    }
    return amf_push_boolean( amf, member_value );
}

static amf_err_t amf_push_simple_list_dbl( amf_t amf, const char * restrict member_name, double member_value ){
    amf_err_t ret = amf_push_simple_list_memb( amf, member_name );
    if( ret != AMF_ERR_NONE ){
        return ret;
    }

    return amf_push_double( amf, member_value );
}
static amf_err_t amf_push_simple_list_int( amf_t amf, const char * restrict member_name, int32_t member_value ){
    amf_err_t ret = amf_push_simple_list_memb( amf, member_name );
    if( ret != AMF_ERR_NONE ){
        return ret;
    }

    return amf_push_integer( amf, member_value );
}
static amf_err_t amf_push_simple_list_null( amf_t amf, const char * restrict member_name ){
    amf_err_t ret = amf_push_simple_list_memb( amf, member_name );
    if( ret != AMF_ERR_NONE ){
        return ret;
    }

    return amf_push_null( amf );
}
static amf_err_t amf_push_simple_list_start_obj( amf_t amf, const char * restrict member_name ){
    amf_err_t ret = amf_push_simple_list_memb( amf, member_name );
    if( ret != AMF_ERR_NONE ){
        return ret;
    }

    return amf_push_object_start( amf );
}
static amf_err_t amf_push_simple_list_start_arr( amf_t amf, const char * restrict member_name ){
    amf_err_t ret = amf_push_simple_list_memb( amf, member_name );
    if( ret != AMF_ERR_NONE ){
        return ret;
    }

    return amf_push_ecma_start( amf, UINT32_MAX );
}

static amf_err_t amf_push_simple_list_end( amf_t amf, const char * restrict member_name ){
    amf_err_t ret = amf_push_simple_list_memb( amf, member_name );
    if( ret != AMF_ERR_NONE ){
        return ret;
    }

    return amf_push_object_end( amf );
}

amf_err_t amf_push_simple_list( amf_t amf, va_list list ){
    amf_err_t ret;
    amf_type_t arg = AMF_TYPE_NONE;
    int depth = 0;
    while( true ){
        arg = (amf_type_t) va_arg( list, int );
        if( arg == AMF_TYPE_NONE ){
            break;
        }
        const char * membname;
        if( depth > 0 || arg == AMF_TYPE_NULL ){
                membname = va_arg( list, const char* );
        }
        if( depth == 0 ){
            membname = nullptr;
        }
        switch( arg ){
            case AMF_TYPE_BOOLEAN: ret = amf_push_simple_list_bool( amf, membname, va_arg( list, int ) ); break;
            case AMF_TYPE_STRING: ret = amf_push_simple_list_str( amf, membname, va_arg( list, const char* ) ); break;
            case AMF_TYPE_DOUBLE: ret = amf_push_simple_list_dbl( amf, membname, va_arg( list, double ) ); break;
            case AMF_TYPE_INTEGER: ret = amf_push_simple_list_int( amf, membname, va_arg( list, int ) ); break;
            case AMF_TYPE_NULL: ret = amf_push_simple_list_null( amf, membname ); break;
            case AMF_TYPE_UNDEFINED: ret = amf_push_undefined( amf ); break;
            case AMF_TYPE_UNSUPPORTED: ret = amf_push_unsupported( amf ); break;
            case AMF_TYPE_OBJECT:
                ++ depth;
                ret = amf_push_simple_list_start_obj( amf, membname );
                break;
            case AMF_TYPE_ARRAY:
                ++ depth;
                ret = amf_push_simple_list_start_arr( amf, membname );
                break;
            case AMF_TYPE_ECMA_ARRAY_ASSOC_END:
                ret = amf_push_assoc_end( amf );
                break;
            case AMF_TYPE_OBJECT_END:
                -- depth;
                ret = amf_push_simple_list_end( amf, membname );
                break;
            break;
            default: return AMF_ERR_INVALID_DATA;
        }
        if( ret != AMF_ERR_NONE ){
            return ret;
        }
    }
    return ret;
}


amf_err_t amf_push_simple( amf_t amf, ... ){
    va_list list;
    va_start( list, amf );
    amf_err_t ret = amf_push_simple_list( amf, list );
    va_end(list);
    return ret;
}
