/*
    ieee754_double.c

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

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

typedef uint8_t byte;

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    #define DBL_BYTE(a) (7-a)
#elif __BYTE_ORDER__ == __ORDER_PDP_ENDIAN__
    #error "Intentionally unsupported"
#else
    #define DBL_BYTE(a) (a)
#endif

//Sign is most significant bit
static inline void set_sign(byte* d){
    d[DBL_BYTE(0)] |= 0x80;
}
static inline void clear_sign(byte* d){
    d[DBL_BYTE(0)] &= ~0x80;
}
static inline int get_sign(const byte*d){
    return (d[DBL_BYTE(0)] & 0x80) ? 1 : 0;
}
//Exponent starts at 2nd most significant bit and is 11 bits long
static inline void set_exp(byte* d, uint16_t exp){
    d[DBL_BYTE(0)] &= 0x80;
    d[DBL_BYTE(1)] &= 0x0F;
    d[DBL_BYTE(0)] |= (exp >> 4) & ~0x80;
    d[DBL_BYTE(1)] |= (exp & 0x0F) << 4;
}
static inline int get_exp(const byte* d){
    int ret;
    ret = (d[DBL_BYTE(0)] & ~0x80) << 4;
    ret |= (d[DBL_BYTE(1)] & ~0x0F ) >> 4;
    return ret;
}
//Fraction starts at 12th most significant bit and is 52 bits long
static inline void set_frac(byte* d, uint64_t frac){
    d[DBL_BYTE(1)] &= ~0x0F;
    d[DBL_BYTE(1)] |= (frac >> 48) & 0x0F;
    d[DBL_BYTE(2)] = (frac >> 40) & 0xFF;
    d[DBL_BYTE(3)] = (frac >> 32) & 0xFF;
    d[DBL_BYTE(4)] = (frac >> 24) & 0xFF;
    d[DBL_BYTE(5)] = (frac >> 16) & 0xFF;
    d[DBL_BYTE(6)] = (frac >> 8) & 0xFF;
    d[DBL_BYTE(7)] = (frac >> 0) & 0xFF;
}
static inline uint64_t get_frac(const byte* d){
    uint64_t ret;
    ret = (uint64_t)(d[DBL_BYTE(1)] & 0x0F) << 48;
    ret |= (uint64_t)d[DBL_BYTE(2)] << 40;
    ret |= (uint64_t)d[DBL_BYTE(3)] << 32;
    ret |= (uint64_t)d[DBL_BYTE(4)] << 24;
    ret |= (uint64_t)d[DBL_BYTE(5)] << 16;
    ret |= (uint64_t)d[DBL_BYTE(6)] << 8;
    ret |= (uint64_t)d[DBL_BYTE(7)] << 0;
    return ret;
}


double read_double_ieee(const void *ptr){
    #ifdef RTMP_ASSUME_IEEE_DOUBLES
    double ret;
    memcpy( &ret, ptr, sizeof( ret ) );
    return ret;
    #else
    const byte *d = ptr;
    double ret = 0;
    if( get_exp( d ) == 0 ){
        //Denormalized value. Value is frac * 2 ^ -1022
        ret = get_frac( d );
        ret *= 0x2p-1022;
    }
    if( get_exp(d) == 0x7FF ) {
        if( get_frac( d ) == 0 ){
            ret = 1.0/0.0; //Infinity
        }
        else{
            ret = NAN;
        }
    }
    else if (get_exp(d) > 0) {
        //A normalized value.
        ret = get_frac( d );
        //Move the bits down into the fractional portion
        ret /= 0x2p51;
        //Add the implicit leading bit
        ret += 1;
        //Multiply it by 2 ^ exp with bias correction
        ret *= pow( 2, get_exp(d) - 1023 );
    }
    if( get_sign( d ) ){
        ret *= -1;
    }
    return ret;
    #endif
}
void write_double_ieee(void *ptr, double value){
    #ifdef RTMP_ASSUME_IEEE_DOUBLES
    memcpy( ptr, &value, sizeof( value ) );
    return;
    #else
    byte d[8];
    #ifdef signbit
    if( signbit(value) != 0 ){
    #else
    if( value < 0.0 ){
    #endif // signbit
        set_sign( d );
    }
    else{
        clear_sign( d );
    }
    if( isnan(value) ){
        //Represent NaN by setting all bits
        memset( &d, 0xFF, 8 );
        clear_sign( d );
    }
    else if( isinf(value) ){
        //Represent infinity by setting all exponent bits and clearing frac bits
        set_exp( d, 0x7FF );
        set_frac( d, 0 );
    }
    else{
        //abs on value to make the math work
        if( value < 0 ){
            value = -value;
        }
        //Get exponent
        int l = floor(log(value) / log(2));
        //Clamp l in range
        if( l < -1022 ){
            l = -1023;
        }
        if( l > 1024 ){
            l = 1025;
        }

        double p = pow(2, l);
        set_exp( d, 1023 + l );
        //value/p normalizes the value.
        //Subtracting one removes the implicit leading bit (it should be trimmed off automatically)
        //Multiply by 2^51 to bring the value into integer range
        set_frac( d, (value / p) * 0x2p51 );
    }
    memcpy( ptr, &d, sizeof( d ) );
    #endif
}

int double_is_ieee754( void ){
    if( sizeof( double ) != 8 ){
        return 0;
    }
    double test_in = -1010101010101010101010101010101010.0;
    double test_out = read_double_ieee( &test_in );
    return memcmp( &test_in, &test_out, 8 ) == 0;
}
