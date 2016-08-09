cmake_minimum_required (VERSION 2.6)
project (libopenrtmp C)
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99 -g -Wall -fstrict-aliasing" )

include_directories( "$(PROJECT_SOURCE_DIR)/include" )

if( NOT CMAKE_CROSSCOMPILING)
    try_run( SUPPORTS_IEEE754 SUPPORTS_IEEE754_BUILT "${PROJECT_BINARY_DIR}/ieeetest" "${PROJECT_SOURCE_DIR}/compile_tests/ieee.c" LINK_LIBRARIES m  )
    if( SUPPORTS_IEEE754 )
        message( STATUS "Native support for IEEE754 floats detected." )
        set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DRTMP_ASSUME_IEEE_DOUBLES" )
    endif()
endif()

file(GLOB_RECURSE src_files "src/*.*")
file(GLOB_RECURSE inc_files "include/*.*")

add_executable( amf_test
	${src_files}
	${inc_files}
)

target_link_libraries( amf_test m )
