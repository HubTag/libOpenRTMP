cmake_minimum_required (VERSION 2.6)
project (libopenrtmp C)
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu11 -g -Wall -fstrict-aliasing -Wextra -Wno-unused-parameter -Wstrict-aliasing=2 -I$(PROJECT_SOURCE_DIR)/include" )
set( OPENRTMP_VERSION "0.0" ) #Set this automatically with git?
set( OPENRTMP_URL "http://hubtag.net" )

set( INSTALL_DIR_LIB_DEF "" )
set( INSTALL_DIR_INCLUDE_DEF "" )

if( UNIX )
    set( INSTALL_DIR_LIB_DEF "/usr/local/lib" )
    set( INSTALL_DIR_INCLUDE_DEF "/usr/local/include" )
endif()

set( INSTALL_DIR_LIB "${INSTALL_DIR_LIB_DEF}" CACHE STRING "Library installation location" )
set( INSTALL_DIR_INCLUDE "${INSTALL_DIR_INCLUDE_DEF}" CACHE STRING "Include file installation location" )



include_directories( "${PROJECT_SOURCE_DIR}/include" )



function( issue_error code_file line col err )
    message( SEND_ERROR " \r${code_file}:${line}:${col}: error: ${err}" )
endfunction()

function( issue_warning code_file line col err )
    message( STATUS " \r${code_file}:${line}:${col}: warning: ${err}" )
endfunction()

function( run_check code_file )
    if( NOT CMAKE_CROSSCOMPILING)
        set( RUN_CHECK_SUPPORT_FILE "${PROJECT_SOURCE_DIR}/compile_tests/${code_file}"  )
        set( RUN_CHECK_SUPPORT false )
        set( RUN_CHECK_SUPPORT_BUILD false )
        set( MUST_COMPILE false )
        set( LINE_NO 0 )
        file( STRINGS "${RUN_CHECK_SUPPORT_FILE}" RUN_CHECK_SUPPORT_LINES  )
        list(LENGTH RUN_CHECK_SUPPORT_LINES len)

        while( ${LINE_NO} LESS ${len} )
            list(GET RUN_CHECK_SUPPORT_LINES ${LINE_NO} LINE )
            math( EXPR LINE_NO "${LINE_NO}+1" )
            string( STRIP "${LINE}" LINE )
            string(TOLOWER "${LINE}" LINE_LOWER)
            if( NOT DEFINED IN_HEAD )
                if( "${LINE_LOWER}" STREQUAL "/* cmake test file" )
                    set( IN_HEAD "true" )
                endif()
            else()
                if( "${LINE}" STREQUAL "*/" )
                    break()
                endif()
                string(FIND "${LINE}" ":" IDX )
                if( NOT ${IDX} EQUAL -1 )
                    string(SUBSTRING "${LINE_LOWER}" 0 ${IDX} CMD)
                    string(REGEX REPLACE "[^ _a-z0-9]" "" CMD "${CMD}")
                    string(STRIP "${CMD}" CMD)
                    math( EXPR IDX "${IDX}+1" )
                    string(SUBSTRING "${LINE}" ${IDX} -1 VAL)
                    string(STRIP ${VAL} VAL)
                    if( "${CMD}" STREQUAL "description" )
                        SET( RUN_CHECK_SUPPORT_NAME "${VAL}" )
                    elseif( "${CMD}" STREQUAL "defines" )
                        SET( RUN_CHECK_SUPPORT_DEF "-D${VAL}" )
                    elseif( "${CMD}" STREQUAL "must compile" )
                        SET( MUST_COMPILE "${VAL}" )
                    endif()
                endif()
            endif()
        endwhile( ${LINE_NO} LESS ${len} )
        if( NOT DEFINED IN_HEAD )
            issue_error( "${RUN_CHECK_SUPPORT_FILE}" ${LINE_NO} 1 "No CMake test header found." )
        endif()
        if( NOT DEFINED RUN_CHECK_SUPPORT_NAME )
            issue_error( "${RUN_CHECK_SUPPORT_FILE}" ${LINE_NO} 1 "No 'Description' entry found." )
        endif()
        if( NOT DEFINED RUN_CHECK_SUPPORT_DEF )
            issue_error( "${RUN_CHECK_SUPPORT_FILE}" ${LINE_NO} 1 "No 'Defines' entry found." )
        endif()
        try_run( RUN_CHECK_SUPPORT RUN_CHECK_SUPPORT_BUILT
                    "${PROJECT_BINARY_DIR}/compile_tests"
                    "${RUN_CHECK_SUPPORT_FILE}"
                    LINK_LIBRARIES m  )
        if( RUN_CHECK_SUPPORT AND RUN_CHECK_SUPPORT_BUILT )
            message( STATUS "Platform supports ${RUN_CHECK_SUPPORT_NAME} " )
            set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${RUN_CHECK_SUPPORT_DEF}" PARENT_SCOPE )

        else()
            message( STATUS "Platform DOES NOT support ${RUN_CHECK_SUPPORT_NAME} " )
            if( NOT RUN_CHECK_SUPPORT_BUILT )
                if( MUST_COMPILE )
                    issue_error( "${RUN_CHECK_SUPPORT_FILE}" 1 1 "Compilation of this file failed where it should not have." )
                elseif( DEFINED SHOW_WARNINGS )
                    issue_warning( "${RUN_CHECK_SUPPORT_FILE}" 1 1 "Compilation of this file failed." )
                endif()
            endif()
        endif()
    endif()
endfunction()

run_check( "ieee.c" )
run_check( "strncasecmp.c" )



file(GLOB_RECURSE src_files "src/*.*")
file(GLOB_RECURSE inc_files "include/*.*")


add_library( openrtmp
	${inc_files}
	${src_files}
)

target_link_libraries( openrtmp m )

configure_file( libopenrtmp.pc.in "${CMAKE_BINARY_DIR}/libopenrtmp.pc" )

install(    TARGETS openrtmp
            DESTINATION "${INSTALL_DIR_LIB}"
            PERMISSIONS     OWNER_READ OWNER_WRITE
                            GROUP_READ
                            WORLD_READ )

install(    DIRECTORY "${PROJECT_SOURCE_DIR}/include/openrtmp"
            DESTINATION "${INSTALL_DIR_INCLUDE}"
            PATTERN "*private*" EXCLUDE
            PATTERN "*.h"
            PERMISSIONS     OWNER_READ OWNER_WRITE
                            GROUP_READ
                            WORLD_READ )

if( UNIX )
    install(    FILES "${CMAKE_BINARY_DIR}/libopenrtmp.pc"
                DESTINATION "${INSTALL_DIR_LIB}/pkgconfig"
                PERMISSIONS     OWNER_READ OWNER_WRITE
                                GROUP_READ
                                WORLD_READ )
endif()

find_package(Doxygen)
if(DOXYGEN_FOUND)
	set(DOXYFILE_IN "${PROJECT_SOURCE_DIR}/doxy.conf.in")
	set(DOXYFILE    "${CMAKE_BINARY_DIR}/doxy.conf")
	set(DOC_DEST    "${CMAKE_BINARY_DIR}/doc")
    configure_file( "${DOXYFILE_IN}" "${DOXYFILE}" )

	make_directory( "${DOC_DEST}" )
	add_custom_target(doc
                      COMMAND ${DOXYGEN_EXECUTABLE} "${DOXYFILE}"
                      WORKING_DIRECTORY "${DOC_DEST}"
                      COMMENT "Generating API documentation with Doxygen"
                      VERBATIM)
else()
	issue_warning( "${CMAKE_CURRENT_LIST_FILE}" "${CMAKE_CURRENT_LIST_LINE}" 2 "Doxygen needed to build documentation." )
endif()

