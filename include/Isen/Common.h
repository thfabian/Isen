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
#ifndef ISEN_COMMON_H
#define ISEN_COMMON_H

#include <Isen/Config.h>
#include <Isen/Type.h>
#include <boost/format.hpp>
#include <cassert>
#include <exception>
#include <memory>
#include <string>

using Isen::Float;

ISEN_NAMESPACE_BEGIN

namespace internal
{

/// Expand variadic arguments
template <typename... Args>
std::string format_vargs(std::string const& fmt, Args&&... args)
{
    boost::format f(fmt);
    int unroll[]{0, (f % std::forward<Args>(args), 0)...};
    static_cast<void>(unroll);
    return boost::str(f);
}
}

/// Simple exception class, which stores a human-readable error description
class IsenException : public std::runtime_error
{
public:
    /// Variadic template constructor to support printf-style arguments
    template <typename... Args>
    IsenException(const char* fmt, const Args&... args) : std::runtime_error(internal::format_vargs(fmt, args...))
    {
    }
};

/// Print an error message and exit the program with EXIT_FAILURE(1)
extern void ISEN_NORETURN error(std::string program, std::string msg);

/// Convert a time value in milliseconds into a human-readable string
extern std::string timeString(Float time, bool precise = false);

/// Convert a memory amount in bytes into a human-readable string
extern std::string memString(std::size_t size, bool precise = false);

// Forward Declaration
struct NameList;
class Output;
class Solver;
class Parser;

ISEN_NAMESPACE_END

#endif
