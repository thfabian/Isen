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
#ifndef ISEN_PYTHON_LOGGER_H
#define ISEN_PYTHON_LOGGER_H

#include <Isen/Common.h>

ISEN_NAMESPACE_BEGIN

/// @brief Expose the Logger interface to Python
class PyLogger
{
public:
    /// Enable the Logger
    void enable() const noexcept;

    /// Disable the Logger
    void disable() const noexcept;
};

ISEN_NAMESPACE_END

#endif
