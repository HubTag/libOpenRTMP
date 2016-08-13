/*
    rtmp_chunk_assembler.h

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

#ifndef RTMP_H_CHUNK_ASSEMBLER_H
#define RTMP_H_CHUNK_ASSEMBLER_H

#include "rtmp_chunk_conn.h"


typedef struct rtmp_chunk_assembler *rtmp_chunk_assembler_t;

rtmp_chunk_assembler_t rtmp_chunk_assembler_create( size_t max_size, rtmp_chunk_proc chunk_cb, rtmp_event_proc event_cb, rtmp_log_proc log_cb, void *user );
void rtmp_chunk_assembler_destroy( rtmp_chunk_assembler_t assembler );
void rtmp_chunk_assembler_assign( rtmp_chunk_assembler_t assembler,  rtmp_chunk_conn_t connection );


#endif
