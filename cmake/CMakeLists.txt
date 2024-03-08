cmake_minimum_required(VERSION 3.2)

project( Report )

#enable ctest
enable_testing()

#insist on ALL projects using c++ 17
set( CMAKE_CXX_STANDARD 20 )

add_compile_options("-Wshadow")
add_compile_options("-Wfatal-errors")
add_compile_options("-Wpessimizing-move")

#setup some common directories from the root of the project
set( CMAKE_PARENT_PATH ${CMAKE_CURRENT_LIST_DIR}/.. )
find_path( REPORT_ROOT_DIR NAMES tests PATHS ${CMAKE_PARENT_PATH} REQUIRED )
set( REPORT_API_DIR ${REPORT_ROOT_DIR}/include )
set( REPORT_SRC_DIR ${REPORT_ROOT_DIR}/src )
set( REPORT_TEST_DIR ${REPORT_ROOT_DIR}/tests )

#ensure language runtimes and other system libs are copied to installation folder
include (InstallRequiredSystemLibraries)

set_property(GLOBAL PROPERTY CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
set_property(GLOBAL PROPERTY POSITION_INDEPENDENT_CODE ON)

add_subdirectory( report )
add_subdirectory( tests )
