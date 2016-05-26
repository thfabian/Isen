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
#ifndef ISEN_LOGGER_H
#define ISEN_LOGGER_H

#include <Isen/Common.h>
#include <Isen/Timer.h>

ISEN_NAMESPACE_BEGIN

enum class log
{
    nop = 0, ///< Do nothing
    ok,      ///< Print [OK], in white, and @c endl the stream
    failed,  ///< Print [FAILED], in red, and @c endl the stream
    flush,   ///< Flush the stream by writing current content to std::cout
    endl,    ///< Flush/reset the stream and jump to new line
    enable,  ///< Reset and reenable  logger output
    disable  ///< Disable logger output
};

/// @brief Log to console
///
/// The logger class can be used to indicate the current state of the simulation and is usually used in conjunction with
/// Timer. The logger prints colored output by default (See Terminal::Color).
///
/// @code{.cpp}
///     Timer t; // Start timer
///     LOG() << "Doing something intresting ... " << log::flush;
///     // ...
///     LOG_SUCCESS(t);
/// @endcode
///
/// Its crucial to pass log::flush in the end to flush the internal buffer to screen. The LOG_SUCCESS macro will take
/// care of resetting and evaluating the Timer. The above example will produce something like:
/// @code{.cpp}
/// Doing something intresting ...                    20.0 ms [ OK ]
/// @endcode
///
/// To indicate a failed operation pass log::failed to the LOG() stream.
/// @code{.cpp}
///     Timer t;
///     LOG() << "Doing something intresting ... " << log::flush;
///     // ...
///     LOG() << log::failed;
/// @endcode
/// @code{.cpp}
/// Doing something intresting ...                        [ FAILED ]
/// @endcode
///
/// Note: passing log::disable to the stream will turn of any logging.
class Logger
{
public:
    Logger() : curlen_(0), disableLogger_(false) {}

    /// Push an arbitrary message to the logging stack
    template <class T>
    Logger& operator<<(const T& msg) noexcept
    {
        logStack_ << msg;
        return (*this);
    }

    /// Manipulate the logging stack
    Logger& operator<<(log loggerEnum) noexcept;

    /// Reset the logging stack
    void clear() noexcept
    {
        logStack_.str("");
        logStack_.clear();
    }

    /// Clear the logging stack (jump to new line)
    void reset() noexcept
    {
        clear();
        curlen_ = 0;
        timeStr_.clear();
    }

    /// @brief Set the time string.
    ///
    /// The time string is going to be printed right before [OK] when piping log::ok to the logger
    void setTimeStr(const std::string& timeStr) noexcept { timeStr_ = timeStr; }

    /// Check if logger is disabled
    bool isDisabled() const { return disableLogger_; }

private:
    std::stringstream logStack_;
    std::string::size_type curlen_;
    std::string timeStr_;

    bool disableLogger_;
};

extern Logger* loggerInstance;

ISEN_NAMESPACE_END

#define LOG() (*Isen::loggerInstance)
#define LOG_SUCCESS(timer)                                                                                             \
    {                                                                                                                  \
        Isen::loggerInstance->setTimeStr(Isen::timeString((timer).stop()));                                            \
        (*Isen::loggerInstance) << Isen::log::ok;                                                                      \
    }

#endif
