/*
    parseurl.h

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

#ifndef RTMP_H_PARSEURL_H
#define RTMP_H_PARSEURL_H

#include "rtmp/rtmp_types.h"

typedef enum {
    PARSEURL_URL,
    PARSEURL_URL_FORGIVING,
    PARSEURL_URL_STRICT,
    PARSEURL_SCHEME,
    PARSEURL_USER,
    PARSEURL_PASS,
    PARSEURL_HOST,
    PARSEURL_PORT,
    PARSEURL_PATH,
    PARSEURL_QUERY,
    PARSEURL_FRAGMENT
} parseurl_part_t;

typedef struct parseurl * parseurl_t;

parseurl_t parseurl_create( void );
void parseurl_destroy( parseurl_t parser );
const char * parseurl_get( parseurl_t parser, parseurl_part_t part, const char *fallback );
bool parseurl_set( parseurl_t parser, parseurl_part_t part, const char *value );

#endif
