cmake_minimum_required (VERSION 2.6)
project (libopenrtmp CXX C)
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -Wall -fstrict-aliasing -Wextra -Wno-unused-parameter -Wstrict-aliasing=2 -fpermissive" )

include_directories( "$(PROJECT_SOURCE_DIR)/include" )

SET_SOURCE_FILES_PROPERTIES( "${PROJECT_SOURCE_DIR}/compile_tests/ieee.c" PROPERTIES LANGUAGE CXX )

if( NOT CMAKE_CROSSCOMPILING)
    try_run( SUPPORTS_IEEE754 SUPPORTS_IEEE754_BUILT "${PROJECT_BINARY_DIR}/ieeetest" "${PROJECT_SOURCE_DIR}/compile_tests/ieee.c" LINK_LIBRARIES m  )
    if( SUPPORTS_IEEE754 )
        message( STATUS "Native support for IEEE754 floats detected." )
        set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DRTMP_ASSUME_IEEE_DOUBLES" )
    endif()
endif()

file(GLOB_RECURSE src_files "src/*.*")
file(GLOB_RECURSE inc_files "include/*.*")
SET_SOURCE_FILES_PROPERTIES( ${src_files} PROPERTIES LANGUAGE CXX )


add_executable( amf_test
	${src_files}
	${inc_files}
)

target_link_libraries( amf_test m )
