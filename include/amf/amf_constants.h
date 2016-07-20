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

#pragma once

#include "../rtmp/rtmp_types.h"

#define AMF_MAX_KEY 1000

typedef enum AMF_ERR {
    AMF_ERR_NONE,
    AMF_ERR_INVALID_DATA = -10,
    AMF_ERR_INCOMPLETE,
    AMF_ERR_NEED_NAME,
    AMF_ERR_OOM,
    AMF_ERR_BAD_ALLOC
} amf_err_t;

//AMF0 Type Markers             See AMF0 Specification
typedef enum AMF0_TYPE{
    AMF0_TYPE_NUMBER,           //Page 5, §2.2
    AMF0_TYPE_BOOLEAN,          //Page 5, §2.3
    AMF0_TYPE_STRING,           //Page 5, §2.4
    AMF0_TYPE_OBJECT,           //Page 5, §2.5
    AMF0_TYPE_MOVIECLIP,        //Page 5, §2.6
    AMF0_TYPE_NULL,             //Page 6, §2.7
    AMF0_TYPE_UNDEFINED,        //Page 6, §2.8
    AMF0_TYPE_REFERENCE,        //Page 6, §2.9
    AMF0_TYPE_ECMA_ARRAY,       //Page 6, §2.10
    AMF0_TYPE_OBJECT_END,       //Page 6, §2.11
    AMF0_TYPE_STRICT_ARRAY,     //Page 7, §2.12
    AMF0_TYPE_DATE,             //Page 7, §2.13
    AMF0_TYPE_LONG_STRING,      //Page 7, §2.14
    AMF0_TYPE_UNSUPPORTED,      //Page 7, §2.15
    AMF0_TYPE_RECORDSET,        //Page 7, §2.16
    AMF0_TYPE_XML_DOCUMENT,     //Page 7, §2.17
    AMF0_TYPE_TYPED_OBJECT,     //Page 8, §2.18
    AMF0_TYPE_AVMPLUS           //Page 8, §3.1
} amf0_type_t;

//AMF3 Type Markers             See AMF3 Specification
typedef enum AMF3_TYPE{
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

#define AMF0_LEN_PROP 2

