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
#ifndef ISEN_PYTHON_ISEN_PYTHON_H
#define ISEN_PYTHON_ISEN_PYTHON_H

#include <Isen/Common.h>
#include <boost/python.hpp>

#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <numpy/ndarrayobject.h>

ISEN_NAMESPACE_BEGIN

/// Translate an IsenException to Python RuntimeError
void translateIsenException(const IsenException& e);

ISEN_NAMESPACE_END

#endif
