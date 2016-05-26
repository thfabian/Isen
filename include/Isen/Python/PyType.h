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
#ifndef ISEN_PYTHON_TYPE_H
#define ISEN_PYTHON_TYPE_H

#include <Isen/Common.h>
#include <Isen/Python/IsenPython.h>
#include <array>

ISEN_NAMESPACE_BEGIN

namespace internal
{

template <class... Args>
boost::python::object toNumpyArrayImpl(const Float* cdata, const Args&... dim)
{
    static_assert(sizeof...(dim) != 0, "no dimensions provided");

    npy_int nd = sizeof...(dim);
    npy_intp dims[sizeof...(dim)] = {dim...};
    Float* data = const_cast<Float*>(cdata);

    PyObject* pyObj = PyArray_SimpleNewFromData(nd, dims, sizeof(Float) == 4 ? NPY_FLOAT : NPY_DOUBLE, data);

    boost::python::handle<> handle(pyObj);
    boost::python::numeric::array arr(handle);

    // Return a copy (lifetime is handle by Python)
    return arr.copy();
}
}

/// Convert an Eigen::Vector to a numpy array
inline boost::python::object toNumpyArray(const VectorXf& vec)
{
    return internal::toNumpyArrayImpl(vec.data(), vec.size(), 1);
}

/// Convert an Eigen::Vector to a numpy array
inline boost::python::object toNumpyArray(const MatrixXf& mat)
{
    // PyArray_SimpleNewFromData expects C-Style array (Row-Major)
    if(MatrixXf::Options == 0 /* Eigen::ColMajor */)
        return internal::toNumpyArrayImpl(MatrixXf(mat.transpose()).data(), mat.rows(), mat.cols());
    return internal::toNumpyArrayImpl(mat.data(), mat.rows(), mat.cols());
}

ISEN_NAMESPACE_END

#endif
