cmake_minimum_required (VERSION 2.6)
project (libopenrtmp C)

set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99 -g -Wall" )

file(GLOB src_files "src/*.*")

add_executable( amf_test
	${src_files}
)

target_link_libraries( amf_test m )
