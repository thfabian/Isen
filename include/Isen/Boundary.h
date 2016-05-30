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
#ifndef ISEN_BOUNDARY_H
#define ISEN_BOUNDARY_H

#include <Isen/Common.h>
#include <array>

ISEN_NAMESPACE_BEGIN

/// @brief Handle boundary conditions
struct Boundary
{
    /// @brief This subroutine makes the array phi periodic.
    ///
    /// At the left and right border the number of 'nb' points is overwritten. The periodicity of this operation is
    /// 'nx'.
    template <class Derived>
    static void periodic(Eigen::MatrixBase<Derived>& phi, int nx, int nb) noexcept
    {
        assert(phi.rows() == (nx + 2 * nb));
        phi.block(0, 0, nb, phi.cols()) = phi.block(nx, 0, nb, phi.cols());
        phi.block(nx + nb, 0, nb, phi.cols()) = phi.block(nb, 0, nb, phi.cols());
    }

    /// Relaxation of boundary conditions.
    template <class Derived>
    static void relax(Eigen::MatrixBase<Derived>& phi, int nx, int nb, double phi1, double phi2) noexcept
    {
        //TODO: phi1 and phi2 should be arrays?

        assert(phi.rows() == (nx + 2 * nb));

        // Relaxation is done over nr grid points
        constexpr int nr = 8;
        const int n = 2 * nb + nx;

        // Initialize relaxation array
        constexpr std::array<double, nr> rel{{1.0, 0.99, 0.95, 0.8, 0.5, 0.2, 0.05, 0.01}};

        for(int i = 0; i < nr; ++i)
        {
            for(int j = 0; j < phi.cols(); ++j)
            {
                phi(i, j) = phi1 * rel[i] + phi(i, j) * (1 - rel[i]);
                phi(n - 1 - i, j) = phi2 * rel[i] + phi(n - 1 - i, j) * (1 - rel[i]);
            }
        }
    }
};

ISEN_NAMESPACE_END

#endif
