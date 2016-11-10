/*
    amf_constants.h

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


    ----------------------------------------

    All references in this file are relevant to the official AMF0 and AMF3 specifications.

    AMF0:
        Download: http://wwwimages.adobe.com/content/dam/Adobe/en/devnet/amf/pdf/amf0-file-format-specification.pdf
        Information Page: http://www.adobe.com/devnet/swf.html
        CRC32: f74e4446
        Date: June 29th, 2016

    AMF3:
        Download: http://wwwimages.adobe.com/content/dam/Adobe/en/devnet/amf/pdf/amf-file-format-spec.pdf
        Information Page: http://www.adobe.com/devnet/swf.html
        CRC32: 8e55cb19
        Date: June 29th, 2016

*/

#ifndef RTMP_H_AMF_CONSTANTS_H
#define RTMP_H_AMF_CONSTANTS_H

#ifdef __cplusplus
extern "C" {
#endif


#include <openrtmp/rtmp/rtmp_types.h>

#define AMF_MAX_KEY 1000

/*! \addtogroup amf_ref AMF
    @{ */

/*! \addtogroup amf_constants Constants
    @{ */

/*! \brief      Refers to either a length or an AMF error code.
    \remarks    Values of this type perform double duty. They are used both to indicate an error status with an AMF operation,
                as well as report on sizes. If the numeric value is less than zero, then there is an error. If it is zero or greater,
                then the operation was a success, and the value usually indicates how many bytes were read, written, or otherwise consumed.
*/
typedef enum AMF_ERR {
    AMF_ERR_NONE,               //!< No error occured.
    AMF_ERR_INVALID_DATA = -10, //!< Invalid data was provided to the codec.
    AMF_ERR_INCOMPLETE,         //!< The data stream ended unexpectedly.
    AMF_ERR_NEED_NAME,          //!< An attempt to write a member without first writing a member name was made.
    AMF_ERR_OOM,                //!< An attempt to allocate dynamic memory failed.
    AMF_ERR_BAD_ALLOC           //!< Unused.
} amf_err_t;

#define AMF_SIZE(n) ((amf_err_t)(n))

//AMF0 Type Markers             See AMF0 Specification
/*! \brief      Refers to AMF0 types using their numeric values as defined in the AMF0 specification.
    \remarks    These are mainly used internally by and to interface with the AMF0 codec.
*/
typedef enum AMF0_TYPE {
    AMF0_TYPE_NUMBER,               //!< Indicates an AMF0 number. \n Defined as `0` in §2.2 on page 5 of the \ref amf0_spec
    AMF0_TYPE_BOOLEAN,              //!< Indicates an AMF0 boolean. \n Defined as `1` in §2.3 on page 5 of the \ref amf0_spec
    AMF0_TYPE_STRING,               //!< Indicates an AMF0 string. \n Defined as `2` in §2.4 on page 5 of the \ref amf0_spec
    AMF0_TYPE_OBJECT,               //!< Indicates the beginning of an AMF0 object. \n Defined as `3` in §2.5 on page 5 of the \ref amf0_spec
    AMF0_TYPE_MOVIECLIP,            //!< Indicates an AMF0 movie clip. \n Defined as `4` in §2.6 on page 5 of the \ref amf0_spec
    AMF0_TYPE_NULL,                 //!< Indicates an AMF0 null value. \n Defined as `5` in §2.7 on page 6 of the \ref amf0_spec
    AMF0_TYPE_UNDEFINED,            //!< Indicates an AMF0 undefined value. \n Defined as `6` in §2.8 on page 6 of the \ref amf0_spec
    AMF0_TYPE_REFERENCE,            //!< Indicates an AMF0 reference. \n Defined as `7` in §2.9 on page 6 of the \ref amf0_spec
    AMF0_TYPE_ECMA_ARRAY,           //!< Indicates an AMF0 ECMA array, which has both associative and ordinal components. \n Defined as `8` in §2.10 on page 6 of the \ref amf0_spec
    AMF0_TYPE_OBJECT_END,           //!< Indicates the end of an AMF0 object. \n Defined as `9` in §2.11 on page 6 of the \ref amf0_spec
    AMF0_TYPE_STRICT_ARRAY,         //!< Indicates an AMF0 strict array, which contains only ordinal indices. \n Defined as `10` in §2.12 on page 7 of the \ref amf0_spec
    AMF0_TYPE_DATE,                 //!< Indicates an AMF0 date. \n Defined as `11` in §2.13 on page 7 of the \ref amf0_spec
    AMF0_TYPE_LONG_STRING,          //!< Indicates an AMF0 long string. \n Defined as `12` in §2.14 on page 7 of the \ref amf0_spec
    AMF0_TYPE_UNSUPPORTED,          //!< Indicates an unsupported type. \n Defined as `13` in §2.15 on page 7 of the \ref amf0_spec
    AMF0_TYPE_RECORDSET,            //!< Indicates an AMF0 record set. \n Defined as `14` in §2.16 on page 7 of the \ref amf0_spec
    AMF0_TYPE_XML_DOCUMENT,         //!< Indicates an AMF0 XML document, which is essentially a long string. \n Defined as `15` in §2.17 on page 7 of the \ref amf0_spec
    AMF0_TYPE_TYPED_OBJECT,         //!< Indicates an AMF0 alias name. \n Defined as `16` in §2.18 on page 8 of the \ref amf0_spec
    AMF0_TYPE_AVMPLUS,              //!< Indicates that the following object is actually formatted in the AMF3 format. \n Defined as `17` in §3.1 on page 8 of the \ref amf0_spec
    AMF0_TYPE_NUMBER_INT,           //Not a part of the spec, for internal use
    AMF0_TYPE_NONE,
} amf0_type_t;

//AMF3 Type Markers             See AMF3 Specification
typedef enum AMF3_TYPE {
    AMF3_TYPE_UNDEFINED,        //Page 6, §3.2
    AMF3_TYPE_NULL,             //Page 6, §3.3
    AMF3_TYPE_FALSE,            //Page 7, §3.4
    AMF3_TYPE_TRUE,             //Page 7, §3.5
    AMF3_TYPE_INTEGER,          //Page 7, §3.6
    AMF3_TYPE_DOUBLE,           //Page 7, §3.7
    AMF3_TYPE_STRING,           //Page 7, §3.8
    AMF3_TYPE_XML_DOCUMENT,     //Page 8, §3.9
    AMF3_TYPE_DATE,             //Page 8, §3.10
    AMF3_TYPE_ARRAY,            //Page 9, §3.11
    AMF3_TYPE_OBJECT,           //Page 9, §3.12
    AMF3_TYPE_XML,              //Page 11, §3.13
    AMF3_TYPE_BYTE_ARRAY,       //Page 11, §3.14
    AMF3_TYPE_VECTOR_INT,       //Page 12, §3.15
    AMF3_TYPE_VECTOR_UINT,      //Page 12, §3.15
    AMF3_TYPE_VECTOR_DOUBLE,    //Page 12, §3.15
    AMF3_TYPE_VECTOR_OBJECT,    //Page 12, §3.15
    AMF3_TYPE_DICTIONARY        //Page 13, §3.16
} amf3_type_t;

//AMF Object Type Markers
/*! \brief      Refers to various AMF types in an `amf_t`.
    \remarks    These types are only meant to be used with libOpenRTMP. There is no overlap between the underlying numeric values
                of these constants and the AMF specifications. They are merely a relatively version agnostic way of representing
                AMF types for use in the `amf_t` abstraction.
*/
typedef enum AMF_TYPE {
    AMF_TYPE_UNDEFINED,             //!< Refers to an undefined type.
    AMF_TYPE_NULL,                  //!< Refers to a null value.
    AMF_TYPE_BOOLEAN,               //!< Refers to a boolean value.
    AMF_TYPE_INTEGER,               //!< Refers to an integer value.
    AMF_TYPE_DOUBLE,                //!< Refers to a double precision floating point number.
    AMF_TYPE_DATE,                  //!< Refers to a date.
    AMF_TYPE_STRING,                //!< Refers to a string.
    AMF_TYPE_XML_DOCUMENT,          //!< Refers to an XML document, which is basically a string.
    AMF_TYPE_OBJECT,                //!< Refers to an AMF object value.
    AMF_TYPE_TYPED_OBJECT,          //!< Refers to an object alias.
    AMF_TYPE_OBJECT_END,            //!< Refers to the end of an object.
    AMF_TYPE_ARRAY,                 //!< Refers to an ECMA array.
    AMF_TYPE_VECTOR_INT,            //!< Refers to an array of integers.
    AMF_TYPE_VECTOR_UINT,           //!< Refers to an array of unsigned integers.
    AMF_TYPE_VECTOR_DOUBLE,         //!< Refers to an array of double precision floating point numbers.
    AMF_TYPE_VECTOR_OBJECT,         //!< Refers to an array of AMF objects.
    AMF_TYPE_BYTE_ARRAY,            //!< Refers to an array of bytes.
    AMF_TYPE_MOVIECLIP,             //!< Refers to a movie clip.
    AMF_TYPE_RECORDSET,             //!< Refers to a record set.
    AMF_TYPE_REFERENCE,             //!< Refers to a reference type.
    AMF_TYPE_UNSUPPORTED,           //!< Refers to an unsupported type.
    AMF_TYPE_AVMPLUS,               //!< Indicates an AMF version upgrade.
    AMF_TYPE_NONE,                  //!< Refers to a non-existing type.
                                    //!< Mainly used internally, and to indicate the end of an AMF object.
    AMF_TYPE_ECMA_ARRAY_ASSOC_END,  //!< Refers to the end of an ECMA array.
                                    //!< Only used when building AMF objects with `amf_push_simple()`.
    AMF_TYPE_INTEGER24,	            //!< Refers to a 24 bit signed integer.
                                    //!< Only used when building AMF objects with `amf_push_simple()`.
    AMF_TYPE_INTEGER16,             //!< Refers to a 16 bit signed integer.
                                    //!< Only used when building AMF objects with `amf_push_simple()`.
    AMF_TYPE_ASSOCIATIVE,           //!< Refers to a type with associative values.
                                    //!< When used to check type equivalence, this will match any of the following types:
                                    //!< * AMF_TYPE_OBJECT
                                    //!< * AMF_TYPE_ARRAY
                                    //!< * AMF_TYPE_TYPED_OBJECT
    AMF_TYPE_COMPLEX,               //!< Refers to a complex type.
                                    //!< When used to check type equivalence, this will match any of the following types:
                                    //!< * AMF_TYPE_OBJECT
                                    //!< * AMF_TYPE_ARRAY
                                    //!< * AMF_TYPE_RECORDSET
                                    //!< * AMF_TYPE_TYPED_OBJECT
                                    //!< * AMF_TYPE_VECTOR_DOUBLE
                                    //!< * AMF_TYPE_VECTOR_INT
                                    //!< * AMF_TYPE_VECTOR_OBJECT
                                    //!< * AMF_TYPE_VECTOR_UINT
                                    //!< * AMF_TYPE_BYTE_ARRAY
                                    //!< * AMF_TYPE_MOVIECLIP
} amf_type_t;

/*! @} */
/*! @} */

#ifdef __cplusplus
}
#endif


#endif
