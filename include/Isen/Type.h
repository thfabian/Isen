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
#ifndef ISEN_TYPE_H
#define ISEN_TYPE_H

#include <Isen/Config.h>

#ifdef ISEN_COMPILER_CLANG
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-register"
#endif
#include <Eigen/Core>
#ifdef ISEN_COMPILER_CLANG
#pragma clang diagnostic pop
#endif

ISEN_NAMESPACE_BEGIN

/// Eigen3 typedefs (matrices are stored in ColMajor order)
using MatrixXf = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>;
using VectorXf = Eigen::Matrix<double, Eigen::Dynamic, 1>;
using MatrixXi = Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic>;
using VectorXi = Eigen::Matrix<int, Eigen::Dynamic, 1>;
using Vector2f = Eigen::Matrix<double, 2, 1>;
using Vector3f = Eigen::Matrix<double, 3, 1>;
using Matrix2f = Eigen::Matrix<double, 2, 2>;
using Matrix3f = Eigen::Matrix<double, 3, 3>;
using Vector2i = Eigen::Matrix<int, 2, 1>;
using Vector3i = Eigen::Matrix<int, 3, 1>;
using Matrix2i = Eigen::Matrix<int, 2, 2>;
using Matrix3i = Eigen::Matrix<int, 3, 3>;

/// Eigen3 extensions
template <class Derived>
inline Vector2i shape(const Eigen::MatrixBase<Derived>& matrix)
{
    return Vector2i(matrix.rows(), matrix.cols());
}

/// std::pow(...) may or may not be optimized for small ints and therefore O(n). These definitions assure O(1)
#if defined(pow2) || defined(pow3)
#error "redefinition of macro pow2 and pow3"
#endif

#define pow2(x) ((x) * (x))
#define pow3(x) ((x) * (x) * (x))

ISEN_NAMESPACE_END

#endif
