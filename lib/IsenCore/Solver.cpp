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
 *  This file is distributed under the MIT Open Source License. See
 *  LICENSE.TXT for details.
 */

#include <Isen/Boundary.h>
#include <Isen/Logger.h>
#include <Isen/Output.h>
#include <Isen/Solver.h>
#include <Isen/Timer.h>

ISEN_NAMESPACE_BEGIN

Solver::Solver(std::shared_ptr<NameList> namelist, Output::ArchiveType archiveType) : namelist_(namelist)
{
    SOLVER_DECLARE_ALL_ALIASES

    Timer t;
    LOG() << "Allocating memory ... " << logger::flush;

    try
    {
        //-------------------------------------------------
        // Define physical fields
        //-------------------------------------------------

        // Topography
        topo_ = VectorXf::Zero(nxb);

        // Horizontal velocity
        zhtold_ = zhtnow_ = MatrixXf::Zero(nxb, nz1);

        // Horizontal velocity
        uold_ = unow_ = unew_ = MatrixXf::Zero(nxb1, nz);

        // Isentropic density
        sold_ = snow_ = snew_ = MatrixXf::Zero(nxb, nz);

        // Montgomery potential
        mtg_ = mtgnew_ = MatrixXf::Zero(nxb, nz);
        mtg0_ = VectorXf::Zero(nz);

        // Exner function
        exn_ = MatrixXf::Zero(nxb, nz1);
        exn0_ = VectorXf::Zero(nz1);

        // Pressure
        prs_ = MatrixXf::Zero(nxb, nz1);
        prs0_ = VectorXf::Zero(nz1);

        // Height-dependent diffusion coefficient
        tau_ = VectorXf::Zero(nz);

        // Upstream profile for theta 
        th0_ = VectorXf::Zero(nz1);

        if(imoist)
        {
            // Precipitation
            prec_ = VectorXf::Zero(nxb);

            // Accumulated precipitation
            tot_prec_ = VectorXf::Zero(nxb);

            // Specific humidity
            qvold_ = qvnow_ = qvnew_ = MatrixXf::Zero(nxb, nz);

            // Specific cloud water content
            qcold_ = qcnow_ = qcnew_ = MatrixXf::Zero(nxb, nz);

            // Specific rain water content
            qrold_ = qrnow_ = qrnew_ = MatrixXf::Zero(nxb, nz);

            if(imicrophys == 2)
            {
                // Rain-droplet number density
                nrold_ = nrnow_ = nrnew_ = MatrixXf::Zero(nxb, nz);

                // Cloud-droplet number density
                ncold_ = ncnow_ = ncnew_ = MatrixXf::Zero(nxb, nz);
            }

            if(idthdt)
            {
                // Latent heating
                dthetadt_ = MatrixXf::Zero(nxb, nz1);
            }
        }

        //-------------------------------------------------
        // Define fields at lateral boundaries
        //-------------------------------------------------
        tbnd1_ = tbnd2_ = 0;

        // Isentropic density
        sbnd1_ = sbnd2_ = VectorXf::Zero(nz);

        // Horizontal velocity
        ubnd1_ = ubnd2_ = VectorXf::Zero(nz);

        if(imoist)
        {
            // Specific humidity
            qvbnd1_ = qvbnd2_ = VectorXf::Zero(nz);

            // Specific cloud water content
            qcbnd1_ = qcbnd2_ = VectorXf::Zero(nz);

            // Specific rain water content
            qrbnd1_ = qrbnd2_ = VectorXf::Zero(nz);

            if(imicrophys == 2)
            {
                // Rain-droplet number density
                nrbnd1_ = nrbnd2_ = VectorXf::Zero(nz);

                // Cloud-droplet number density
                ncbnd1_ = ncbnd2_ = VectorXf::Zero(nz);
            }

            if(idthdt)
            {
                // Latent heating
                dthetadtbnd1_ = dthetadtbnd2_ = VectorXf::Zero(nz1);
            }
        }

        //-------------------------------------------------
        // Define scalar fields
        //-------------------------------------------------
        dtdx_ = dt / dx;
        topofact_ = 1.0;
    }
    catch(std::bad_alloc&)
    {
        LOG() << logger::failed;
        throw IsenException("out of memory");
    }
    LOG_SUCCESS(t);

    // Allocate space for output
    output_ = std::make_shared<Output>(namelist_, archiveType);
}

void Solver::init() noexcept
{
    SOLVER_DECLARE_ALL_ALIASES

    // Make upstream profiles and initial conditions
    //-------------------------------------------------------------
    const Float g2 = g * g;

    Timer t;
    LOG() << "Create initial profile ... " << logger::flush;

    VectorXf z0 = VectorXf::Zero(nz1);
    VectorXf rh0 = VectorXf::Zero(nz1);
    VectorXf qv0 = VectorXf::Zero(nz);

    VectorXf qc0, qr0, nc0, nr0;
    if(imoist)
    {
        qc0 = VectorXf::Zero(nz);
        qr0 = VectorXf::Zero(nz);

        if(imicrophys == 2)
        {
            nc0 = VectorXf::Zero(nz);
            nr0 = VectorXf::Zero(nz);
        }
    }

    // Upstream profile for Brunt-Vaisalla frequency (unstaggered)
    //------------------------------------------------------------
    VectorXf bv0 = bv00 * VectorXf::Ones(nz1).array();

    // Upstream profile of theta (staggered)
    // -----------------------------------------------------------
    th0_ = th00 * VectorXf::Ones(nz1).array() + dth * VectorXf::LinSpaced(nz1, 0, nz1 - 1).array();

    // Upstream profile for Exner function and pressure (staggered)
    //-------------------------------------------------------------
    exn0_[0] = exn00;
    for(int k = 1; k < nz1; ++k)
        exn0_[k]
            = exn0_[k - 1] - (16 * g2 * (th0_[k] - th0_[k - 1]) / (pow2(bv0[k - 1] + bv0[k]) * pow2(th0_[k - 1] + th0_[k])));

    for(int k = 0; k < nz1; ++k)
        prs0_[k] = pref * std::pow(exn0_[k] / cp, cpdr);

    // Upstream profile for geometric height (staggered)
    //-------------------------------------------------------------
    z0[0] = z00;
    for(int k = 1; k < nz1; ++k)
        z0[k] = z0[k - 1] + (8 * g * (th0_[k] - th0_[k - 1]) / (pow2(bv0[k - 1] + bv0[k]) * (th0_[k - 1] + th0_[k])));

    // Upstream profile for Montgomery potential (unstaggered)
    //-------------------------------------------------------------
    mtg0_[0] = g * z0[0] + th00 * exn0_[0] + dth * exn0_[0] / 2.;

    Float mtg0old = mtg0_[0];
    for(int k = 1; k < nz; ++k)
    {
        std::swap(mtg0_[k], mtg0old);
        mtg0_[k] += dth * exn0_[k];
    }

    // Upstream profile for isentropic density (unstaggered)
    //-------------------------------------------------------------
    VectorXf s0 = -1. / g * (prs0_.tail(nz1 - 1) - prs0_.head(nz1 - 1)) / dth;

    // Upstream profile for velocity (unstaggered)
    //-------------------------------------------------------------
    VectorXf u0 = u00 * VectorXf::Ones(nz).array();

    if(ishear)
    {
        // *** Exercise 3.3 Downslope windstorm ***
        // *** use indices k_shl, k_sht, and wind speeds u00_sh, u00

        // *** edit here ...
    }
    else
    {
    }

    // Upstream profile for moisture (unstaggered)
    //-------------------------------------------------------------
    if(imoist)
    {
        // *** Exercise 4.1 Initial Moisture profile ***
        // *** define new indices and create the profile ***
        // *** for rh0; then use function rrmixv1 to compute qv0 ***

        // *** edit here ...


        // *** Exercise 4.1 Initial Moisture profile ***

        // Upstream profile for number densities(unstaggered)
        //---------------------------------------------------------
        if(imicrophys == 2)
        {
        }
    }

    // Initial conditions for isentropic density (sigma), velocity u, and
    // moisture qv
    //-------------------------------------------------------------
    sold_ = s0.transpose().replicate(sold_.rows(), 1);
    snow_ = s0.transpose().replicate(snow_.rows(), 1);
    mtg_ = mtg0_.transpose().replicate(mtg_.rows(), 1);
    mtgnew_ = mtg0_.transpose().replicate(mtgnew_.rows(), 1);
    uold_ = u0.transpose().replicate(uold_.rows(), 1);
    unow_ = u0.transpose().replicate(unow_.rows(), 1);

    if(imoist)
    {
        qvold_ = qv0.transpose().replicate(qvold_.rows(), 1);
        qvnow_ = qv0.transpose().replicate(qvnow_.rows(), 1);
        qcold_ = qc0.transpose().replicate(qcold_.rows(), 1);
        qcnow_ = qc0.transpose().replicate(qcnow_.rows(), 1);
        qrold_ = qr0.transpose().replicate(qrold_.rows(), 1);
        qrnow_ = qr0.transpose().replicate(qrnow_.rows(), 1);

        // Droplet density for 2-moment scheme
        if(imicrophys == 2)
        {
            ncold_ = nc0.transpose().replicate(ncold_.rows(), 1);
            ncnow_ = nc0.transpose().replicate(ncnow_.rows(), 1);
            nrold_ = nr0.transpose().replicate(nrold_.rows(), 1);
            nrnow_ = nr0.transpose().replicate(nrnow_.rows(), 1);
        }
    }

    LOG_SUCCESS(t);

    // Save boundary values for the lateral boundary relaxation
    //-------------------------------------------------------------
    if(irelax)
    {
        LOG() << "Saving lateral boundary values ... " << logger::flush;
        t.start();

        sbnd1_ = snow_.row(0);
        sbnd2_ = snow_.row(snow_.rows() - 1);

        ubnd1_ = unow_.row(0);
        ubnd2_ = unow_.row(unow_.rows() - 1);

        if(imoist)
        {
            qvbnd1_ = qvnow_.row(0);
            qvbnd2_ = qvnow_.row(qvnow_.rows() - 1);

            qcbnd1_ = qcnow_.row(0);
            qcbnd2_ = qcnow_.row(qcnow_.rows() - 1);

            qrbnd1_ = qrnow_.row(0);
            qrbnd2_ = qrnow_.row(qrnow_.rows() - 1);

            if(imicrophys == 2)
            {
                ncbnd1_ = ncnow_.row(0);
                ncbnd2_ = ncnow_.row(ncnow_.rows() - 1);

                nrbnd1_ = nrnow_.row(0);
                nrbnd2_ = nrnow_.row(nrnow_.rows() - 1);
            }

            if(idthdt)
            {
                dthetadtbnd1_ = dthetadt_.row(0);
                dthetadtbnd2_ = dthetadt_.row(dthetadt_.rows() - 1);
            }
        }

        LOG_SUCCESS(t);
    }

    // Calculate geometric height (staggered)
    //-------------------------------------------------------------
    for(int k = 1; k < nz1; ++k)
    {
        zhtnow_.col(k) = zhtnow_.col(k - 1).array()
                         - rdcp / g * 0.5 * (th0_[k - 1] * exn0_[k - 1] + th0_[k] * exn0_[k]) * (prs0_[k] - prs0_[k - 1])
                               / (0.5 * (prs0_[k] + prs0_[k - 1]));
    }

    // Make topography
    //-------------------------------------------------------------
    LOG() << "Creating topography ... " << logger::flush;
    t.start();

    Float x0 = (nxb - 1) / 2.0 + 1;
    VectorXf x = (VectorXf::LinSpaced(nxb, 0, nxb - 1).array() + 1 - x0) * dx;

    VectorXf toponf(nxb);
    for(int i = 0; i < nxb; ++i)
        toponf[i] = topomx * std::exp(-(pow2(x[i] / Float(topowd))));

    for(int i = 1; i < nxb - 1; ++i)
        topo_[i] = toponf[i] + 0.25 * (toponf[i - 1] - 2.0 * toponf[i] + toponf[i + 1]);

    LOG_SUCCESS(t);

    // Switch between boundary relaxation / periodic boundary conditions
    //-------------------------------------------------------------
    if(irelax)
    {
        LOG() << "Relax topography ... " << logger::flush;
        t.start();

        tbnd1_ = topo_[0];
        tbnd2_ = topo_[topo_.size() - 1];
        Boundary::relax(topo_, nx, nb, tbnd1_, tbnd2_);

        LOG_SUCCESS(t);
    }
    else
    {
        LOG() << "Periodic topography ... " << logger::flush;
        t.start();
        Boundary::periodic(topo_, nx, nb);
        LOG_SUCCESS(t);
    }

    // Height-dependent diffusion coefficient
    //-------------------------------------------------------------
    tau_ = diff * VectorXf::Ones(nz).array();

    // *** Exercise 3.1 height-dependent diffusion coefficient ***

    // Output initial fields
    //-------------------------------------------------------------
    if(iiniout)
        output_->makeOutput(this);
}

void Solver::write(std::string filename)
{
    output_->write(filename);
}

ISEN_NAMESPACE_END
