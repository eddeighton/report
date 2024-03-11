##  Copyright (c) Deighton Systems Limited. 2022. All Rights Reserved.
##  Author: Edward Deighton
##  License: Please see license.txt in the project root folder.

##  Use and copying of this software and preparation of derivative works
##  based upon this software are permitted. Any copy of this software or
##  of any derivative work must include the above copyright notice, this
##  paragraph and the one after it.  Any distribution of this software or
##  derivative works must comply with all applicable laws.

##  This software is made available AS IS, and COPYRIGHT OWNERS DISCLAIMS
##  ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE
##  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
##  PURPOSE, AND NOTWITHSTANDING ANY OTHER PROVISION CONTAINED HEREIN, ANY
##  LIABILITY FOR DAMAGES RESULTING FROM THE SOFTWARE OR ITS USE IS
##  EXPRESSLY DISCLAIMED, WHETHER ARISING IN CONTRACT, TORT (INCLUDING
##  NEGLIGENCE) OR STRICT LIABILITY, EVEN IF COPYRIGHT OWNERS ARE ADVISED
##  OF THE POSSIBILITY OF SUCH DAMAGES.



cmake_minimum_required(VERSION 3.2)

#get boost
include( ${WORKSPACE_ROOT_PATH}/thirdparty/boost/boost_include.cmake )

#get gtest
include( ${WORKSPACE_ROOT_PATH}/thirdparty/gtest/gtest_include.cmake )

#get json
include( ${WORKSPACE_ROOT_PATH}/thirdparty/nlohmann/json_include.cmake )

#get inja
include( ${WORKSPACE_ROOT_PATH}/thirdparty/inja/inja_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )

include_directories( ${REPORT_API_DIR} )
include_directories( ${REPORT_SRC_DIR} )

set( REPORT_UNIT_TESTS
	${REPORT_TEST_DIR}/report_tests.cpp
	)

enable_testing()

add_executable( report_tests ${REPORT_UNIT_TESTS} ${REPORT_TEST_DIR}/main_test_runner.cpp )

# allow access to source folder for tests
target_include_directories( report_tests PUBLIC ${REPORT_SRC_DIR})

target_link_libraries( report_tests report )

set_target_properties( report_tests PROPERTIES FOLDER tests/unit )

link_boost( report_tests filesystem )
link_boost( report_tests system )
link_boost( report_tests date_time )
link_boost( report_tests program_options )
link_boost( report_tests iostreams )
link_boost( report_tests serialization )
link_boost( report_tests url )
link_gtest( report_tests )
link_json( report_tests )
link_inja( report_tests )
link_common( report_tests )

gtest_add_tests( TARGET report_tests SOURCES ${REPORT_UNIT_TESTS} )

install( TARGETS report_tests DESTINATION bin )

add_custom_command( TARGET report_tests POST_BUILD COMMAND report_tests WORKING_DIRECTORY ${REPORT_UNIT_TESTS_DIR} )