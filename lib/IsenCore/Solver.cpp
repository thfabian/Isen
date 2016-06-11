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

#define _USE_MATH_DEFINES
#include <cmath>

#include <Isen/Boundary.h>
#include <Isen/Logger.h>
#include <Isen/Output.h>
#include <Isen/MeteoUtils.h>
#include <Isen/Progressbar.h>
#include <Isen/Solver.h>
#include <Isen/Timer.h>

#ifdef ISEN_PYTHON
#include <boost/python.hpp>
#endif

ISEN_NAMESPACE_BEGIN

Solver::Solver(const std::shared_ptr<NameList>& namelist, Output::ArchiveType archiveType)
{
    // Copy NameList
    namelist_ = std::make_shared<NameList>(*namelist);
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

            // Temperature
            temp_ = MatrixXf::Zero(nxb, nz1);

            // Parametrization
            if(imicrophys == 1)
                kessler_ = std::make_shared<Kessler>(namelist_);

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
        tbnd1_ = tbnd2_ = VectorXf::Zero(1);

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
    const double g2 = g * g;

    Timer t;
    LOG() << "Create initial profile ... " << logger::flush;

    VectorXf z0 = VectorXf::Zero(nz1);

    VectorXf rh0, qv0, qc0, qr0, nc0, nr0;
    if(imoist)
    {
        rh0 = VectorXf::Zero(nz);

        qv0 = VectorXf::Zero(nz);
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
        exn0_[k] = exn0_[k - 1]
                   - (16 * g2 * (th0_[k] - th0_[k - 1]) / (pow2(bv0[k - 1] + bv0[k]) * pow2(th0_[k - 1] + th0_[k])));

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

    double mtg0old = mtg0_[0];
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
        for(int k = 0; k < k_shl; ++k)
            u0(k) = u00_sh;

        for(int k = k_shl; k < k_sht; ++k)
            u0(k) = u00_sh - (u00_sh - u00) * (k - k_shl) / (k_sht - k_shl);

        for(int k = k_sht; k < nz; ++k)
            u0(k) = u00;
    }

    // Upstream profile for moisture (unstaggered)
    //-------------------------------------------------------------
    if(imoist)
    {
        double rhmax = 0.98;
        const int kc = 12;
        const int kw = 10;

        for(int k = kc - kw; k < (kc + kw - 1); ++k)
        {
            double cos_k = std::cos((std::abs((k + 1) - kc) / double(kw)) * M_PI * 0.5);
            rh0[k] = rhmax * cos_k * cos_k;
        }

        for(int k = 0; k < nz; ++k)
        {
            qv0[k] = MeteoUtils::rrmixv1(0.5 * (prs0_[k] + prs0_[k + 1]) / 100,
                                         0.5 * (th0_[k] / cp * exn0_[k] + th0_[k + 1] / cp * exn0_[k + 1]), rh0[k],
                                         MeteoUtils::ERelative);
        }

        // Upstream profile for number densities(unstaggered)
        //---------------------------------------------------------
        if(imicrophys == 2)
        {
            for(int k = 0; k < nz; ++k)
            {
                nc0[k] = 0;
                nr0[k] = 0;
            }
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
                         - rdcp / g * 0.5 * (th0_[k - 1] * exn0_[k - 1] + th0_[k] * exn0_[k])
                               * (prs0_[k] - prs0_[k - 1]) / (0.5 * (prs0_[k] + prs0_[k - 1]));
    }

    // Make topography
    //-------------------------------------------------------------
    LOG() << "Creating topography ... " << logger::flush;
    t.start();

    double x0 = (nxb - 1) / 2.0 + 1;
    VectorXf x = (VectorXf::LinSpaced(nxb, 0, nxb - 1).array() + 1 - x0) * dx;

    VectorXf toponf(nxb);
    for(int i = 0; i < nxb; ++i)
        toponf[i] = topomx * std::exp(-(pow2(x[i] / double(topowd))));

    for(int i = 1; i < nxb - 1; ++i)
        topo_[i] = toponf[i] + 0.25 * (toponf[i - 1] - 2.0 * toponf[i] + toponf[i + 1]);

    LOG_SUCCESS(t);

    // Switch between boundary relaxation / periodic boundary conditions
    //-------------------------------------------------------------
    if(irelax)
    {
        LOG() << "Relax topography ... " << logger::flush;
        t.start();

        tbnd1_[0] = topo_[0];
        tbnd2_[0] = topo_[topo_.size() - 1];
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
    LOG() << "Height-dependent diffusion coefficient ... " << logger::flush;
    t.start();

    tau_ = diff * VectorXf::Ones(nz).array();

    for(int k = nz - nab; k < nz; ++k)
    {
        double sin_k = std::sin(0.5 * M_PI * ((k + 1) - (nz - nab)) / nab);
        tau_(k) = diff + (diffabs - diff) * (sin_k * sin_k);
    }

    LOG_SUCCESS(t);

    // Set up getter maps
    //-------------------------------------------------------------
    matMap_.insert(std::make_pair<std::string, MatrixXf*>("zhtold", &zhtold_));
    matMap_.insert(std::make_pair<std::string, MatrixXf*>("zhtnow", &zhtnow_));
    matMap_.insert(std::make_pair<std::string, MatrixXf*>("uold", &uold_));
    matMap_.insert(std::make_pair<std::string, MatrixXf*>("unow", &unow_));
    matMap_.insert(std::make_pair<std::string, MatrixXf*>("unew", &unew_));
    matMap_.insert(std::make_pair<std::string, MatrixXf*>("sold", &sold_));
    matMap_.insert(std::make_pair<std::string, MatrixXf*>("snow", &snow_));
    matMap_.insert(std::make_pair<std::string, MatrixXf*>("snew", &snew_));
    matMap_.insert(std::make_pair<std::string, MatrixXf*>("mtg", &mtg_));
    matMap_.insert(std::make_pair<std::string, MatrixXf*>("mtgnew", &mtgnew_));
    matMap_.insert(std::make_pair<std::string, MatrixXf*>("exn", &exn_));
    matMap_.insert(std::make_pair<std::string, MatrixXf*>("prs", &prs_));
    matMap_.insert(std::make_pair<std::string, MatrixXf*>("qvold", &qvold_));
    matMap_.insert(std::make_pair<std::string, MatrixXf*>("qvnow", &qvnow_));
    matMap_.insert(std::make_pair<std::string, MatrixXf*>("qvnew", &qvnew_));
    matMap_.insert(std::make_pair<std::string, MatrixXf*>("qrold", &qrold_));
    matMap_.insert(std::make_pair<std::string, MatrixXf*>("qrnow", &qrnow_));
    matMap_.insert(std::make_pair<std::string, MatrixXf*>("qrnew", &qrnew_));
    matMap_.insert(std::make_pair<std::string, MatrixXf*>("qcold", &qcold_));
    matMap_.insert(std::make_pair<std::string, MatrixXf*>("qcnow", &qcnow_));
    matMap_.insert(std::make_pair<std::string, MatrixXf*>("qcnew", &qcnew_));
    matMap_.insert(std::make_pair<std::string, MatrixXf*>("temp", &temp_));
    matMap_.insert(std::make_pair<std::string, MatrixXf*>("nrold", &nrold_));
    matMap_.insert(std::make_pair<std::string, MatrixXf*>("nrnow", &nrnow_));
    matMap_.insert(std::make_pair<std::string, MatrixXf*>("nrnew", &nrnew_));
    matMap_.insert(std::make_pair<std::string, MatrixXf*>("ncold", &ncold_));
    matMap_.insert(std::make_pair<std::string, MatrixXf*>("ncnow", &ncnow_));
    matMap_.insert(std::make_pair<std::string, MatrixXf*>("ncnew", &ncnew_));
    matMap_.insert(std::make_pair<std::string, MatrixXf*>("dthetadt", &dthetadt_));

    vecMap_.insert(std::make_pair<std::string, VectorXf*>("topo", &topo_));
    vecMap_.insert(std::make_pair<std::string, VectorXf*>("mtg0", &mtg0_));
    vecMap_.insert(std::make_pair<std::string, VectorXf*>("exn0", &exn0_));
    vecMap_.insert(std::make_pair<std::string, VectorXf*>("prs0", &prs0_));
    vecMap_.insert(std::make_pair<std::string, VectorXf*>("tau", &tau_));
    vecMap_.insert(std::make_pair<std::string, VectorXf*>("th0", &th0_));
    vecMap_.insert(std::make_pair<std::string, VectorXf*>("prec", &prec_));
    vecMap_.insert(std::make_pair<std::string, VectorXf*>("tot_prec", &tot_prec_));
    vecMap_.insert(std::make_pair<std::string, VectorXf*>("sbnd1", &sbnd1_));
    vecMap_.insert(std::make_pair<std::string, VectorXf*>("sbnd2", &sbnd2_));
    vecMap_.insert(std::make_pair<std::string, VectorXf*>("ubnd1", &ubnd1_));
    vecMap_.insert(std::make_pair<std::string, VectorXf*>("ubnd2", &ubnd2_));
    vecMap_.insert(std::make_pair<std::string, VectorXf*>("qvbnd1", &qvbnd1_));
    vecMap_.insert(std::make_pair<std::string, VectorXf*>("qvbnd2", &qvbnd2_));
    vecMap_.insert(std::make_pair<std::string, VectorXf*>("qcbnd1", &qcbnd1_));
    vecMap_.insert(std::make_pair<std::string, VectorXf*>("qcbnd2", &qcbnd2_));
    vecMap_.insert(std::make_pair<std::string, VectorXf*>("qrbnd1", &qrbnd1_));
    vecMap_.insert(std::make_pair<std::string, VectorXf*>("qrbnd2", &qrbnd2_));
    vecMap_.insert(std::make_pair<std::string, VectorXf*>("dthetadtbnd1", &dthetadtbnd1_));
    vecMap_.insert(std::make_pair<std::string, VectorXf*>("dthetadtbnd2", &dthetadtbnd2_));
    vecMap_.insert(std::make_pair<std::string, VectorXf*>("nrbnd1", &nrbnd1_));
    vecMap_.insert(std::make_pair<std::string, VectorXf*>("nrbnd2", &nrbnd2_));
    vecMap_.insert(std::make_pair<std::string, VectorXf*>("ncbnd1", &ncbnd1_));
    vecMap_.insert(std::make_pair<std::string, VectorXf*>("ncbnd2", &ncbnd2_));
    vecMap_.insert(std::make_pair<std::string, VectorXf*>("tbnd1", &tbnd1_));
    vecMap_.insert(std::make_pair<std::string, VectorXf*>("tbnd2", &tbnd2_));

    // Output initial fields
    //-------------------------------------------------------------
    if(iiniout)
        output_->makeOutput(this);
}

const MatrixXf& Solver::getMat(std::string name) const
{
    try
    {
        return *matMap_.at(name);
    }
    catch(std::out_of_range&)
    {
        throw IsenException("no matrix named '%s' in Solver", name);
    }
}

const VectorXf& Solver::getVec(std::string name) const
{
    try
    {
        return *vecMap_.at(name);
    }
    catch(std::out_of_range&)
    {
        throw IsenException("no vector named '%s' in Solver", name);
    }
}

Eigen::Map<MatrixXf> Solver::getField(std::string name) const
{
    if(matMap_.find(name) != matMap_.end())
    {
        const auto& mat = matMap_.at(name);
        return Eigen::Map<MatrixXf>(const_cast<double*>(mat->data()), mat->rows(), mat->cols());
    }
    else if(vecMap_.find(name) != vecMap_.end())
    {
        const auto& vec = vecMap_.at(name);
        return Eigen::Map<MatrixXf>(const_cast<double*>(vec->data()), vec->rows(), vec->cols());
    }
    else
        throw IsenException("no field named '%s' in Solver", name);
}

void Solver::run()
{
    SOLVER_DECLARE_ALL_ALIASES

    Timer t;

    Progressbar pbar(nts);
    const bool logIsDisabled = LOG().isDisabled();
    Progressbar::disableProgressbar = logIsDisabled;

    double curTime = 0;

    // Loop over all time steps
    //------------------------------------------------------------
    for(int i = 1; i < (nts + 1); ++i)
    {
        if(!iprtcfl)
            pbar.advance();

        curTime += dt;
        topofact_ = std::min(1., curTime / topotim);

        // Special treatment of first time step
        //--------------------------------------------------------
        dtdx_ = i == 1 ? 0.5 * dt / dx : dt / dx;

        // Prognostic step
        //--------------------------------------------------------

        // Isentropic mass density
        progIsendens();

        // Moisture scalars
        if(imoist)
            progMoisture();

        // Velocity
        progVelocity();

        // Exchange boundaries if periodic
        //--------------------------------------------------------
        if(!irelax)
            applyPeriodicBoundary();

        // Relaxation of prognostic fields
        //--------------------------------------------------------
        if(irelax)
            applyRelaxationBoundary();

        uold_.swap(unow_);
        sold_.swap(snow_);
        qvold_.swap(qvnow_);
        qcold_.swap(qcnow_);
        qrold_.swap(qrnow_);

        unow_.swap(unew_);
        snow_.swap(snew_);
        qvnow_.swap(qvnew_);
        qcnow_.swap(qcnew_);
        qrnow_.swap(qrnew_);

        // Diffusion and gravity wave absorber
        //--------------------------------------------------------
        horizontalDiffusion();

        if(!irelax)
            applyPeriodicBoundary();

        if(imoist)
            clipMoisture();

        unow_.swap(unew_);
        snow_.swap(snew_);
        qvnow_.swap(qvnew_);
        qcnow_.swap(qcnew_);
        qrnow_.swap(qrnew_);

        // Diagnostic step
        //--------------------------------------------------------

        // Pressure
        diagPressure();

        // Montgomorey
        diagMontgomery();

        // Calculation of geometric height (staggered)
        //--------------------------------------------------------
        zhtnow_.swap(zhtold_);
        geometricHeight();

        // Microphysics
        //---------------------------------------------------------
        if(imoist)
        {
            if(imicrophys == 1) // Kessler scheme
            {
                kessler_->apply(
                    // Output
                    temp_, qvnew_, qcnew_, qrnew_, tot_prec_, prec_,

                    // Input
                    th0_, prs_, snow_, qvnow_, qcnow_, qrnow_, exn_, zhtnow_);
            }
            else if(imicrophys == 2) // Two-moment scheme
            {
                //TODO...
            }

            if(imicrophys > 0)
            {
                if(idthdt) // Diabatic flow
                {
                    //TODO...
                }
            }
        }

        qvnow_.swap(qvnew_);
        qcnow_.swap(qcnew_);
        qrnow_.swap(qrnew_);

        // Check maximum CFL condition
        //--------------------------------------------------------
        double umax = computeCFL();
        double cflmax = umax * dtdx_;

        if(iprtcfl)
            std::printf("CFL max: %f U max: %f m/s \n", cflmax, umax);

        if(cflmax > 1)
            warning("isen", (boost::format("CFL condition violated (CFL max %f)") % cflmax).str());
        if(std::isnan(cflmax))
            error("isen", "model encountered NaN values");

        // Output every 'iout'-th time step
        //--------------------------------------------------------
        if((i % iout) == 0)
            output_->makeOutput(this);

#ifdef ISEN_PYTHON
        // Handle Python signals
        //--------------------------------------------------------
        if(PyErr_CheckSignals() == -1)
            throw IsenException("PySolver::run : signal caught");
#endif
    }

    pbar.pause();
    if(!logIsDisabled)
        Progressbar::printBar('=');

    if(logIsDisabled && itime)
        std::printf("Elapsed time: %s\n", timeString(t.stop()).c_str());

    LOG() << "Finished time loop ...";
    LOG_SUCCESS(t);
}

double Solver::computeCFL() const noexcept
{
    SOLVER_DECLARE_ALL_ALIASES

    double umax = -std::numeric_limits<double>::max();
    for(int k = 0; k < nz; ++k)
        for(int i = 0; i < nxb; ++i)
            umax = std::max(umax, std::fabs(unow_(i, k)));
    return umax;
}

void Solver::horizontalDiffusion() noexcept
{
    SOLVER_DECLARE_ALL_ALIASES

    const int nxnb = nx + nb;
    const int nxnb1 = nx + nb + 1;

    for(int k = 0; k < nz; ++k)
    {
        const double tau = tau_(k);
        const bool sel = tau_(k) > 0.0;
        const bool negSel = !sel;

        // Velocity
        for(int i = nb; i < nxnb1; ++i)
        {
            unew_(i, k) = sel * (unow_(i, k) + 0.25 * tau * (unow_(i - 1, k) - 2 * unow_(i, k) + unow_(i + 1, k)))
                          + negSel * unow_(i, k);
        }

        // Isentropic density
        for(int i = nb; i < nxnb; ++i)
        {
            snew_(i, k) = sel * (snow_(i, k) + 0.25 * tau * (snow_(i - 1, k) - 2 * snow_(i, k) + snow_(i + 1, k)))
                          + negSel * snow_(i, k);
        }

        if(imoist && imoist_diff)
        {
            // Water vapor (qv)
            for(int i = nb; i < nxnb; ++i)
            {
                qvnew_(i, k)
                    = sel * (qvnow_(i, k) + 0.25 * tau * (qvnow_(i - 1, k) - 2 * qvnow_(i, k) + qvnow_(i + 1, k)))
                      + negSel * qvnow_(i, k);
            }

            // Specific cloud water content (qc)
            for(int i = nb; i < nxnb; ++i)
            {
                qcnew_(i, k)
                    = sel * (qcnow_(i, k) + 0.25 * tau * (qcnow_(i - 1, k) - 2 * qcnow_(i, k) + qcnow_(i + 1, k)))
                      + negSel * qcnow_(i, k);
            }

            // Specific rain water content (qr)
            for(int i = nb; i < nxnb; ++i)
            {
                qrnew_(i, k)
                    = sel * (qrnow_(i, k) + 0.25 * tau * (qrnow_(i - 1, k) - 2 * qrnow_(i, k) + qrnow_(i + 1, k)))
                      + negSel * qrnow_(i, k);
            }
        }
    }
}

void Solver::geometricHeight() noexcept
{
    SOLVER_DECLARE_ALL_ALIASES

    for(int i = 0; i < nxb; ++i)
        zhtnow_(i, 0) = topo_(i) * topofact_;

    const double rcpg05 = 0.5 * r / cp / g;
    for(int k = 1; k < nz1; ++k)
        for(int i = 0; i < nxb; ++i)
        {
            double th0exn = (th0_(k - 1) * exn_(i, k - 1) + th0_(k) * exn_(i, k));
            double prs = (prs_(i, k) - prs_(i, k - 1)) / (0.5 * (prs_(i, k) + prs_(i, k - 1)));
            zhtnow_(i, k) = zhtnow_(i, k - 1) - rcpg05 * th0exn * prs;
        }
}

void Solver::applyPeriodicBoundary() noexcept
{
    SOLVER_DECLARE_ALL_ALIASES

    assert(!irelax);
    Boundary::periodic(snew_, nx, nb);
    Boundary::periodic(unew_, nx + 1, nb);

    if(imoist)
    {
        Boundary::periodic(qvnew_, nx, nb);
        Boundary::periodic(qcnew_, nx, nb);
        Boundary::periodic(qrnew_, nx, nb);

        if(imicrophys == 2)
        {
            Boundary::periodic(ncnew_, nx, nb);
            Boundary::periodic(nrnew_, nx, nb);
        }
    }
}

void Solver::applyRelaxationBoundary() noexcept
{
    SOLVER_DECLARE_ALL_ALIASES

    assert(irelax);
    Boundary::relax(snew_, nx, nb, sbnd1_, sbnd2_);
    Boundary::relax(unew_, nx1, nb, ubnd1_, ubnd2_);

    if(imoist)
    {
        Boundary::relax(qvnew_, nx, nb, qvbnd1_, qvbnd2_);
        Boundary::relax(qcnew_, nx, nb, qcbnd1_, qcbnd2_);
        Boundary::relax(qrnew_, nx, nb, qrbnd1_, qrbnd2_);

        if(imicrophys == 2)
        {
            Boundary::relax(ncnew_, nx, nb, ncbnd1_, ncbnd2_);
            Boundary::relax(nrnew_, nx, nb, nrbnd1_, nrbnd2_);
        }
    }
}

void Solver::clipMoisture() noexcept
{
    SOLVER_DECLARE_ALL_ALIASES
    const int nxnb = nx + nb;

    auto clip = [&](MatrixXf& mat)
    {
        for(int k = 0; k < nz; ++k)
            for(int i = nb; i < nxnb; ++i)
                if(mat(i, k) < 0.0)
                    mat(i, k) = 0.0;
    };

    clip(qvnew_);
    clip(qcnew_);
    clip(qrnew_);
}

void Solver::diagMontgomery() noexcept
{
    SOLVER_DECLARE_ALL_ALIASES

    const double dth05 = dth * 0.5;
    const double gtopofact_ = g * topofact_;

    // Exner function
    for(int k = 0; k < nz1; ++k)
        for(int i = 0; i < nxb; ++i)
            exn_(i, k) = cp * std::pow(prs_(i, k) / pref, rdcp);

    // Montgomery
    for(int i = 0; i < nxb; ++i)
        mtg_(i, 0) = gtopofact_ * topo_(i) + th0_(0) * exn_(i, 0) + dth05 * exn_(i, 0);

    for(int k = 1; k < nz; ++k)
        for(int i = 0; i < nxb; ++i)
            mtg_(i, k) = mtg_(i, k - 1) + dth * exn_(i, k);
}

void Solver::diagPressure() noexcept
{
    SOLVER_DECLARE_ALL_ALIASES

    const double gdth = g * dth;

    for(int i = 0; i < nxb; ++i)
        prs_(i, nz) = prs0_(nz);

    for(int k = nz - 1; k >= 0; --k)
        for(int i = 0; i < nxb; ++i)
            prs_(i, k) = prs_(i, k + 1) + gdth * snow_(i, k);
}

void Solver::progIsendens() noexcept
{
    SOLVER_DECLARE_ALL_ALIASES

    const double dtdx05 = 0.5 * dtdx_;
    const int nxnb = nx + nb;

    for(int k = 0; k < nz; ++k)
        for(int i = nb; i < nxnb; ++i)
            snew_(i, k) = sold_(i, k)
                          - (dtdx05) * (snow_(i + 1, k) * (unow_(i + 2, k) + unow_(i + 1, k))
                                        - snow_(i - 1, k) * (unow_(i, k) + unow_(i - 1, k)));
}

void Solver::progVelocity() noexcept
{
    SOLVER_DECLARE_ALL_ALIASES

    const double dtdx = dtdx_;
    const double dtdx2 = 2 * dtdx_;
    const int nx1nb = nx + nb + 1;

    for(int k = 0; k < nz; ++k)
    {
        for(int i = nb; i < nx1nb; ++i)
        {
            unew_(i, k) = uold_(i, k) - dtdx * unow_(i, k) * (unow_(i + 1, k) - unow_(i - 1, k))
                          - dtdx2 * (mtg_(i, k) - mtg_(i - 1, k));
        }
    }
}

void Solver::progMoisture() noexcept
{
    SOLVER_DECLARE_ALL_ALIASES

    const double dtdx05 = 0.5 * dtdx_;
    const int nxnb = nx + nb;

    // Water vapor (qv)
    for(int k = 0; k < nz; ++k)
        for(int i = nb; i < nxnb; ++i)
            qvnew_(i, k)
                = qvold_(i, k) - dtdx05 * (unow_(i, k) + unow_(i + 1, k)) * (qvnow_(i + 1, k) - qvnow_(i - 1, k));

    // Specific cloud water content (qc)
    for(int k = 0; k < nz; ++k)
        for(int i = nb; i < nxnb; ++i)
            qcnew_(i, k)
                = qcold_(i, k) - dtdx05 * (unow_(i, k) + unow_(i + 1, k)) * (qcnow_(i + 1, k) - qcnow_(i - 1, k));

    // Specific rain water content (qr)
    for(int k = 0; k < nz; ++k)
        for(int i = nb; i < nxnb; ++i)
            qrnew_(i, k)
                = qrold_(i, k) - dtdx05 * (unow_(i, k) + unow_(i + 1, k)) * (qrnow_(i + 1, k) - qrnow_(i - 1, k));
}

void Solver::progNumdens() noexcept
{
    SOLVER_DECLARE_ALL_ALIASES
}

void Solver::write(std::string filename)
{
    output_->write(filename);
}

ISEN_NAMESPACE_END
