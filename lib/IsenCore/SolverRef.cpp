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

void SolverRef::run() noexcept
{
    SOLVER_DECLARE_ALL_ALIASES

    Timer t;

    Progressbar pbar(nts);
    pbar.disableProgressbar = LOG().isDisabled();

    Float curTime = 0;

    // Loop over all time steps
    //------------------------------------------------------------
    for(int i = 1; i < (nts + 1); ++i)
    {
        pbar.advance();

        curTime += dt;
        topofact_ = std::min(1., curTime / topotim);

        // Special treatment of first time step
        //--------------------------------------------------------
        if(i == 1)
            dtdx_ = 0.5 * dt / dx;
        else
            dtdx_ = dt / dx;

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

        // Montgomorey 
        diagMontgomery();

        // Pressure
        diagPressure();

        // Calculation of geometric height (staggered) 
        //--------------------------------------------------------


        // Microphysics
        //--------------------------------------------------------

        
        // Check maximum CFL condition   
        //--------------------------------------------------------
        //iprtcfl


        // Output every 'iout'-th time step
        //--------------------------------------------------------
        if((i % iout) == 0)
            output_->makeOutput(this);
    }


    pbar.pause();
    if(!LOG().isDisabled())
        Progressbar::printBar('=');

    LOG() << "Finished time loop ...";
    LOG_SUCCESS(t);
}

void SolverRef::horizontalDiffusion() noexcept
{
    SOLVER_DECLARE_ALL_ALIASES

    const int nxnb = nx + nb;
    const int nxnb1 = nx + nb + 1;

    bool tauIsNegative = false;
    for(int k = 0; k < nz; ++k)
        tauIsNegative |= tau_(k) <= 0.0;


    // Solve diffusion equation
    //------------------------------------------------------------
    for(int k = 0; k < nz; ++k)
    {
        Float tau = tau_(k);
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
}

void SolverRef::diagPressure() noexcept
{
    SOLVER_DECLARE_ALL_ALIASES
}

void SolverRef::progIsendens() noexcept
{
    SOLVER_DECLARE_ALL_ALIASES

    const Float dtdx05 = 0.5 * dtdx_;
    const int nxnb = nx + nb;

    for(int k = 0; k < nz; ++k)
    {
        for(int i = nb; i < nxnb; ++i)
        {
            snew_(i, k) = sold_(i, k)
                          - (dtdx05) * (snow_(i + 1, k) * (unow_(i + 2, k) + unow_(i + 1, k))
                                       - snow_(i - 1, k) * (unow_(i, k) + unow_(i - 1, k)));
        }
    }
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
