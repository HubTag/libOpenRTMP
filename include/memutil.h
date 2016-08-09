/*
    memutil.h

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

#include "rtmp/rtmp_types.h"
#include "rtmp/rtmp_constants.h"
#include <string.h>
#include <stdlib.h>


#ifdef __cplusplus
template<class T> struct remove_ref       {typedef T type;};
template<class T> struct remove_ref<T&>   {typedef T type;};
template<class T> struct remove_ref<T&&>  {typedef T type;};
#define VEC_PRIV_T(ptr) remove_ref<decltype(ptr)>::type
#else
#define VEC_PRIV_T(ptr) void*
#endif

#define VEC_PRIV_SCALE_NUMERATOR 2
#define VEC_PRIV_SCALE_DENOMINATOR 1
#define VEC_PRIV_ADJUSTRES(reserve,guarantee) (reserve=reserve*VEC_PRIV_SCALE_NUMERATOR/VEC_PRIV_SCALE_DENOMINATOR+guarantee)

#define VEC_PRIV_CALCRES(base,reserve) sizeof(*base)*reserve+sizeof(size_t)*2
#define VEC_PRIV_PTR(base) (((size_t*)base)-2)
#define VEC_PRIV_BASE(ptr) ((void*)(((size_t*)ptr)+2))
#define VEC_PRIV_ACCESS(base) (VEC_PRIV_PTR(base))

#define VEC_PRIV_SHIFT(base,len,idx,amt)                                                    \
(idx==-1?                                   /*   if idx == -1 //Append to end           */  \
    ((len+=amt),                            /*      len += amt;                         */  \
    (base+len-amt))                         /*      return base + len - amt;            */  \
:                                           /*   else   //Shift stuff to make room      */  \
    (memmove(   base+idx+amt,               /*      memmove(base+idx+amt,               */  \
                base+idx,                   /*              base+idx,                   */  \
                (len-idx)*sizeof(*base)),   /*              (len-idx)*sizeof(*base));   */  \
    (len+=amt),                             /*      len += amt;                         */  \
    (base+idx)))                            /*      return base + idx;                  */


#define VEC_PRIV_MK_SPACE_AT2(base,len,reserve,idx,amt)                     /*                                                              */ \
(                                                                           /*                                                              */ \
    (len+amt<reserve)?(                                                     /*  if( len + amt < reserve )                                   */ \
        VEC_PRIV_SHIFT(base,len,idx,amt)                                    /*      return vec_shift(base, len, idx, amt);                  */ \
    ):(                                                                     /*  else {                                                      */ \
        VEC_PRIV_ADJUSTRES(reserve,amt),                                    /*      reserve = reserve * numer / denom + amt;                */ \
        VEC_PRIV_REALLOC((void**)&base, VEC_PRIV_CALCRES(base,reserve))?(   /*      if( vec_realloc(base, calc_reserve(base, reserve)) )    */ \
            VEC_PRIV_SHIFT(base,len,idx,amt)                                /*          return vec_shift(base, len, idx, amt);              */ \
        ):(                                                                 /*      else                                                    */ \
            (VEC_PRIV_T(base))0                                             /*          return nullptr;                                     */ \
        )                                                                   /*  }                                                           */ \
    )                                                                       /*                                                              */ \
 )


#define VEC_PRIV_MK_SPACE_AT(base,len,reserve,idx,amt) VEC_PRIV_MK_SPACE_AT2((base),(len),(reserve),(idx),(amt))
#define VEC_PRIV_MK_SPACE(base,len,reserve) VEC_PRIV_MK_SPACE_AT(base,len,reserve,-1,1)
#define VEC_PRIV_RM_AT(base,len,reserve,idx,amt) \
do{                                                                             \
    if(idx!=-1&&idx+amt<len){                                                   \
        memmove(base+idx,base+idx+amt,                                          \
                (len-idx-amt) * sizeof(*base));                                 \
    }                                                                           \
    if(idx==-1||idx+amt<=len){                                                  \
        len-=amt;                                                               \
    }                                                                           \
    else{                                                                       \
        len = idx;                                                              \
    }                                                                           \
    if( len * 2 < reserve ){                                                    \
        reserve /= 2;                                                           \
        VEC_PRIV_REALLOC((void**)&base, VEC_PRIV_CALCRES(base,reserve)); \
    }                                                                           \
}while(0)

#define VEC_PRIV_RESERVE(base,len,reserve,amt) do{if(reserve<amt){void*tmp=realloc(base,amt*sizeof(*base));base=tmp?tmp:base;reserve=amt;}}while(0)


#define VEC_PRIV_RESERVED(name) (VEC_PRIV_ACCESS(name)[1])
#define VEC_DECLARE(type) type *
#define VEC_INIT(vec) (vec=(VEC_PRIV_T(vec))VEC_PRIV_BASE(calloc(1,sizeof(size_t)*2)))

#define VEC_INSERT_N(name,idx,n) VEC_PRIV_MK_SPACE_AT(name,VEC_SIZE(name),VEC_PRIV_RESERVED(name),idx,n)
#define VEC_PUSH_N(name,n) VEC_INSERT_N(name,-1,n)
#define VEC_INSERT(name,idx) VEC_INSERT_N(name,idx,1)
#define VEC_PUSH(name) VEC_INSERT_N(name,-1,1)

#define VEC_ERASE_N(name,idx,n) VEC_PRIV_RM_AT(name,VEC_SIZE(name),VEC_PRIV_RESERVED(name),idx,n)
#define VEC_ERASE(name,idx) VEC_ERASE_N(name,idx,1)
#define VEC_POP_N(name,n) VEC_ERASE_N(name,-1,n)
#define VEC_POP(name) VEC_ERASE_N(name,-1,1)

#define VEC_RESERVE(name,amt) VEC_PRIV_RESERVE(name,VEC_SIZE(name),VEC_PRIV_RESERVED(name), (amt))
#define VEC_AT(name,idx) (name[idx])
#define VEC_BACK(name) VEC_AT(name, VEC_SIZE(name)-1)
#define VEC_SIZE(name) (VEC_PRIV_ACCESS(name)[0])
#define VEC_DESTROY(name) ((name?(free(VEC_PRIV_PTR(name)),0):0),name=(VEC_PRIV_T(name))0)


inline void * VEC_PRIV_REALLOC(void**ptr, size_t size){
	void *ret = realloc(VEC_PRIV_PTR(*ptr), size);
	if( ret ){
		*ptr = VEC_PRIV_BASE(ret);
	}
	return ret;
}

//memcpy that will reverse byte order if the machine is little endian
void ntoh_memcpy(void * restrict dst, const void * restrict src, size_t len);

//Read a short from a buffer based on endianess
short ntoh_read_s(const void *src);

//Read an int from a buffer based on endianess
int ntoh_read_d(const void *src);

//Read a short from a buffer based on endianess
uint16_t ntoh_read_us(const void *src);

//Read an int from a buffer based on endianess
uint32_t ntoh_read_ud(const void *src);

//Write a short to a buffer based on endianess
void ntoh_write_s(void *dst, short value);

//Write an int to a buffer based on endianess
void ntoh_write_d(void *dst, int value);

//Write a short to a buffer based on endianess
void ntoh_write_us(void *dst, uint16_t value);

//Write an int to a buffer based on endianess
void ntoh_write_ud(void *dst, uint32_t value);

void htol_write_ud(void *dst, uint32_t value);
uint32_t ltoh_read_ud(void *src);


//Write a 3 byte int to a buffer based on endianess
void ntoh_write_ud3(void *dst, uint32_t value);
//Read a 3 byte int from a buffer based on endianess
uint32_t ntoh_read_ud3(void *dst);

//strlen which considers nullptr to be a zero-length string.
size_t strlen_check( const char *str );

//strcpy which returns str + strlen( appending ).
char *strwrite( char * restrict str, const char * restrict appending );

void emit_err(const char* err);

int32_t timestamp_get_delta( rtmp_time_t stamp1, rtmp_time_t stamp2 );

//If generating of allocating a nonce, it MUST be initialized to null!
rtmp_err_t rtmp_nonce_gen(void **nonce, size_t length);
rtmp_err_t rtmp_nonce_alloc(void **nonce, size_t length);
rtmp_err_t rtmp_nonce_del(void **nonce);

rtmp_time_t rtmp_get_time();

unsigned long long si_convert_ull(unsigned long long number, const si_prefix from, const si_prefix to);

byte* safe_alloc(size_t amount);


char * str_dupl( const char * input, size_t len );
char * str_dup( const char * input );
