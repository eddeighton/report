
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

#include "report/report.hpp"
#include "report/renderer_html.hpp"

#include "common/file.hpp"
#include "common/git.hpp"
#include "common/string.hpp"
#include "common/assert_verify.hpp"

#include <nlohmann/json.hpp>

#include <boost/program_options.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/algorithm/string.hpp>

#include <iostream>
#include <sstream>
#include <variant>

namespace
{
using namespace std::string_literals;

using Value = std::variant< double, int, std::string >;

using URL    = report::URL;
using Colour = report::Colour;

using ValueVector     = report::ValueVector< Value >;
using ContainerVector = report::ContainerVector< Value >;

using Report    = report::Container< Value >;
using Branch    = report::Branch< Value >;
using Line      = report::Line< Value >;
using Multiline = report::Multiline< Value >;
using Table     = report::Table< Value >;
using Plot      = report::Plot< Value >;
using Graph     = report::Graph< Value >;

using ResultHistory    = std::vector< nlohmann::json >;
using ResultHistoryMap = std::map< std::string, ResultHistory >;

Report makeSummaryReport( const ResultHistoryMap& results )
{
    Branch report{ { "Benchmark Summary Report"s } };

    for( const auto& [ benchmark, resultSequence ] : results )
    {
        Branch branch{ { benchmark } };

        std::map< std::string, Plot > plots;

        for( const auto& r : resultSequence )
        {
            // "context": {
            //     "date": "2024-03-13T02:01:32+00:00",
            //     "host_name": "reddwarf",
            //     "executable": "/build/linux_gcc_static_retail/search/build/bench/benchmarks",
            //     "num_cpus": 12,
            //     "mhz_per_cpu": 2700,
            //     "cpu_scaling_enabled": true,

            const auto context = r[ "context" ];
            // const std::string hostName = context[ "hostname" ];
            const std::string date = context[ "date" ];

            for( const auto& b : r[ "benchmarks" ] )
            {
                std::string name = b[ "name" ];
                {
                    using boost::algorithm::replace_all;
                    replace_all( name, "_", " " );
                }

                VERIFY_RTE( !name.empty() );

                // "name": "BM_3By3By1_Line3_3Deep",
                // "family_index": 0,
                // "per_family_instance_index": 0,
                // "run_name": "BM_3By3By1_Line3_3Deep",
                // "run_type": "iteration",
                // "repetitions": 1,
                // "repetition_index": 0,
                // "threads": 1,
                // "iterations": 27818,
                // "real_time": 2.4927432705488449e+04,
                // "cpu_time": 2.4860354734344666e+04,
                // "time_unit": "ns"

                const double realTime = b[ "real_time" ];

                auto iFind = plots.find( name );
                if( iFind != plots.end() )
                {
                    Plot& plot = iFind->second;
                    plot.m_points.push_back(
                        Plot::Point{ static_cast< int >( plot.m_points.size() ), realTime, date } );
                }
                else
                {
                    Plot plot{ { name }, { Plot::Point{ 0, realTime, date } } };
                    plots.insert( { name, plot } );
                }
            }
        }

        for( const auto& [ _, plot ] : plots )
        {
            // avoid gnuplot warning when only one point
            if( plot.m_points.size() > 1 )
            {
                branch.m_elements.push_back( std::move( plot ) );
            }
        }

        report.m_elements.push_back( std::move( branch ) );
    }

    return report;
}

} // namespace

int main( int argc, const char* argv[] )
{
    std::string strMachineName, strConfigName;

    std::string                dataDirectory, repoDirectory, outputJSONFile, outputHTMLFile;
    std::vector< std::string > benchmarkDataFiles;

    {
        bool bShowHelp = false;

        namespace po = boost::program_options;
        po::options_description options;

        // clang-format off
        options.add_options()
        ( "help",       po::bool_switch( &bShowHelp ),                                    "Show Command Line Help" )

        ( "machine",    po::value< std::string >( &strMachineName ),                      "Machine host name" )
        ( "config",     po::value< std::string >( &strConfigName ),                       "Build configuration name" )

        ( "repo",       po::value< std::string >( &repoDirectory ),                       "Performance data git repository" )
        
        ( "output",     po::value< std::string >( &outputHTMLFile ),                      "Output html file" )

        ( "files",      po::value< std::vector< std::string > >( &benchmarkDataFiles ),   "Google benchmark files" )
        ;
        // clang-format on

        po::positional_options_description p;
        p.add( "files", -1 );

        po::parsed_options parsedOptions
            = po::command_line_parser( argc, argv ).options( options ).positional( p ).run();

        po::variables_map vm;
        po::store( parsedOptions, vm );
        po::notify( vm );

        if( bShowHelp )
        {
            std::cout << options << "\n";
            return 0;
        }
    }

    try
    {
        VERIFY_RTE_MSG( !strMachineName.empty(), "Missing machine name" );
        VERIFY_RTE_MSG( !strConfigName.empty(), "Missing configuration name" );

        const boost::filesystem::path repoDirectoryPath
            = boost::filesystem::edsCannonicalise( boost::filesystem::absolute( repoDirectory ) );
        VERIFY_RTE_MSG( boost::filesystem::exists( repoDirectoryPath ),
                        "Failed to locate performance git repository at: " << repoDirectoryPath.string() );
        VERIFY_RTE_MSG(
            git::isGitRepo( repoDirectoryPath ), "Failed to locate git repository at: " << repoDirectoryPath.string() );

        VERIFY_RTE_MSG( !outputHTMLFile.empty(), "Missing output file name" );
        const boost::filesystem::path outputHTMLFilePath
            = boost::filesystem::edsCannonicalise( boost::filesystem::absolute( outputHTMLFile ) );

        const boost::filesystem::path benchmarkRepoDir = repoDirectoryPath / strMachineName / strConfigName;

        for( const auto& benchmarkFile : benchmarkDataFiles )
        {
            const boost::filesystem::path benchmarkFilePath
                = boost::filesystem::edsCannonicalise( boost::filesystem::absolute( benchmarkFile ) );
            VERIFY_RTE_MSG( boost::filesystem::exists( benchmarkFilePath ),
                            "Failed to locate benchmark result file: " << benchmarkFilePath.string() );

            const boost::filesystem::path benchmarkRepoFilePath = benchmarkRepoDir / benchmarkFilePath.filename();

            boost::filesystem::ensureFoldersExist( benchmarkRepoFilePath );
            boost::filesystem::copyFileIfChanged( benchmarkFilePath, benchmarkRepoFilePath );
        }

        // commit the new benchmark results
        {
            std::ostringstream osMsg;
            osMsg << "Automated commit by bench_summary tool";
            git::commit( repoDirectoryPath, osMsg.str() );
        }

        // generate summary report
        {
            // gather the data
            ResultHistoryMap results;

            for( const auto& benchmarkFile : benchmarkDataFiles )
            {
                const boost::filesystem::path benchmarkFilePath
                    = boost::filesystem::edsCannonicalise( boost::filesystem::absolute( benchmarkFile ) );
                const boost::filesystem::path benchmarkRepoFilePath = benchmarkRepoDir / benchmarkFilePath.filename();

                for( const auto& hash : git::getFileGitHashes( repoDirectoryPath, benchmarkRepoFilePath ) )
                {
                    const auto contents = git::getGitFile( repoDirectoryPath, benchmarkRepoFilePath, hash );
                    auto       data     = nlohmann::json::parse( contents );
                    results[ benchmarkFile ].push_back( std::move( data ) );
                }
            }

            // render the html report
            {
                const auto report = makeSummaryReport( results );
                auto       pFile  = boost::filesystem::createNewFileStream( outputHTMLFilePath );
                report::renderHTML( report, *pFile );
            }
        }
    }
    catch( std::exception& ex )
    {
        std::cerr << "Exception: " << ex.what() << std::endl;
        return -1;
    }

    return 0;
}