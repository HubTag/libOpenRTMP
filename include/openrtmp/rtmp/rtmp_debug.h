/*
    rtmp_debug.h

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

#ifndef RTMP_H_DEBUG_H
#define RTMP_H_DEBUG_H

#ifdef __cplusplus
extern "C" {
#endif


#include <openrtmp/rtmp/rtmp_types.h>
#include <openrtmp/rtmp/rtmp_constants.h>


typedef struct{
    void (*s)(const char* v);
    void (*s2)(const char* v, size_t len);
    void (*d)(int v);
    void (*f)(double v);
    void (*u)(unsigned int v);
} rtmp_printer, *rtmp_printer_t;

extern rtmp_printer_t rtmp_default_printer;


/*! \addtogroup rtmp_ref RTMP
    @{ */
/*! \addtogroup rtmp_debug Debugging
    @{ */


/*! \brief      Gets a human readable string containing the name of a `rtmp_message_type_t` code.
    \param      code     The message type code.
    \return     A human readable C string which corresponds to \a code.
 */
const char* rtmp_get_message_type_name( rtmp_message_type_t code );

/*! \brief      Gets a human readable string containing the name of a `rtmp_limit_t` code.
    \param      code     The limit type code.
    \return     A human readable C string which corresponds to \a code.
 */
const char* rtmp_get_limit_type_name( rtmp_limit_t code );

/*! \brief      Gets a human readable string containing the name of a single `rtmp_support_codec_snd_t` flag.
    \param      flag     The sound codec support flag.
    \return     A human readable C string which corresponds to \a flag.
    \remarks    This function will only recognize individual flags. If you want to pass in a whole bitset, please use
                `rtmp_get_snd_codec_flag_list()`.
 */
const char* rtmp_get_snd_codec_flag_name( rtmp_support_codec_snd_t flag );

/*! \brief      Gets a human readable string containing the name of a whole `rtmp_support_codec_snd_t` bitset.
    \param      bitset   The sound codec support bitset.
    \return     A human readable C string which contains a comma separated list of the names for all of the flags expressed in \a bitset.
    \remarks    The library uses an internally allocated static buffer for this operation. Subsequent calls to this function or
                similar functions will cause the return value from prior calls to no longer be valid. Additionally, repeated calls
                to this function with different arguments will likely yield the same pointer, though the contents will likely have
                changed every time.
    \sa         rtmp_get_snd_codec_flag_name
 */
const char* rtmp_get_snd_codec_flag_list( rtmp_support_codec_snd_t bitset );

/*! \brief      Gets a human readable string containing the name of a `rtmp_support_codec_vid_t` flag.
    \param      flag     The video codec support flag.
    \return     A human readable C string which corresponds to \a flag.
    \remarks    This function will only recognize individual flags. If you want to pass in a whole bitset, please use
                `rtmp_get_vid_codec_flag_list()`.
 */
const char* rtmp_get_vid_codec_flag_name( rtmp_support_codec_vid_t flag );

/*! \brief      Gets a human readable string containing the name of a whole `rtmp_support_codec_vid_t` bitset.
    \param      bitset   The video codec support bitset.
    \return     A human readable C string which contains a comma separated list of the names for all of the flags expressed in \a bitset.
    \remarks    The library uses an internally allocated static buffer for this operation. Subsequent calls to this function or
                similar functions will cause the return value from prior calls to no longer be valid. Additionally, repeated calls
                to this function with different arguments will likely yield the same pointer, though the contents will likely have
                changed every time.
    \sa         rtmp_get_vid_codec_flag_name
 */
const char* rtmp_get_vid_codec_flag_list( rtmp_support_codec_vid_t bitset );

/*! \brief      Gets a human readable string containing the name of a `rtmp_support_func_vid_t` flag.
    \param      flag     The video function support flag.
    \return     A human readable C string which corresponds to \a flag.
    \remarks    This function will only recognize individual flags. If you want to pass in a whole bitset, please use
                `rtmp_get_vid_func_flag_list()`.
 */
const char* rtmp_get_vid_func_flag_name( rtmp_support_func_vid_t flag );

/*! \brief      Gets a human readable string containing the name of a whole `rtmp_support_func_vid_t` bitset.
    \param      bitset   The video function support bitset.
    \return     A human readable C string which contains a comma separated list of the names for all of the flags expressed in \a bitset.
    \remarks    The library uses an internally allocated static buffer for this operation. Subsequent calls to this function or
                similar functions will cause the return value from prior calls to no longer be valid. Additionally, repeated calls
                to this function with different arguments will likely yield the same pointer, though the contents will likely have
                changed every time.
    \sa         rtmp_get_vid_func_flag_name
 */
const char* rtmp_get_vid_func_flag_list( rtmp_support_func_vid_t bitset );

/*! \brief      Gets a human readable string containing the name of a `rtmp_so_evt_t` code.
    \param      code     The shared object event code.
    \return     A human readable C string which corresponds to \a code.
 */
const char* rtmp_get_so_evt_type_name( rtmp_so_evt_t code );

/*! \brief      Gets a human readable string containing the name of a `rtmp_usr_evt_t` code.
    \param      code     The user event code.
    \return     A human readable C string which corresponds to \a code.
 */
const char* rtmp_get_usr_evt_type_name( rtmp_usr_evt_t code );

/*! \brief      Gets a human readable string containing the name of a `rtmp_event_t` code.
    \param      code     The event code.
    \return     A human readable C string which corresponds to \a code.
 */
const char* rtmp_get_event_name( rtmp_event_t code );

/*! \brief      Gets a human readable string containing the name of a `rtmp_err_t` code.
    \param      code     The error code.
    \return     A human readable C string which corresponds to \a code.
 */
const char* rtmp_get_err_name( rtmp_err_t code );

/*! @} */
/*! @} */

#ifdef __cplusplus
}
#endif

#endif
