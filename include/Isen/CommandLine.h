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

#pragma once
#ifndef ISEN_COMMANDLINE_H
#define ISEN_COMMANDLINE_H

#include <Isen/Common.h>
#include <boost/program_options.hpp>
#include <string>

ISEN_NAMESPACE_BEGIN

/// @brief Handle command-line arguments
///
/// This is just a simple wrapper around Boost.Program_options
class CommandLine
{
public:
    /// Setup the commands to be looked for
    CommandLine();

    /// Setup the commands to be looked for and call CommandLine::parse
    CommandLine(int argc, const char* const argv[]);

    /// Invoke the option parser (exit program if an error occurs)
    void parse(int argc, const char* const argv[]);

    /// Print version information and exit the program with EXIT_SUCCESS(0)
    ISEN_NORETURN void printVersion() noexcept;

    /// Print help information and exit the program with EXIT_SUCCESS(0)
    ISEN_NORETURN void printHelp() const noexcept;

    /// Check whether the command-line argument @c arg has been passed
    bool has(const std::string& arg) const { return (variableMap_.count(arg) == 1); }

    /// Get command-line argument @c arg as type @c T
    template <typename T>
    T as(const std::string& arg) const
    {
        return variableMap_[arg].as<T>();
    }

private:
    boost::program_options::variables_map variableMap_;
    boost::program_options::options_description desc_;
    boost::program_options::positional_options_description posDesc_;
};

ISEN_NAMESPACE_END

#endif
