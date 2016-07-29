/*
    ringbuffer.h

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

typedef struct rtmp_video_stream * rtmp_video_stream_t;
typedef struct rtmp_audio_stream * rtmp_audio_stream_t;

typedef rtmp_cb_status_t (*rtmp_audio_proc)(
    rtmp_audio_stream_t audio,
    void *user
);

typedef rtmp_cb_status_t (*rtmp_video_proc)(
    rtmp_video_stream_t audio,
    void *user
);

rtmp_err_t rtmp_push_audio( rtmp_audio_stream_t, byte *data, size_t data_len );
rtmp_err_t rtmp_push_video( rtmp_video_stream_t, byte *data, size_t data_len );
