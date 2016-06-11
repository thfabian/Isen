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
#ifndef ISEN_KESSLER_H
#define ISEN_KESSLER_H

#include <Isen/Common.h>
#include <Isen/NameList.h>

ISEN_NAMESPACE_BEGIN

/// @brief Kessler Parametrization
///
/// Kessler (1969) Microphysics Scheme
class Kessler
{
public:
    /// Initialize temporaries
    Kessler(std::shared_ptr<NameList> namelist);

    /// Apply the Kessler microphysic scheme
    void apply(
        // Output
        MatrixXf& temp,
        MatrixXf& qvnew,
        MatrixXf& qcnew,
        MatrixXf& qrnew,
        VectorXf& tot_prec,
        VectorXf& prec,

        // Input
        const VectorXf& th0,
        const MatrixXf& prs,
        const MatrixXf& snow,
        const MatrixXf& qvnow,
        const MatrixXf& qcnow,
        const MatrixXf& qrnow,
        const MatrixXf& exn,
        const MatrixXf& zhtnow) noexcept;

private:
    std::shared_ptr<NameList> namelist_;

    // Internal variables
    MatrixXf rho_;
    MatrixXf qcprod_;

    MatrixXf qrr_;
    MatrixXf vt_fact_;
    MatrixXf vt_;

    MatrixXf rdzw_;
    MatrixXf crmax_;

    VectorXf ppt_;
    MatrixXf zw_;
    VectorXf k_max_value_per_col_;

    MatrixXf qrprod_;
    MatrixXf pressure_;
    MatrixXf gam_;
    MatrixXf es_;
    MatrixXf qvs_;
    MatrixXf diff_;
    MatrixXf produc_;
    MatrixXf ern_;

    MatrixXf production_;
};

/// This is a convenience macro to declare local aliases of the NameList class inside any Kessler method
#define KESSLER_DECLARE_ALL_ALIASES ISEN_NAMELIST_DECLARE_ALIAS(namelist_)

ISEN_NAMESPACE_END

#endif
