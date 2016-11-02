title: Debug (rtmp/rtmp_debug.h)
--------------------------------------

This header provides functions for converting numeric status codes into human readable strings.

<div id="rtmp_get_message_type_name"></div>
> [`const char* rtmp_get_message_type_name( rtmp_message_type_t mtype )`](#rtmp_get_message_type_name)
> `rtmp_message_type_t mtype`: The message type number
>
> Returns a human readable string for `mtype`.

<div id="rtmp_get_limit_type_name"></div>
> [`const char* rtmp_get_limit_type_name( rtmp_limit_t mtype )`](#rtmp_get_limit_type_name)
> `rtmp_limit_t mtype`: The limit value
>
> Returns a human readable string for `mtype`.

<div id="rtmp_get_snd_codec_flag_name"></div>
> [`const char* rtmp_get_snd_codec_flag_name( rtmp_support_codec_snd_t mtype )`](#rtmp_get_snd_codec_flag_name)
> `rtmp_support_codec_snd_t mtype`: A sound codec support flag.
>
> Returns a human readable string for `mtype`. `mtype` must be a single flag. To obtain a list, use `rtmp_get_snd_codec_flag_list` instead.

<div id="rtmp_get_snd_codec_flag_list"></div>
> [`const char* rtmp_get_snd_codec_flag_list( rtmp_support_codec_snd_t mtype )`](#rtmp_get_snd_codec_flag_list)
> `rtmp_support_codec_snd_t mtype`: A sound codec support bitset.
>
> Returns a human readable string for `mtype`, which lists all supported codecs in a single string.
> The returned value is a static buffer which may be mutated in future calls to this function and similar.

<div id="rtmp_get_vid_codec_flag_name"></div>
> [`const char* rtmp_get_vid_codec_flag_name( rtmp_support_codec_vid_t mtype )`](#rtmp_get_vid_codec_flag_name)
> `rtmp_support_codec_vid_t mtype`: A video codec support flag.
>
> Returns a human readable string for `mtype`. `mtype` must be a single flag. To obtain a list, use `rtmp_get_vid_codec_flag_list` instead.

<div id="rtmp_get_vid_codec_flag_list"></div>
> [`const char* rtmp_get_vid_codec_flag_list( rtmp_support_codec_vid_t mtype )`](#rtmp_get_vid_codec_flag_list)
> `rtmp_support_codec_vid_t mtype`: A video codec support bitset.
>
> Returns a human readable string for `mtype`, which lists all supported codecs in a single string.
> The returned value is a static buffer which may be mutated in future calls to this function and similar.

<div id="rtmp_get_vid_func_flag_name"></div>
> [`const char* rtmp_get_vid_func_flag_name( rtmp_support_func_vid_t mtype )`](#rtmp_get_vid_func_flag_name)
> `rtmp_support_func_vid_t mtype`: A video function support flag.
>
> Returns a human readable string for`mtype`. `mtype` must be a single flag. To obtain a list, use `rtmp_get_vid_func_flag_list` instead.

<div id="rtmp_get_vid_func_flag_list"></div>
> [`const char* rtmp_get_vid_func_flag_list( rtmp_support_func_vid_t mtype )`](#rtmp_get_vid_func_flag_list)
> `rtmp_support_func_vid_t mtype`: 
>
> Returns a human readable string for `mtype`, which lists all supported functions in a single string.
> The returned value is a static buffer which may be mutated in future calls to this function and similar.

<div id="rtmp_get_so_evt_type_name"></div>
> [`const char* rtmp_get_so_evt_type_name( rtmp_so_evt_t mtype )`](#rtmp_get_so_evt_type_name)
> `rtmp_so_evt_t mtype`: A shared object event type number.
>
> Returns a human readable string for `mtype`.

<div id="rtmp_get_usr_evt_type_name"></div>
> [`const char* rtmp_get_usr_evt_type_name( rtmp_usr_evt_t mtype )`](#rtmp_get_usr_evt_type_name)
> `rtmp_usr_evt_t mtype`: A user event type number.
>
> Returns a human readable string for `mtype`.

<div id="rtmp_get_event_name"></div>
> [`const char* rtmp_get_event_name( rtmp_event_t mtype )`](#rtmp_get_event_name)
> `rtmp_event_t mtype`: An RTMP chunk type number.
>
> Returns a human readable string for `mtype`.

<div id="rtmp_get_err_name"></div>
> [`const char* rtmp_get_err_name( rtmp_err_t err )`](#rtmp_get_err_name)
> `rtmp_err_t err`: An error code.
>
> Returns a human readable string for `err`.