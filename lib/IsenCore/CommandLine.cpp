/**
 *                       _________ _______   __
 *                      /  _/ ___// ____/ | / /
 *                      / / \__ \/ __/ /  |/ /
 *                    _/ / ___/ / /___/ /|  /
 *                   /___//____/_____/_/ |_/
 *
 *  Isentropic model - ETH Zurich
 *  Copyright (C) 2016  Fabian Thuering (thfabian@student.ethz.ch)
 *
 *  This file is distributed under the MIT Open Source License. See LICENSE.TXT for details.
 */

#include <Isen/CommandLine.h>
#include <Isen/Terminal.h>
#include <algorithm>
#include <boost/filesystem.hpp>
#include <boost/version.hpp>
#include <cstdlib>
#include <initializer_list>
#include <iostream>
#include <string>
#include <vector>

#ifdef ISEN_PYTHON
#include <boost/preprocessor/stringize.hpp>
#include <boost/python.hpp>
#define ISEN_PYTHON_VERSION_STRING                                                                                     \
    BOOST_PP_STRINGIZE(PY_MAJOR_VERSION)                                                                               \
    "." BOOST_PP_STRINGIZE(PY_MINOR_VERSION) "." BOOST_PP_STRINGIZE(PY_MICRO_VERSION)
#endif

namespace po = boost::program_options;
namespace bf = boost::filesystem;

ISEN_NAMESPACE_BEGIN

CommandLine::CommandLine() : desc_("Options", Terminal::getWidth())
{
    desc_.add_options()
        // --help
        ("help", "Display this information.")
        // --version
        ("version", "Display version information.")
        // --file, -f
        ("file,f", po::value<std::vector<std::string>>(),
         "Specify the input file(s) which will be parsed. Usually a valid "
         "MATLAB (.m) or Python (.py) file containing the input variables. "
         "Multiple files will be parsed/executed one after another.")
        // --no-output
        ("no-output", "Don't write simulation to output file.")
        // --print-namelist
        ("print-namelist,p", "Print the parsed NameList.")
        // --quiet
        ("quiet", "Be quit (no output to terminal).")
        // --archive, -a
        ("archive,a", po::value<std::string>(), "Set the archive type of the output file(s). Allowed values are:"
                                                "\n text - A portable plain text archive"
                                                "\n xml  - A portable XML archive"
                                                "\n bin  - A non-portable native binary archive"
                                                "\nBy default a plain text archive is being used.")
        // --parsing-style
        ("parsing-style", po::value<std::string>(),
         "Set the parsing style for the input file(s). Allowed values are:"
         "\n matlab - Use Matlab syntax"
         "\n python - Use Python syntex"
         "\nBy default the parsing style is deduced from file the extension.")
        // --no-color
        ("no-color", "Don't use colored terminal output (useful when piping the output to a file).");

    // Associate positional arguments to options
    posDesc_.add("file", -1);
}

CommandLine::CommandLine(int argc, const char* const argv[]) : CommandLine()
{
    parse(argc, argv);
}

/// Check if the value of arg is in the list
template <class T, class VarMap>
inline void validate(const std::string& arg, const VarMap& varMap, std::initializer_list<T> list)
{
    if(varMap.count(arg) && std::count(list.begin(), list.end(), varMap[arg].template as<T>()) == 0)
        throw po::validation_error(po::validation_error::invalid_option_value, arg, varMap[arg].template as<T>());
}

void CommandLine::parse(int argc, const char* const argv[])
{
    try
    {
        po::store(po::command_line_parser(argc, argv).options(desc_).positional(posDesc_).run(), variableMap_);
        po::notify(variableMap_);


        // Validation
        validate<std::string>("archive", variableMap_, {"text", "xml", "bin"});
        validate<std::string>("parsing-style", variableMap_, {"matlab", "python"});
    }
    catch(const std::exception& e)
    {
        error(argv[0], (boost::format("%s, for help type '%s --help'") % e.what()
                        % bf::path(argv[0]).filename().string()).str());
    }
}

ISEN_NORETURN void CommandLine::printHelp() const noexcept
{
    std::cout << "Isen - 2D Isentropic Model\n\n";
    std::cout << "Usage: isen [options] files...\n\n";
    std::cout << desc_ << std::endl;
    std::exit(EXIT_SUCCESS);
}

void CommandLine::printVersion() noexcept
{
    std::cout << "Isen (" << ISEN_VERSION_STRING << ") - 2D Isentropic Model\n";

    // Tag compiler
#if defined(ISEN_COMPILER_MSVC)
    std::cout << " - Compiler: Microsoft(R) Visual C/C++ " << _MSC_VER << "\n";
#elif defined(ISEN_COMPILER_INTEL)
    std::cout << " - Compiler: " << __VERSION__ << "\n";
#elif defined(ISEN_COMPILER_CLANG)
    std::cout << " - Compiler: LLVM Clang " << __clang_version__ << "\n";
#elif defined(ISEN_COMPILER_GNU)
    std::cout << " - Compiler: GNU GCC/G++ " << __VERSION__ << "\n";
#endif

    // Boost version
    std::cout << " - Boost version: " << BOOST_LIB_VERSION << "\n";

    // Python version
#ifdef ISEN_PYTHON
    std::cout << " - Python version: " << ISEN_PYTHON_VERSION_STRING << "\n";
#endif

    std::exit(EXIT_SUCCESS);
}

ISEN_NAMESPACE_END
