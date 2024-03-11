
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

#include "common/file.hpp"
#include "common/git.hpp"
#include "common/string.hpp"
#include "common/assert_verify.hpp"

#include <boost/program_options.hpp>
#include <boost/filesystem/operations.hpp>

#include <iostream>
#include <sstream>

int main( int argc, const char* argv[] )
{
    std::string                dataDirectory, repoDirectory, outputJSONFile, outputHTMLFile;
    std::vector< std::string > benchmarkDataFiles;

    {
        bool bShowHelp = false;

        namespace po = boost::program_options;
        po::options_description options;

        // clang-format off
        options.add_options()
        ( "help",       po::bool_switch( &bShowHelp ),                                    "Show Command Line Help" )
        
        ( "data",       po::value< std::string >( &dataDirectory ),                       "Data Directory" )
        ( "repo",       po::value< std::string >( &repoDirectory ),                       "Get Repository Directory" )
        ( "out_json",   po::value< std::string >( &outputJSONFile ),                      "Output data file" )
        ( "out_html",   po::value< std::string >( &outputHTMLFile ),                      "Output html file" )
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
        const boost::filesystem::path dataDirectoryPath
            = boost::filesystem::edsCannonicalise( boost::filesystem::absolute( dataDirectory ) );
        // VERIFY_RTE_MSG( boost::filesystem::exists( dataDirectoryPath ),
        //                 "Failed to locate data directory at: " << dataDirectoryPath.string() );

        const boost::filesystem::path outputJSONFilePath
            = boost::filesystem::edsCannonicalise( boost::filesystem::absolute( outputJSONFile ) );

        const boost::filesystem::path outputHTMLFilePath
            = boost::filesystem::edsCannonicalise( boost::filesystem::absolute( outputHTMLFile ) );

        const boost::filesystem::path repoDirectoryPath
            = boost::filesystem::edsCannonicalise( boost::filesystem::absolute( repoDirectory ) );
        VERIFY_RTE_MSG( boost::filesystem::exists( repoDirectoryPath ),
                        "Failed to locate git repository at: " << repoDirectoryPath.string() );
        VERIFY_RTE_MSG(
            git::isGitRepo( repoDirectoryPath ), "Failed to locate git repository at: " << repoDirectoryPath.string() );

        // const std::vector< boost::filesystem::path > inputSourceFiles
        //     = mega::utilities::pathListToFiles( componentSrcDir, objectSourceFileNames );

        int counter = 0;
        for( const auto& benchmarkFile : benchmarkDataFiles )
        {
            const boost::filesystem::path benchmarkFilePath
                = boost::filesystem::edsCannonicalise( boost::filesystem::absolute( benchmarkFile ) );

            {
                std::cout << "got: " << benchmarkFilePath.string() << "\n";

                VERIFY_RTE_MSG(
                    boost::filesystem::copyFileIfChanged( benchmarkFilePath, outputJSONFilePath ),
                    "Failed to copy file: " << benchmarkFilePath.string() << " to: " << outputJSONFilePath.string() );
            }

            {
                std::ostringstream os;
                os << "bench_" << common::date() + ".json";
                const boost::filesystem::path dataFileCopyPath = dataDirectoryPath / os.str();
                boost::filesystem::ensureFoldersExist( dataFileCopyPath );

                VERIFY_RTE_MSG(
                    boost::filesystem::copyFileIfChanged( benchmarkFilePath, dataFileCopyPath ),
                    "Failed to copy file: " << benchmarkFilePath.string() << " to: " << dataFileCopyPath.string() );
            }

            /*for( const auto& hash : git::getFileGitHashes( repoDirectoryPath, benchmarkFilePath ) )
            {
                std::cout << hash << "\n";

                const auto contents = git::getGitFile( repoDirectoryPath, benchmarkFilePath, hash );

                {
                    std::ostringstream os;
                    os << outputFile << "_" << counter++;

                    auto pFile = boost::filesystem::createNewFileStream( os.str() );
                    *pFile << contents;
                }
            }*/
        }
        std::cout << std::endl;
    }
    catch( std::exception& ex )
    {
        std::cerr << "Exception: " << ex.what() << std::endl;
        return -1;
    }

    return 0;
}