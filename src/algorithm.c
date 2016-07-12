/*
    algorithm.c

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


#include "rtmp_types.h"
#include "algorithm.h"

static bool equals( const void* a, const void *b, less_than_proc less_than ){
    return (!less_than(a, b)) && (!less_than(b, a));
}
static bool greater_than( const void* a, const void *b, less_than_proc less_than ){
    return less_than(b, a) ;
}

/*
Unused for now.

static bool greater_than_equals( const void* a, const void *b, less_than_proc less_than ){
    return !less_than(a, b) ;
}
static bool less_than_equals( const void* a, const void *b, less_than_proc less_than ){
    return !less_than(b, a);
}*/


size_t alg_search_bin( const void *needle, const void *haystack, size_t element_size, size_t count, less_than_proc less_than ){
    //Redefine here for ease of use
    const char *d = haystack;
    size_t lower_bound = 0;
    size_t upper_bound = count;
    size_t index = 0;
    while( lower_bound < upper_bound ) {
        index = lower_bound + ( upper_bound - lower_bound ) / 2;
        //If needle is less than haystack[index], move upper_bound to index
        if( less_than( needle, d + (element_size * index) ) ){
            upper_bound = index;
        }
        //Else if needle is greater than haystack[index], move lower_bound to index+i,
        //as index needn't ever be checked again
        else if( greater_than( needle, d + (element_size * index), less_than ) ) {
            lower_bound = index + 1;
        }
        //Otherwise, we've found the item. No need to check again since equals is defined
        //as (not less than) and (not greater than)
        else {
            return index;
        }
    }
    return index;
}

size_t alg_search_lin( const void *needle, const void *haystack, size_t element_size, size_t count, less_than_proc less_than ){
    const char *d = haystack;
    for( size_t i = 0; i < count; ++i ){
        if( equals( needle, d + (element_size * i), less_than ) ){
            return i;
        }
    }
    return count;
}



