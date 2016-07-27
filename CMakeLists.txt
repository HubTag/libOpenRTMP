cmake_minimum_required (VERSION 2.6)
project (libopenrtmp C)
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99 -g -Wall -fstrict-aliasing" )

include_directories( "$(PROJECT_SOURCE_DIR)/include" )

file(GLOB_RECURSE src_files "src/*.*")
file(GLOB_RECURSE inc_files "include/*.*")

add_executable( amf_test
	${src_files}
	${inc_files}
)

target_link_libraries( amf_test m )
