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



cmake_minimum_required( VERSION 3.1...3.16 )

#get boost
include( ${WORKSPACE_ROOT_PATH}/thirdparty/boost/boost_include.cmake )

#get json
include( ${WORKSPACE_ROOT_PATH}/thirdparty/nlohmann/json_include.cmake )

#get inja
include( ${WORKSPACE_ROOT_PATH}/thirdparty/inja/inja_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )

#################################################################
set( JINJA_TEMPLATES 
	${REPORT_SRC_DIR}/report/templates/branch.jinja
	${REPORT_SRC_DIR}/report/templates/graph.jinja
	${REPORT_SRC_DIR}/report/templates/multiline.jinja
	${REPORT_SRC_DIR}/report/templates/report.jinja
	${REPORT_SRC_DIR}/report/templates/table.jinja
	)

set( REPORTS_HEADERS
    ${REPORT_API_DIR}/report/colours.hxx
    ${REPORT_API_DIR}/report/linker.hpp
    ${REPORT_API_DIR}/report/renderer_html.hpp
    ${REPORT_API_DIR}/report/report.hpp
    ${REPORT_API_DIR}/report/reporter_id.hpp
    ${REPORT_API_DIR}/report/value.hpp
)

set( REPORTS_SOURCE
    ${REPORT_SRC_DIR}/report/colours.cxx
    ${REPORT_SRC_DIR}/report/renderer_html.cpp
    ${REPORT_SRC_DIR}/report/value.cpp
)

add_library( report
    ${REPORTS_HEADERS}
    ${REPORTS_SOURCE}
)

set_target_properties( report PROPERTIES FOLDER report )

target_include_directories( report PUBLIC ${REPORT_API_DIR} )

link_boost( report system )
link_boost( report filesystem )
link_boost( report serialization )
link_json( report )
link_inja( report )
link_common( report )

#################################################################
#################################################################
## install everything
install( FILES ${JINJA_TEMPLATES} DESTINATION templates )
install( FILES ${REPORTS_HEADERS} DESTINATION include/report )
install( TARGETS report EXPORT report DESTINATION bin)
install( EXPORT report NAMESPACE Report:: FILE report-config.cmake DESTINATION share )

install( TARGETS report DESTINATION bin)