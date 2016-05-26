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
#ifndef ISEN_PROGRESSBAR_H
#define ISEN_PROGRESSBAR_H

#include <Isen/Common.h>
#include <Isen/Timer.h>

ISEN_NAMESPACE_BEGIN

/// @brief Report current progress
class Progressbar
{
public:
    /// Disable the progressbar (default: false)
    static bool disableProgressbar;

    /// Initialize progressbar with max timestep
    Progressbar(int maxStep);

    /// @brief Advance the progressbar by one step and draw the bar
    ///
    /// The bar is only redrawn if the bar has changed or the time between two consecutive invocations of
    /// Progressbar::advance is larger than Progressbar::IntervalMs.
    void advance();

    /// Pause drawing of the progressbar by resetting the cursor position
    void pause() const;

    /// Print a bar of characters @c c matching the terminal width
    static void printBar(char c);

    /// Only print the bar in Progressbar::advance if IntervalMs have passed
    static const float IntervalMs;

private:
    int curStep_;       ///< Current progress
    int maxStep_;       ///< Maximum steps to reach 100%
    int curBarWidth_;   ///< Current width of the bar
    int maxBarWidth_;   ///< Bar width
    int terminalWidth_; ///< Terminal width

    std::string strSpace_; ///< Spaces printed after bar
    std::string strBar_;   ///< Bar string
    Timer timer_;          ///< Time invocation intervals of advance
};

ISEN_NAMESPACE_END

#endif
