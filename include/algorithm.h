/*
    algorithm.h

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

//Return true if a is less than b. false otherwise.
typedef bool (*less_than_proc)(
    const void *a,
    const void *b
);

//Binary search. Must be done on sorted array.
size_t alg_search_bin(
    const void *needle,
    const void *haystack,
    size_t element_size,
    size_t count,
    less_than_proc less_than
);

//Linear search. May be used on non-linear array.
size_t alg_search_lin(
    const void *needle,
    const void *haystack,
    size_t element_size,
    size_t count,
    less_than_proc less_than
);



