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

#define _USE_MATH_DEFINES
#include <cmath>

#include <Isen/Boundary.h>
#include <Isen/Logger.h>
#include <Isen/Output.h>
#include <Isen/Progressbar.h>
#include <Isen/SolverCpu.h>
#include <Isen/Timer.h>

//#ifndef NDEBUG
#define MIB_OFF
//#endif
#include <mib.h>

#ifdef ISEN_PYTHON
#include <boost/python.hpp>
#endif

ISEN_NAMESPACE_BEGIN

SolverCpu::SolverCpu(std::shared_ptr<NameList> namelist, Output::ArchiveType archiveType)
    : Base(namelist, archiveType)
{}

void SolverCpu::horizontalDiffusion() noexcept
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
    }
}

void SolverCpu::geometricHeight() noexcept
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
// -------------------------------------------------- diagMontgomery ---------------------------------------------------
ISEN_NO_INLINE void kernel_diagMontgomery_Exner(const int nx,
                                                const int nz,
                                                const int nb,
                                                double* ISEN_RESTRICT exn,
                                                const double* ISEN_RESTRICT prs,
                                                const double cp,
                                                const double pref,
                                                const double rdcp)
{
    const int nxb = nx + 2 * nb;
    const int nz1 = nz + 1;
    
    const double fac = cp * std::pow(1.0 / pref, rdcp);

//#pragma omp parallel for num_threads(getNumThreads(nx, nz))
    for(int k = 0; k < nz1; ++k)
        for(int i = 0; i < nxb; ++i)
            exn[k * nxb + i] = fac * std::pow(prs[k * nxb + i], rdcp);
}

ISEN_NO_INLINE void kernel_diagMontgomery_Montgomery(const int nx,
                                                     const int nz,
                                                     const int nb,
                                                     double* ISEN_RESTRICT mtg,
                                                     const double* ISEN_RESTRICT topo,
                                                     const double* ISEN_RESTRICT exn,
                                                     const double th0,
                                                     const double cp,
                                                     const double dth,
                                                     const double gtopofact)
{
    const int nxb = nx + 2 * nb;
    const double th0dth05 = dth * 0.5 + th0;

    for(int i = 0; i < nxb; ++i)
        mtg[i] = gtopofact * topo[i] + th0dth05 * exn[i];

    for(int k = 1; k < nz; ++k)
        for(int i = 0; i < nxb; ++i)
            mtg[k * nxb + i] = mtg[(k - 1) * nxb + i] + dth * exn[k * nxb + i];
}

void SolverCpu::diagMontgomery() noexcept
{
    SOLVER_DECLARE_ALL_ALIASES

    // Exner function
    kernel_diagMontgomery_Exner(nx, nz, nb, exn_.data(), prs_.data(), cp, pref, rdcp);

    // Montgomery
    kernel_diagMontgomery_Montgomery(nx, nz, nb, mtg_.data(), topo_.data(), exn_.data(), th0_(0), cp, dth,  g * topofact_);
}


// -------------------------------------------------- diagPressure -----------------------------------------------------
ISEN_NO_INLINE void kernel_diagPressure(const int nxb,
                                        const int nz,
                                        double* ISEN_RESTRICT prs,
                                        const double* ISEN_RESTRICT snow,
                                        const double gdth,
                                        const double prs0)
{
    const int nz_offset = nz * nxb;

    for(int i = 0; i < nxb; ++i)
        prs[nz_offset + i] = prs0;

    for(int k = nz - 1; k >= 0; --k)
        for(int i = 0; i < nxb; ++i)
            prs[k * nxb + i] = prs[(k + 1) * nxb + i] + gdth * snow[k * nxb + i];
}

void SolverCpu::diagPressure() noexcept
{
    SOLVER_DECLARE_ALL_ALIASES
    kernel_diagPressure(nxb, nz, prs_.data(), snow_.data(), g * dth, prs0_(nz));
}

// -------------------------------------------------- progIsendens -----------------------------------------------------
ISEN_NO_INLINE void kernel_progIsendens(const int nx,
                                        const int nz,
                                        const int nb,
                                        double* ISEN_RESTRICT snew,
                                        const double* ISEN_RESTRICT snow,
                                        const double* ISEN_RESTRICT sold,
                                        const double* ISEN_RESTRICT unow,
                                        const double dtdx05)
{
    const int nxb = nx + 2 * nb;
    const int nxb1 = nx + 2 * nb + 1;
    const int nxnb = nx + nb;

    for(int k = 0; k < nz; ++k)
        for(int i = nb; i < nxnb; ++i)
        {
            double snow_iplus1 = snow[k*nxb + i + 1] * (unow[k*nxb1 + i + 2] + unow[k*nxb1 + i + 1]);
            double snow_iminus1 = snow[k*nxb + i - 1] * (unow[k*nxb1 + i] + unow[k*nxb1 + i -1]);
            snew[k*nxb + i] = sold[k*nxb + i] - dtdx05 * (snow_iplus1 - snow_iminus1);
        }
}

void SolverCpu::progIsendens() noexcept
{
    SOLVER_DECLARE_ALL_ALIASES
    kernel_progIsendens(nx, nz, nb, snew_.data(), snow_.data(), sold_.data(), unow_.data(), 0.5 * dtdx_);
}

// -------------------------------------------------- progVelocity -----------------------------------------------------
ISEN_NO_INLINE void kernel_progVelocity(const int nx,
                                        const int nz,
                                        const int nb,
                                        double* ISEN_RESTRICT unew,
                                        const double* ISEN_RESTRICT unow,
                                        const double* ISEN_RESTRICT uold,
                                        const double* ISEN_RESTRICT mtg,
                                        const double dtdx)
{
    const int nxb = nx + 2 * nb;
    const int nx1b = nx + 2 * nb + 1;
    const int nx1nb = nx + nb + 1;

    const double dtdx2 = 2 * dtdx;

    for(int k = 0; k < nz; ++k)
        for(int i = nb; i < nx1nb; ++i)
        {
            double unow_delta = unow[k * nx1b + i] * (unow[k * nx1b + i + 1] - unow[k * nx1b + i - 1]);
            double mtg_dtdx2 = dtdx2 * (mtg[k * nxb + i] - mtg[k * nxb + i - 1]);
            unew[k * nx1b + i] = uold[k * nx1b + i] - dtdx * unow_delta - mtg_dtdx2;
        }
}

void SolverCpu::progVelocity() noexcept
{
    SOLVER_DECLARE_ALL_ALIASES
    kernel_progVelocity(nx, nz, nb, unew_.data(), unow_.data(), uold_.data(), mtg_.data(), dtdx_);
}

// ---------------------------------------------------------------------------------------------------------------------
void SolverCpu::run()
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
        MIB_START("progIsendens")        
        progIsendens();

        // Velocity
        MIB_NEXT("progVelocity")        
        progVelocity();

        // Exchange boundaries if periodic
        //--------------------------------------------------------
        MIB_NEXT("applyBoundary")                
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
        MIB_NEXT("horizontalDiffusion")                        
        horizontalDiffusion();
        
        MIB_NEXT("applyBoundary")                                
        if(!irelax)
            applyPeriodicBoundary();

        unow_.swap(unew_);
        snow_.swap(snew_);

        // Diagnostic step
        //--------------------------------------------------------

        // Pressure
        MIB_NEXT("diagPressure")                                        
        diagPressure();

        // Montgomorey
        MIB_NEXT("diagMontgomery")                                                
        diagMontgomery();

        // Calculation of geometric height (staggered)
        //--------------------------------------------------------
        zhtnow_.swap(zhtold_);
        MIB_NEXT("geometricHeight")                                                
        geometricHeight();

        // Microphysics
        //---------------------------------------------------------


        // Check maximum CFL condition
        //--------------------------------------------------------
        MIB_NEXT("computeCFL")                                                        
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
        MIB_NEXT("makeOutput")                                                                
        if((i % iout) == 0)
            output_->makeOutput(this);
        MIB_STOP("makeOutput")        

#ifdef ISEN_PYTHON
        // Handle Python signals
        //--------------------------------------------------------
        if(PyErr_CheckSignals() == -1)
            throw IsenException("PySolver::run : signal caught");
#endif
    }

    MIB_PRINT("cycle")
    
    pbar.pause();
    if(!logIsDisabled)
        Progressbar::printBar('=');

    if(logIsDisabled && itime)
        std::printf("Elapsed time: %s\n", timeString(t.stop()).c_str());

    LOG() << "Finished time loop ...";
    LOG_SUCCESS(t);
}

ISEN_NAMESPACE_END
