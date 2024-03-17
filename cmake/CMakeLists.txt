cmake_minimum_required(VERSION 3.2)

project( Report )

include( ${WORKSPACE_ROOT_PATH}/src/env/src/common_settings.cmake )

#enable ctest
enable_testing()

#setup some common directories from the root of the project
set( CMAKE_PARENT_PATH ${CMAKE_CURRENT_LIST_DIR}/.. )
find_path( REPORT_ROOT_DIR NAMES tests PATHS ${CMAKE_PARENT_PATH} REQUIRED )
set( REPORT_API_DIR ${REPORT_ROOT_DIR}/include )
set( REPORT_SRC_DIR ${REPORT_ROOT_DIR}/src )
set( REPORT_TEST_DIR ${REPORT_ROOT_DIR}/tests )

add_subdirectory( report )
add_subdirectory( tests )
add_subdirectory( bench_summary )
