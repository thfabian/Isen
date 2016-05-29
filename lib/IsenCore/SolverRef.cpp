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

#include <Isen/Boundary.h>
#include <Isen/Logger.h>
#include <Isen/Output.h>
#include <Isen/Progressbar.h>
#include <Isen/SolverRef.h>
#include <Isen/Timer.h>

#ifdef ISEN_PYTHON
#include <boost/python.hpp>
#endif

ISEN_NAMESPACE_BEGIN

SolverRef::SolverRef(std::shared_ptr<NameList> namelist,
                     Output::ArchiveType archiveType)
    : Solver(namelist, archiveType)
{
}

void SolverRef::init() noexcept
{
    Base::init();
}

void SolverRef::run()
{
    SOLVER_DECLARE_ALL_ALIASES

    Timer t;

    Progressbar pbar(nts);
    const bool logIsDisabled = LOG().isDisabled();
    pbar.disableProgressbar = logIsDisabled;

    Float curTime = 0;

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

        unow_.swap(unew_);
        snow_.swap(snew_);

        // Diffusion and gravity wave absorber
        //--------------------------------------------------------
        horizontalDiffusion();

        if(!irelax)
            applyPeriodicBoundary();

        unow_.swap(unew_);
        snow_.swap(snew_);

        // Diagnostic step
        //--------------------------------------------------------

        // Pressure
        diagPressure();

        // Montgomorey 
        diagMontgomery();

        // Calculation of geometric height (staggered) 
        //--------------------------------------------------------


        // Microphysics
        //--------------------------------------------------------

        
        // Check maximum CFL condition   
        //--------------------------------------------------------  
        Float umax = computeCFL();
        Float cflmax = umax * dtdx_;

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
        std::printf("Elapsed time: %s\n", timeString(t.stop()));

    LOG() << "Finished time loop ...";
    LOG_SUCCESS(t);
}

Float SolverRef::computeCFL() const noexcept
{
    SOLVER_DECLARE_ALL_ALIASES

    Float umax = -std::numeric_limits<Float>::max();
    for(int k = 0; k < nz; ++k)
        for(int i = 0; i < nxb; ++i)
            umax = std::max(umax, std::fabs(unow_(i, k)));
    return umax;
}

void SolverRef::horizontalDiffusion() noexcept
{
    SOLVER_DECLARE_ALL_ALIASES

    const int nxnb = nx + nb;
    const int nxnb1 = nx + nb + 1;

    // Solve diffusion equation
    //------------------------------------------------------------
    for(int k = 0; k < nz; ++k)
    {
        const Float tau = tau_(k);
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
    }
}

void SolverRef::applyPeriodicBoundary() noexcept
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

void SolverRef::applyRelaxationBoundary() noexcept
{
    SOLVER_DECLARE_ALL_ALIASES

    assert(irelax);
    //Boundary::relax(snew_, nx, nb, sbnd1_, sbnd2_);
    //Boundary::relax(unew_, nx1, nb, ubnd1_ ,ubnd2_);
}

void SolverRef::diagMontgomery() noexcept
{
    SOLVER_DECLARE_ALL_ALIASES

    const Float dth05 = dth * 0.5;
    const Float gtopofact_ = g * topofact_;

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

void SolverRef::diagPressure() noexcept
{
    SOLVER_DECLARE_ALL_ALIASES

    const Float gdth = g* dth;

    for(int i = 0; i < nxb; ++i)
        prs_(i, nz) = prs0_(nz);

    for(int k = nz - 1; k >= 0; --k)
        for(int i = 0; i < nxb; ++i)
            prs_(i, k) = prs_(i, k + 1) + gdth * snow_(i, k);
}

void SolverRef::progIsendens() noexcept
{
    SOLVER_DECLARE_ALL_ALIASES

    const Float dtdx05 = 0.5 * dtdx_;
    const int nxnb = nx + nb;

    for(int k = 0; k < nz; ++k)
        for(int i = nb; i < nxnb; ++i)
            snew_(i, k) = sold_(i, k)
                          - (dtdx05) * (snow_(i + 1, k) * (unow_(i + 2, k) + unow_(i + 1, k))
                                       - snow_(i - 1, k) * (unow_(i, k) + unow_(i - 1, k)));
}

void SolverRef::progVelocity() noexcept
{
    SOLVER_DECLARE_ALL_ALIASES

    const Float dtdx = dtdx_;
    const Float dtdx2 = 2 * dtdx_;
    const int nxnb = nx + nb + 1;

    for(int k = 0; k < nz; ++k)
    {
        for(int i = nb; i < nxnb; ++i)
        {
            unew_(i, k) = uold_(i, k) - dtdx * unow_(i, k) * (unow_(i + 1, k) - unow_(i - 1, k))
                          - dtdx2 * (mtg_(i, k) - mtg_(i - 1, k));
        }
    }
}

void SolverRef::progMoisture() noexcept
{
    SOLVER_DECLARE_ALL_ALIASES
}

void SolverRef::progNumdens() noexcept
{
    SOLVER_DECLARE_ALL_ALIASES
}

void SolverRef::write(std::string filename)
{
    Base::write(filename);
}

ISEN_NAMESPACE_END
