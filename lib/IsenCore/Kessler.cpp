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

#include <Isen/Common.h>
#include <Isen/Kessler.h>

#include <iostream>

ISEN_NAMESPACE_BEGIN

Kessler::Kessler(std::shared_ptr<NameList> namelist) : namelist_(namelist)
{
}

void Kessler::apply(
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
    const MatrixXf& zhtnow) noexcept
{
    KESSLER_DECLARE_ALL_ALIASES

    // Define constants
    //--------------------------------------------------------
    const double dt_in = 2 * dt;

    const double c1 = 0.001 * autoconv_mult;
    const double c2 = autoconv_th;
    constexpr double c3 = 2.2;
    constexpr double c4 = 0.875;
    
    constexpr double svp2 = 17.67;
    constexpr double svp3 = 29.65;
    constexpr double svpt0 = 273.15;
    
    const double ep2 = r/r_v;
    constexpr double xlv = 2.5 * 1e06;
    constexpr double max_cr_sedimentation = 0.75;
    constexpr double rhowater = 1000.;

    //std::cout << xlv << std::endl;

    //std::exit(0);
}

ISEN_NAMESPACE_END
