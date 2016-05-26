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
#ifndef ISEN_TIMER_H
#define ISEN_TIMER_H

#include <Isen/Common.h>
#include <chrono>

ISEN_NAMESPACE_BEGIN

/// @brief High resolution timer
///
/// This class is used to time operations using the built-in clock type with the highest precision.
/// @code{.cpp}
///     Timer t;
///     // Some work ...
///     std::cout << "elapsed time: " << t.stop() << " ms." << std::endl;
/// @endcode
class Timer
{
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start_;

public:
    /// Start the timer
    Timer() : start_(std::chrono::high_resolution_clock::now()) {}

    /// Reset the timer to the current time
    inline void start() noexcept { start_ = std::chrono::high_resolution_clock::now(); }

    /// Return the number of milliseconds elapsed since the timer was last reset
    inline double stop() noexcept
    {
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double, std::milli>(end - start_).count();
    }
};

ISEN_NAMESPACE_END

#endif
