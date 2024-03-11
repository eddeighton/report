//  Copyright (c) Deighton Systems Limited. 2022. All Rights Reserved.
//  Author: Edward Deighton
//  License: Please see license.txt in the project root folder.

//  Use and copying of this software and preparation of derivative works
//  based upon this software are permitted. Any copy of this software or
//  of any derivative work must include the above copyright notice, this
//  paragraph and the one after it.  Any distribution of this software or
//  derivative works must comply with all applicable laws.

//  This software is made available AS IS, and COPYRIGHT OWNERS DISCLAIMS
//  ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
//  PURPOSE, AND NOTWITHSTANDING ANY OTHER PROVISION CONTAINED HEREIN, ANY
//  LIABILITY FOR DAMAGES RESULTING FROM THE SOFTWARE OR ITS USE IS
//  EXPRESSLY DISCLAIMED, WHETHER ARISING IN CONTRACT, TORT (INCLUDING
//  NEGLIGENCE) OR STRICT LIABILITY, EVEN IF COPYRIGHT OWNERS ARE ADVISED
//  OF THE POSSIBILITY OF SUCH DAMAGES.

#include <gtest/gtest.h>

#include "report/report.hpp"
#include "report/renderer_html.hpp"

#include "common/file.hpp"

#include <sstream>

extern boost::filesystem::path g_resultDir;

enum Foobar
{
    eOne,
    eTwo
};
std::ostream& operator<<( std::ostream& os, Foobar v )
{
    switch( v )
    {
        case eOne:
            return os << "One";
        case eTwo:
            return os << "Two";
        default:
            THROW_RTE( "Unknown value of Foobar" );
            return os;
    }
}

TEST( Report, Basic )
{
    using namespace std::string_literals;

    using V = std::variant< int, std::string, Foobar >;

    using namespace report;

    using Cont = Container< V >;
    using B    = Branch< V >;
    using L    = Line< V >;
    using M    = Multiline< V >;
    using T    = Table< V >;
    using G    = Graph< V >;

    // clang-format off
    Cont c =
    B
    { 
        { "Report.Basic"s }, 
        { 
            L{ "T1"s }, 
            B
            { 
                { "Nested"s }, 
                { L{ "T2a"s }, L{ "T2b"s } } 
            }, 
            G
            {
                // nodes
                { 
                    { { { "Node1"s, eOne }, { 1, 2 } } },
                    { { { "Node2"s, eTwo } } },
                    { { { "Node3"s, eOne } } }  
                },
                // edges
                {
                    { 0, 1 },
                    { 1, 2, Colour::red },
                    { 2, 0, Colour::blue, G::Edge::Style::dotted }
                },
                // subgraphs
                {
                    {
                        {},
                        { 1, 2 }
                    }
                }
            },
            L{ "T3"s },
            T{ { "H1"s, "H2"s, "H3"s },
            {
                { L{ 1 }, L{ 2 }, L{ eOne } },
                { L{ 1 }, L{ 2 }, L{ eTwo } },
                { L{ 1 }, L{ 2 }, L{ eOne } },
                { L{ 1 }, L{ 2 }, L{ eTwo } }
            }}
        } 
    };
    // clang-format on

    std::ostringstream os;
    renderHTML( c, os );

    auto pFile = boost::filesystem::createNewFileStream( g_resultDir / "basic.html" );
    *pFile << os.str();
}

TEST( Report, Custom )
{
    using namespace std::string_literals;

    using V = std::variant< int, std::string, Foobar >;

    using namespace report;

    using Cont = Container< V >;
    using B    = Branch< V >;
    using L    = Line< V >;

    Cont c = B{ { "Report.Custom"s }, { L{ "T1"s }, B{ {}, { L{ "T2a"s }, L{ "T2b"s } } }, L{ "T3"s } } };

    HTMLTemplateEngine templateEngine{ false };

    std::ostringstream os;
    renderHTML( c, os, templateEngine );

    auto pFile = boost::filesystem::createNewFileStream( g_resultDir / "custom.html" );
    *pFile << os.str();
}
