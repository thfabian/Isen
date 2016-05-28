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

#include <Isen/Python/PyLogger.h>
#include <Isen/Logger.h>

ISEN_NAMESPACE_BEGIN

void PyLogger::enable() const noexcept
{
    LOG() << log::enable;
}

void PyLogger::disable() const noexcept
{
    LOG() << log::disable;
}

ISEN_NAMESPACE_END
