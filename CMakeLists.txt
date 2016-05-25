cmake_minimum_required (VERSION 2.6)
project (libopenrtmp C)

set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -g -Wall" )

add_executable( amf_test
	"${PROJECT_SOURCE_DIR}/amf_decode.c"
	"${PROJECT_SOURCE_DIR}/data_stream.c"
	"${PROJECT_SOURCE_DIR}/memutil.c"
	"${PROJECT_SOURCE_DIR}/amf_encode.c"
)
