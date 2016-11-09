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

#ifndef RTMP_H_AMF_OBJECT_H
#define RTMP_H_AMF_OBJECT_H

#ifdef __cplusplus
extern "C" {
#endif


#include <openrtmp/amf/amf_constants.h>
#include <openrtmp/rtmp/rtmp_types.h>
#include <stdarg.h>


/*! \addtogroup amf_ref AMF
    @{
*/
/*!
    \struct     amf_value_t
    \brief      Refers to a value encapsulated within an `amf_t`
    \remarks    \parblock
                The state of an AMF value is dependent on the underlying AMF object. If the underlying AMF object is
                destroyed, then all AMF values which descend from the object will be invalidated.

                Additionally, actual primitive values should be acquired through the AMF object getter functions.
                \endparblock
 */
typedef union amf_value * amf_value_t;

/*! \struct     amf_t
    \brief      Represents a generic AMF abstraction.
    \remarks    \parblock
                It currently only implements AMF0 internally.

                When pushing values to an AMF object, there is a notion of push slots or push position. This is merely
                the logical name for the area where the next pushed value will reside. This is always the deepest
                nested object or array which has not yet been closed.

                If there are no complex values in an AMF object, a pushed value just gets inserted at the end of the
                AMF object. However, if the last item in the AMF object is a complex value which hasn't been closed,
                then the item is added to the end of that complex value, unless the end of that complex value is in and
                of itself a complex value, in which case the algorithm will recurse until it finds a location which
                follows a complete value, and inserts the pushed value there.

                Some rough illustrations:

                If there is no complex value at the end of our object:
                \code
                    String: "Foo"
                    Number: 123
                    // Next insert will be here.
                \endcode

                If the last value is an open object:
                \code
                    String: "Foo"
                    Number: 123
                    Object: {
                    String:     memb: "bar"
                                // Next insert will be here.
                \endcode

                If the last value is an open object with a nested open object at the end:
                \code
                    String: "Foo"
                    Number: 123
                    Object: {
                    String:     memb: "bar"
                    Object:     {
                    String:         thing: "A thing"
                                    // Next insert will be here

                \endcode

                If the last value is a closed object:
                \code
                    String: "Foo"
                    Number: 123
                    Object: {
                    String:     memb: "bar"
                    Object:     {
                    String:         thing: "A thing"
                                }
                    String:     another: "So much data"
                            }
                    // Next Insert will be here.
                \endcode
                \endparblock
 */
typedef struct amf_object * amf_t;

/*!
    @}
*/

/*! \brief      Creates an AMF object abstraction for manipulating and reading AMF data.
    \param      type    The version of AMF that this object should represent. Currently, only AMF0 is supported.
    \return     On success, the return value is a valid `amf_t` pointer.
    \return     On failure, `nullptr` is returned.
    \remarks    As of now, only AMF version 0 is supported, and the API mostly reflects this.
                When AMF3 support is added, the API is almost certainly going to change to be less coupled with the
                underlying value representations.
    \memberof   amf_t
    \sa         amf_reference
 */
amf_t amf_create( char type );

/*! \brief      Increments the reference count on an AMF object.
    \param      other     An AMF object to take a reference from.
    \return     Returns \a other
    \remarks    This will increase an internal reference count for \a other. An additional `amf_destroy()` call must be
                made for every time `amf_reference()` is called on an `amf_t`.
    \memberof   amf_t
    \sa         amf_create
 */
amf_t amf_reference( amf_t other );

/*! \brief      Decreases the reference count and/or destroys an AMF object.
    \param      amf     The AMF object which should be destroyed.
    \return     This function returns no value.
    \remarks    If the AMF object has been referenced, an additional call to `amf_destroy()` must be made for every time
                it was referenced before it will actually be destroyed.
    \memberof   amf_t
 */
void amf_destroy( amf_t amf );

/*! \brief      Serializes AMF data into a buffer.
    \param          amf     The AMF object to serialize
    \param[out]     dest    The buffer which will receive the serialized data. This may be `nullptr`, which is useful to
                            determine the size of the serialized data without actually having a buffer to receive it.
    \param          size    The size of \a dest
    \param[in,out]  written An optional reference to a variable which is used to control serialization. If it is `nullptr`,
                            then an attempt is made to serialize the whole message. \n
                            When set, the initial value indicates the index of the value to begin serialization from.
                            Upon returning, the value is set to the index after the last index which was serialized.
    \return     On success, the return value is the number of bytes written, unless \a dest is `nullptr`, in which case the
                returned value is the number of bytes which would have been written, given an adequately large buffer.
    \return     On failure, the return value is an AMF error code.
    \memberof   amf_t
    \sa amf_write_value
 */
amf_err_t amf_write( amf_t amf, byte *dest, size_t size, size_t *written );

/*! \brief      Deserializes AMF data from a buffer.
    \param          amf     The AMF object to read AMF data into.
    \param          src     The buffer which holds the AMF data to deserialize.
    \param          size    The size of \a src.
    \param[in,out]  read    An optional reference to a variable which, upon failure, will be filled with the number of bytes which
                            were read from \a src. This value is not filled on success.
    \return     On success, the number of bytes which were read is returned. Otherwise, an AMF error code is returned.
    \remarks    If the error code is `AMF_ERR_INCOMPLETE`, then it is safe to advance the buffer by the amount stored in \a read,
                fill it with more data, and call this function again. This failure mode is for reading AMF data out of multiple
                packets of data. Other error codes indicate a deeper failure in the data, and as a result, indicate that processing
                of the message should be aborted.
    \memberof   amf_t
 */
amf_err_t amf_read( amf_t amf, const byte *src, size_t size, size_t *read );

/*! \brief      Serializes a single AMF value into a buffer.
    \param      value   The AMF value to serialize.
    \param[out] dest    The buffer which will receive the serialized data. This may be `nullptr`, which is useful to determine the
                        size of the serialized data without actually having a buffer to receive it.
    \param      size    The size of \a dest.
    \return     On success, the return value is the number of bytes written, unless \a dest is `nullptr`, in which case the returned
                value is the number of bytes which would have been written, given an adequately large buffer.
    \return     On failure, the return value is an AMF error code.
    \remarks    `amf_write()` calls this function internally on all of its members. If you wish to shortcircuit `amf_write()`, this is how you do it.
    \memberof   amf_t
    \sa amf_write
 */
amf_err_t amf_write_value( amf_value_t value, byte *dest, size_t size );

/*! \brief      Pushes a double into the current push location.
    \param      amf     The AMF object into which the value will be pushed.
    \param      number  The numeric value to push.
    \return     An AMF error code.
    \remarks    For more information about push locations, see `amf_t`.
    \memberof   amf_t
    \sa amf_push_integer
 */
amf_err_t amf_push_double( amf_t amf, double number );

/*! \brief      Pushes an integer into the current push location.
    \param      amf     The AMF object into which the value will be pushed.
    \param      number  The numeric value to push.
    \return     An AMF error code.
    \remarks    For more information about push locations, see `amf_t`.
    \memberof   amf_t
    \sa amf_push_double
 */
amf_err_t amf_push_integer( amf_t amf, int32_t number );

/*! \brief      Pushes a boolean into the current push location.
    \param      amf     The AMF object into which the value will be pushed.
    \param      boolean The boolean value to push.
    \return     An AMF error code.
    \remarks    For more information about push locations, see `amf_t`.
    \memberof   amf_t
 */
amf_err_t amf_push_boolean( amf_t amf, char boolean );

/*! \brief      Pushes a string into the current push location.
    \param      amf     The AMF object into which the value will be pushed.
    \param      str     The string buffer to push. \n
                        This value may be allocated with a previous call to `amf_push_string_alloc()`. If it was, this function
                        will consume the buffer. Otherwise, the input will be interpreted as a C string, and a copy will be
                        made internally.
    \return     An AMF error code.
    \remarks    For more information about push locations, see `amf_t`.
    \memberof   amf_t
    \sa amf_push_string_alloc amf_push_long_string
 */
amf_err_t amf_push_string( amf_t amf, const void *str );

/*! \brief      Starts an object at the current push location.
    \param      amf     The AMF object in which the object will be started.
    \return     An AMF error code.
    \remarks    For more information about push locations, see `amf_t`.
    \memberof   amf_t
 */
amf_err_t amf_push_object_start( amf_t amf );

/*! \brief      Starts an ECMA array at the current push location.
    \param      amf             The AMF object in which the array will be started.
    \param      assoc_members   The number of associative members which will be pushed. After this number of members are pushed,
                                all future members pushed into this array will be pushed as ordinal members.
    \return     An AMF error code.
    \remarks    For more information about push locations, see `amf_t`.
    \memberof   amf_t
    \sa amf_push_member amf_push_object_end
 */
amf_err_t amf_push_ecma_start( amf_t amf, uint32_t assoc_members );

/*! \brief      Pushes a member name for the current push location.
    \param      amf     The AMF object into which the member will be pushed.
    \param      str     A string buffer which contains the name of the member. \n
                        This value may be allocated with a previous call to `amf_push_string_alloc()`. If it was, this function will
                        consume the buffer. Otherwise, the input will be interpreted as a C string, and a copy will be made internally.
    \return     An AMF error code.
    \remarks    All associative values use this function to set the names for any keys. This includes both ECMA arrays and objects.
    \remarks    If an AMF push function returns `AMF_ERR_NEED_NAME`, that means this function needs to be called to set the member name
                for the next value.
    \remarks    For more information about push locations, see `amf_t`.
    \memberof   amf_t
    \sa amf_push_member amf_push_object_end
 */
amf_err_t amf_push_member( amf_t amf, const void *str );

/*! \brief      Pushes a null value into the current push location.
    \param      amf     The AMF object into which the value will be pushed.
    \return     An AMF error code.
    \remarks    For more information about push locations, see `amf_t`.
    \memberof   amf_t
    \sa amf_push_string_alloc amf_push_object_start amf_push_ecma_start
 */
amf_err_t amf_push_null( amf_t amf );

/*! \brief      Pushes a value with undefined type into the current push location.
    \param      amf     The AMF object into which the value will be pushed.
    \return     An AMF error code.
    \remarks    For more information about push locations, see `amf_t`.
    \memberof   amf_t
 */
amf_err_t amf_push_undefined( amf_t amf );

/*! \brief      Pushes a value with an unsupported type into the current push location.
    \param      amf     The AMF object into which the value will be pushed.
    \return     An AMF error code.
    \remarks    For more information about push locations, see `amf_t`.
    \memberof   amf_t
 */
amf_err_t amf_push_unsupported( amf_t amf );

/*! \brief      Pushes a reference offset into the current push location.
    \param      amf     The AMF object into which the value will be pushed.
    \param      ref     The reference offset to push.
    \return     An AMF error code.
    \remarks    Reference offsets are simply a numeric value indicating the nth object value which was pushed into this AMF object.
                As of now, there is minimal support for references.
    \remarks    For more information about push locations, see `amf_t`.
    \memberof   amf_t
 */
amf_err_t amf_push_reference( amf_t amf, unsigned int ref );

/*! \brief      Indicates the end of the complex value where values are currently being pushed.
    \param      amf     The AMF object in which the value will be closed.
    \return     An AMF error code.
    \remarks    Complex values are ECMA arrays and object values.
    \remarks    Attempting to call this function on an AMF object which has no open complex values to close will invoke undefined behavior.
    \remarks    Attempting to close an ECMA array before all associative members have been pushed will invoke undefined behavior.
    \remarks    For more information about push locations, see `amf_t`.
    \memberof   amf_t
 */
amf_err_t amf_push_object_end( amf_t amf );

/*! \brief      Pushes a date value into the current push location.
    \param      amf         The AMF object into which the value will be pushed.
    \param      timestamp   The Unix timestamp to push.
    \param      timezone    The timezone for this date, given in an offset from UTC in minutes. However, this is officially reserved
                            by the AMF0 specification, and should always be 0.
    \return     An AMF error code.
    \remarks    A Unix timestamp is the number of seconds which have elapsed since 00:00:00 on January 1st, 1970 UTC. Unix timestamps
                do not count leap seconds. It is a very common standard format, so a cursory search should give you all the information
                you need on Unix timestamps.
    \remarks    For more information about push locations, see `amf_t`.
    \memberof   amf_t
 */
amf_err_t amf_push_date( amf_t amf, double timestamp, int16_t timezone );

/*! \brief      Pushes a long string into the current push location.
    \param      amf     The AMF object into which the value will be pushed.
    \param      str     The string buffer to push. \n
                        This value may be allocated with a previous call to `amf_push_string_alloc()`. If it was, this function
                        will consume the buffer. Otherwise, the input will be interpreted as a C string, and a copy will be
                        made internally.
    \return     An AMF error code.
    \remarks    Long strings are standard strings which may occupy more bytes than a normal string. libOpenRTMP will automatically
                convert a normal string to a long string if needed, though.
    \remarks    For more information about push locations, see `amf_t`.
    \memberof   amf_t
    \sa amf_push_string_alloc amf_push_string amf_push_xml
 */
amf_err_t amf_push_long_string( amf_t amf, const void *str );

/*! \brief      Pushes a string into the current push location.
    \param      amf     The AMF object into which the value will be pushed.
    \param      xml     The string buffer to push. \n
                        This value may be allocated with a previous call to `amf_push_string_alloc()`. If it was, this function
                        will consume the buffer. Otherwise, the input will be interpreted as a C string, and a copy will be
                        made internally.
    \return     An AMF error code.
    \remarks    XML documents are basically just long strings. They are given their own name, but internally, they are represented
                exactly like a long string, just with a different name.
    \remarks    For more information about push locations, see `amf_t`.
    \memberof   amf_t
    \sa amf_push_string_alloc amf_push_long_string amf_push_string
 */
amf_err_t amf_push_xml( amf_t amf, const void *xml );




//Used to prepare a buffer allocation for all string data, that being:
//Strings
//Long strings
//Member names
//XML documents
/*! \brief      Prepares a buffer into which strings may be written.
    \param      amf             The AMF object for which the buffer is going to be allocated.
    \param[out] destination     A reference to a `void *` which will receive the buffer pointer.
    \param      length          The length in bytes of the buffer which is being requested. This must be the length of the string
                                without a null terminator. All functions assume that there are no null terminators on the buffers
                                and will use the length of the buffer as the string length. As such, overallocating will lead to
                                uninitialized access to the end of the buffer.
    \return     An AMF error code.
    \remarks    \parblock
                All of the push functions which take string values in the AMF library rely on this function to allocate space for
                the strings. First, this function is called to allocate a buffer, then the string is `memcpy()`'d into the allocated
                buffer, and then the buffer is pushed.

                If the user passes in an external string buffer to a push function, then this function is called internally to make
                a copy of the string.

                After calling `amf_push_string_alloc()`, the buffer should be pushed into the object before calling it again. There is
                a single internal slot which is used to store a reference to the allocation, and if that slot is not emptied with a
                push prior to calling the function again, memory will be leaked. Attempts to push an old buffer after overwriting the
                slot will result in the object not realizing that it's an internal allocation, and will treat it like a normal string
                buffer. Since a null pointer won't have been written to the end of the buffer, this will not only leak memory, but it
                will also invoke undefined behavior.
                \endparblock
    \memberof   amf_t
    \sa amf_push_string amf_push_member amf_push_long_string amf_push_xml
 */
amf_err_t amf_push_string_alloc( amf_t amf, void** destination, size_t length );

//Return number of items in the AMF object
/*! \brief      Returns the number of values in an AMF object.
    \param      amf     The AMF object whose member count will be fetched.
    \return     The number of values in \a amf.
    \memberof   amf_t
 */
size_t amf_get_count( amf_t amf );

//Returns an item by index
/*! \brief      Returns the AMF value at the given index in an AMF object.
    \param      amf     The AMF object from which the AMF value will be fetched.
    \param      idx     The index of the value to fetch.
    \return     The AMF value from `amf` at index \a idx.
    \remarks    Attempting to access an index which is out of bounds is undefined.
    \memberof   amf_t
 */
amf_value_t amf_get_item( amf_t amf, const size_t idx );

#define AMF_PRIV_NULL_0() AMF_TYPE_NULL
#define AMF_PRIV_NULL_1(A) AMF_TYPE_NULL, A
#define AMF_PRIV_NULL_I(x,A,FUNC, ...)  FUNC
#define AMF_PRIV_NULL(...)  AMF_PRIV_NULL_I(__VA_ARGS__, AMF_PRIV_NULL_1(__VA_ARGS__),AMF_PRIV_NULL_0(__VA_ARGS__) )
//#define AMF_NULL(...)  AMF_PRIV_NULL(0, __VA_ARGS__)
#define AMF_NULL(...) AMF_TYPE_NULL, "" __VA_ARGS__

#define AMF_ARR_ORD(...) AMF_TYPE_ECMA_ARRAY_ASSOC_END, "", __VA_ARGS__
#define AMF_ARR(...) AMF_TYPE_ARRAY, __VA_ARGS__, AMF0_TYPE_OBJECT_END, ""
#define AMF_DBL(...) AMF_TYPE_DOUBLE, __VA_ARGS__
#define AMF_INT(...) AMF_TYPE_INTEGER, __VA_ARGS__
#define AMF_STR(...) AMF_TYPE_STRING, __VA_ARGS__
#define AMF_BOOL(...) AMF_TYPE_BOOLEAN, __VA_ARGS__
#define AMF_OBJ(...) AMF_TYPE_OBJECT, __VA_ARGS__ , AMF_TYPE_OBJECT_END, ""
#define AMF(...) __VA_ARGS__ , AMF_TYPE_NONE


/*! \brief      Pushes an aggregate of values into an AMF object in a single call.
    \param      amf     The AMF object
    \param      list    The variadic function parameter list of value types and values to push.
    \return     An AMF error code.
    \remarks    This is just the `va_list` variant of `amf_push_simple()`. See that page for more information.
    \memberof   amf_t
    \sa \ref creating_amf_push_simple amf_push_simple
 */
amf_err_t amf_push_simple_list( amf_t amf, va_list list );

/*! \brief      Pushes an aggregate of values into an AMF object in a single call.
    \param      amf     The AMF object
    \param      ...     A list of value types and values to push.
    \return     An AMF error code.
    \remarks    This function is covered in much more detail in \ref creating_amf_push_simple \n
                On that page, usage examples and helper macros are displayed and explained.
    \remarks    For more information about push locations, see `amf_t`.
    \memberof   amf_t
    \sa \ref creating_amf_push_simple
 */
amf_err_t amf_push_simple( amf_t amf, ... );



//Returns true if the given value is of the specified type.
//Reference values will return true if the type is reference
//or if the type matches the referenced value's type.
bool amf_value_is( amf_value_t value, const amf_type_t type );

//Returns true if the type of the specified value is compatible
//with the given type. Compatibility means that the given value
//may be read as the given type.
//Compatibility is specified by the following table. Types are
//automatically compatible with themselves.
//
// Actual Type      | Compatible Types
//------------------|--------------------------------------------
// number           | date
// boolean          |
// string           | long string, xml document
// object           | typed object
// movieclip        |
// null             | number, boolean, object
// undefined        |
// reference        | based on type of referenced value
// ECMA array       | strict array
// strict array     | ECMA array
// date             | number
// long string      | string, xml document
// unsupported      |
// recordset        |
// XML document     | string, long string
// typed object     | object
//
bool amf_value_is_like( amf_value_t value, const amf_type_t type );

double amf_value_get_double( amf_value_t target );
int32_t amf_value_get_integer( amf_value_t target );
bool amf_value_get_bool( amf_value_t target );
const char* amf_value_get_string( amf_value_t target, size_t *length );
size_t amf_value_get_ref( amf_value_t target );
double amf_value_get_date( amf_value_t target, int16_t *timezone );
const char* amf_value_get_xml( amf_value_t target, size_t *length );


//Get member from object by key.
amf_value_t amf_obj_get_value( amf_value_t target, const char *key );

//Get member from object by index. Key is returned if the arguments are not null.
amf_value_t amf_obj_get_value_idx( amf_value_t target, size_t idx, char **key, size_t *key_len );

//Get the number of members in object
size_t amf_obj_get_count( amf_value_t target );

//Same as with the obj functions, but with arrays
//Associative values
amf_value_t amf_arr_get_assoc_value( amf_value_t target, const char *key );
amf_value_t amf_arr_get_assoc_value_idx( amf_value_t target, size_t idx, char **key, size_t *key_len );
size_t amf_arr_get_assoc_count( const amf_value_t target );

//Ordinal values
amf_value_t amf_arr_get_ord_value( amf_value_t target, const char *key );
amf_value_t amf_arr_get_ord_value_idx( amf_value_t target, size_t idx, char **key, size_t *key_len );
size_t amf_arr_get_ord_count( const amf_value_t target );

//And then a generic version which can be used to fetch any associative data from an array or object.
amf_value_t amf_assoc_get_value( amf_value_t target, const char *key );
amf_value_t amf_assoc_get_value_idx( amf_value_t target, size_t idx, char **key, size_t *key_len );
size_t amf_assoc_get_count( const amf_value_t target );


void amf_print_value( amf_value_t val );
void amf_print( amf_t val );


#ifdef __cplusplus
}
#endif

#endif
