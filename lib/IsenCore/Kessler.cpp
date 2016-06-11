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
#include <Isen/Logger.h>
#include <Isen/MeteoUtils.h>
#include <cmath>

ISEN_NAMESPACE_BEGIN

Kessler::Kessler(std::shared_ptr<NameList> namelist) : namelist_(namelist)
{
    KESSLER_DECLARE_ALL_ALIASES

    try
    {
        rho_ = MatrixXf::Zero(nxb, nz);
        qcprod_ = MatrixXf::Zero(nxb, nz);

        qrr_ = MatrixXf::Zero(nxb, nz);
        vt_fact_ = MatrixXf::Zero(nxb, nz);
        vt_ = MatrixXf::Zero(nxb, nz);

        rdzw_ = MatrixXf::Zero(nxb, nz);
        crmax_ = MatrixXf::Zero(nxb, nz);

        ppt_ = VectorXf::Zero(nxb);
        zw_ = MatrixXf::Zero(nxb, nz);
        k_max_value_per_col_ = VectorXf::Zero(nz);

        qrprod_ = MatrixXf::Zero(nxb, nz);
        pressure_= MatrixXf::Zero(nxb, nz);
        gam_= MatrixXf::Zero(nxb, nz);
        es_= MatrixXf::Zero(nxb, nz);
        qvs_= MatrixXf::Zero(nxb, nz);
        diff_= MatrixXf::Zero(nxb, nz);
        produc_= MatrixXf::Zero(nxb, nz);
        ern_ = MatrixXf::Zero(nxb, nz);

        production_ = MatrixXf::Zero(nxb, nz);
    }
    catch(std::bad_alloc&)
    {
        LOG() << logger::failed;
        throw IsenException("out of memory");
    }
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

    const double ep2 = r / r_v;
    constexpr double xlv = 2.5 * 1e06;
    constexpr double max_cr_sedimentation = 0.75;
    constexpr double rhowater = 1000.;

    // Reset rain rate to zero
    for(int i = 0; i < nxb; ++i)
        prec(i) = 0.0;

    // Compute density
    //--------------------------------------------------------
    for(int k = 0; k < nz; ++k)
        for(int i = 0; i < nxb; ++i)
            rho_(i, k) = snow(i, k) * dth / (zhtnow(i, k + 1) - zhtnow(i, k)); //TODO: use rdzw

    const double f5 = svp2 * (svpt0 - svp3) * xlv / cp;

    // Terminal velocity calculation and advection
    //--------------------------------------------------------
    for(int k = 0; k < nz; ++k)
        for(int i = 0; i < nxb; ++i)
            qcprod_(i, k) = qrnow(i, k);

    for(int k = 0; k < nz; ++k)
        for(int i = 0; i < nxb; ++i)
            qrr_(i, k) = std::max(0.0, 0.001 * qrnow(i, k) * rho_(i, k));

    for(int k = 0; k < nz; ++k)
        for(int i = 0; i < nxb; ++i)
            vt_fact_(i, k) = 36.34 * vt_mult * std::sqrt(rho_(i, 0) / rho_(i, k));

    for(int k = 0; k < nz; ++k)
        for(int i = 0; i < nxb; ++i)
            vt_(i, k) = std::pow(qrr_(i, k) , 0.1364) * vt_fact_(i, k);
    
    for(int k = 0; k < nz; ++k)
        for(int i = 0; i < nxb; ++i)
            rdzw_(i, k) = 1.0 / (zhtnow(i, k + 1) - zhtnow(i, k));

    // Determine Courant number
    for(int k = 0; k < nz; ++k)
        for(int i = 0; i < nxb; ++i)
            crmax_(i, k) = std::max(0.5 * dt_in * vt_(i, k) * rdzw_(i, k), 0.0);

    // Determine maximum nfall for all grid points
    double nfalld = -1.0;
    for(int k = 0; k < nz; ++k)
        for(int i = 0; i < nxb; ++i)
            nfalld = std::max(nfalld, std::max(1.0, std::ceil(0.5 + crmax_(i, k) / max_cr_sedimentation)));

    int nfall = static_cast<int>(nfalld);
    assert(nfall > 0);

    // Splitting so Courant number for sedimentation is stable
    double dtfall = dt_in / nfall;
    double time_sediment = dt_in;

    if(sediment_on)
    {
        // Terminal velocity calculation and advection (split loop for stability)
        while(nfall > 0)
        {
            time_sediment = time_sediment - dtfall;

            for(int i = 0; i < nxb; ++i)
                ppt_(i) = rho_(i, 0) * qcprod_(i, 0) * vt_(i, 0) * dtfall / rhowater;

            // Precipitation (mm/h)
            for(int i = 0; i < nxb; ++i)
                prec(i) = ppt_(i) * 1000 / dtfall * 3600;

            // Accumulated precipitation (mm)
            for(int i = 0; i < nxb; ++i)
                tot_prec(i) = tot_prec(i) + ppt_(i) * 1000;

            // Time split loop, fallout with flux upstream
            for(int k = 0; k < nz; ++k)
                for(int i = 0; i < nxb; ++i)
                    zw_(i, k) = qcprod_(i, k) * vt_(i, k) * rho_(i, k);

            for(int k = 0; k < nz; ++k)
            {
                // Find max element per col
                double max_element = zw_(0, k);
                for(int i = 1; i < nxb; ++i)
                    max_element = std::max(max_element, zw_(i, k));
                k_max_value_per_col_(k) = max_element;
            }

            // Find largest index which is non-zero
            int k_max = 0;
            for(int k = 1; k < nz; ++k)
                k_max = k_max_value_per_col_(k) != 0.0 ? k : k_max;

            if(k_max == (nz - 1))
            {
                for(int k = 0; k < k_max; ++k)
                    for(int i = 0; i < nxb; ++i)
                        qcprod_(i, k) = qcprod_(i, k) - dtfall * (rdzw_(i, k) / rho_(i, k)) * (zw_(i, k) - zw_(i, k + 1));

                for(int i = 0; i < nxb; ++i)
                    qcprod_(i, nz - 1) = qcprod_(i, nz - 1) - dtfall * rdzw_(i, nz - 1) * zw_(i, nz - 1) / (rho_(i, nz - 1) * rho_(i, nz - 1));
            }
            else
            {
                for(int k = 0; k <= k_max; ++k)
                    for(int i = 0; i < nxb; ++i)
                        qcprod_(i, k) = qcprod_(i, k) - dtfall * (rdzw_(i, k) / rho_(i, k)) * (zw_(i, k) - zw_(i, k + 1));
            }

            // Compute new sedimentation velocity and check/recompute new sedimentation timestep 
            // if this isnt the last split step
            if(nfall > 1)
            {
                nfall = nfall - 1;

                for(int k = 0; k < nz; ++k)
                    for(int i = 0; i < nxb; ++i)
                        qrr_(i, k) = std::max(0.0, 0.001 * qcprod_(i, k) * rho_(i, k));

                for(int k = 0; k < nz; ++k)
                    for(int i = 0; i < nxb; ++i)
                        vt_(i, k) = std::pow(qrr_(i, k), 0.1364) * vt_fact_(i, k);

                for(int k = 0; k < nz; ++k)
                    for(int i = 0; i < nxb; ++i)
                        crmax_(i, k) = std::max(time_sediment * vt_(i, k) * rdzw_(i, k), 0.0);

                double nfalld_new = -1.0;
                for(int k = 0; k < nz; ++k)
                    for(int i = 0; i < nxb; ++i)
                        nfalld_new = std::max(nfalld_new, std::max(1.0, std::ceil(0.5 + crmax_(i, k) / max_cr_sedimentation)));
                int nfall_new = static_cast< int >(nfalld_new);

                if(nfall_new != nfall)
                {
                    nfall = nfall_new;
                    dtfall = time_sediment / nfall;
                }
            }
            else
            {
                nfall = 0;
            }
        }
    }
    else // sediment_on
    {
        for(int k = 0; k < nz; ++k)
            for(int i = 0; i < nxb; ++i)
                qcprod_(i, k) = 0.0;
    }

    // Production/deletion of qc and qr
    //--------------------------------------------------------
    for(int k = 0; k < nz; ++k)
        for(int i = 0; i < nxb; ++i)
        {
            double factorn = 1.0 / (1.0 + c3 * dt_in * std::pow(std::max(0.0, qrnow(i, k)), c4));
            qrprod_(i, k) = qcnow(i, k) * (1.0 - factorn) + c1 * dt_in * factorn * std::max(0.0, qcnow(i, k) - c2);
        }

     // Set limit
    for(int k = 0; k < nz; ++k)
        for(int i = 0; i < nxb; ++i)
            qcnew(i, k) = std::max(qcnow(i, k) - qrprod_(i, k), 0.0);

    for(int k = 0; k < nz; ++k)
        for(int i = 0; i < nxb; ++i)
            qrnew(i, k) = std::max(qcprod_(i, k) + qrprod_(i, k), 0.0);
    
    // Atmospheric conditions
    //--------------------------------------------------------
    for(int k = 0; k < nz; ++k)
        for(int i = 0; i < nxb; ++i)
            temp(i, k) = 0.5 * ((exn(i, k + 1) / cp) * th0(k + 1) + (exn(i, k) / cp) * th0(k));

    for(int k = 0; k < nz; ++k)
        for(int i = 0; i < nxb; ++i)
            pressure_(i, k) = 0.5 * (prs(i, k) + prs(i, k + 1));

    for(int k = 0; k < nz; ++k)
        for(int i = 0; i < nxb; ++i)
            gam_(i, k) = 2.5 * 1e06 / (1004 * 0.5 * (exn(i, k) + exn(i, k + 1)) / cp);

    for(int k = 0; k < nz; ++k)
        for(int i = 0; i < nxb; ++i)
            es_(i, k) = MeteoUtils::eswat1(temp(i, k)) * 100;

    for(int k = 0; k < nz; ++k)
        for(int i = 0; i < nxb; ++i)
            qvs_(i, k) = ep2 * es_(i, k) / (pressure_(i, k) - es_(i, k));

    // Calculate saturation deficit
    for(int k = 0; k < nz; ++k)
        for(int i = 0; i < nxb; ++i)
        {
            double diff_delta = qvs_(i, k) - qvnow(i, k);
            diff_(i, k) = diff_delta < 0.0 ? 0.0 : diff_delta;
        }

    // Saturation adjustment: condensation/evaporation 
    for(int k = 0; k < nz; ++k)
        for(int i = 0; i < nxb; ++i)
            produc_(i, k) = (qvnow(i, k) - qvs_(i, k)) / 
                            (1.0 + pressure_(i, k) / (pressure_(i, k) - es_(i, k)) * qvs_(i, k) * f5 
                             / ((temp(i, k) - svp3) * (temp(i, k) - svp3)));

    // Evaporation of rain
    //--------------------------------------------------------
    if(iern)
    {
        for(int k = 0; k < nz; ++k)
            for(int i = 0; i < nxb; ++i)
                ern_(i, k) = std::min(dt_in * (((1.6 + 124.9 * std::pow(0.001 * rho_(i, k) * qrnew(i, k), 0.2046)) *
                                                (std::pow(0.001 * rho_(i, k) * qrnew(i, k), 0.525))) /
                                               (2.55 * 1e08 / (pressure_(i, k) * qvs_(i, k) + 5.4 * 1e05)))
                                            * (diff_(i, k) / (0.001 * rho_(i, k) * qvs_(i, k))),
                                      std::max(-produc_(i, k) - qcnew(i, k), 0.0));

        // Limit evaporation of rain to current rain amount
        for(int k = 0; k < nz; ++k)
            for(int i = 0; i < nxb; ++i)
                ern_(i, k) = std::min(ern_(i, k) , qrnew(i, k));
    }
    else
    {
        for(int k = 0; k < nz; ++k)
            for(int i = 0; i < nxb; ++i)
                ern_(i, k) = 0.0;
    }

    // Update all variables
    //--------------------------------------------------------
    for(int k = 0; k < nz; ++k)
        for(int i = 0; i < nxb; ++i)
            production_(i, k) = std::max(produc_(i, k), -qcnew(i, k));

    for(int k = 0; k < nz; ++k)
        for(int i = 0; i < nxb; ++i)
            temp(i, k) = gam_(i, k) * (production_(i, k) - ern_(i, k));

    for(int k = 0; k < nz; ++k)
        for(int i = 0; i < nxb; ++i)
            qvnew(i, k) = std::max(qvnow(i, k) - production_(i, k) + ern_(i, k), 0.0);

    for(int k = 0; k < nz; ++k)
        for(int i = 0; i < nxb; ++i)
            qcnew(i, k) = qcnew(i, k) + production_(i, k);
    
    for(int k = 0; k < nz; ++k)
        for(int i = 0; i < nxb; ++i)
            qrnew(i, k) = qrnew(i, k) - ern_(i , k);
}

ISEN_NAMESPACE_END
