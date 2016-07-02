/*
    rtmp_debug.h

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

#include "rtmp_types.h"
#include "rtmp_constants.h"

typedef struct{
    void (*s)(const char* v);
    void (*d)(int v);
    void (*f)(double v);
    void (*u)(unsigned int v);
} rtmp_printer, *rtmp_printer_t;

extern rtmp_printer_t rtmp_default_printer;

const char* rtmp_get_message_type_name( rtmp_message_type_t mtype );
const char* rtmp_get_limit_type_name( rtmp_limit_t mtype );
const char* rtmp_get_snd_codec_flag_name( rtmp_support_codec_snd_t mtype );
const char* rtmp_get_snd_codec_flag_list( rtmp_support_codec_snd_t mtype );
const char* rtmp_get_vid_codec_flag_name( rtmp_support_codec_vid_t mtype );
const char* rtmp_get_vid_codec_flag_list( rtmp_support_codec_vid_t mtype );
const char* rtmp_get_vid_func_flag_name( rtmp_support_func_vid_t mtype );
const char* rtmp_get_vid_func_flag_list( rtmp_support_func_vid_t mtype );
const char* rtmp_get_so_evt_type_name( rtmp_so_evt_t mtype );
const char* rtmp_get_usr_evt_type_name( rtmp_usr_evt_t mtype );

const char* rtmp_get_err_name( rtmp_err_t err );
