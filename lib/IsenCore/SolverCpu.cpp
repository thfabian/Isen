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

#define MIB_OFF
#include <mib.h>

#ifdef ISEN_PYTHON
#include <boost/python.hpp>
#endif

ISEN_NAMESPACE_BEGIN

SolverCpu::SolverCpu(std::shared_ptr<NameList> namelist, Output::ArchiveType archiveType)
    : Base(namelist, archiveType)
{}

// -------------------------------------------------- horizontalDiffusion ----------------------------------------------
ISEN_NO_INLINE void kernel_horizontalDiffusion(const int nx,
                                               const int nz,
                                               const int nb,
                                               double* ISEN_RESTRICT unew,
                                               double* ISEN_RESTRICT snew,
                                               double* ISEN_RESTRICT qvnew,
                                               double* ISEN_RESTRICT qcnew,
                                               double* ISEN_RESTRICT qrnew,
                                               const double* ISEN_RESTRICT unow,
                                               const double* ISEN_RESTRICT snow,
                                               const double* ISEN_RESTRICT qvnow,
                                               const double* ISEN_RESTRICT qcnow,
                                               const double* ISEN_RESTRICT qrnow,
                                               const double* ISEN_RESTRICT tau,
                                               const bool imoist)
{
    const int nxnb = nx + nb;
    const int nxnb1 = nx + nb + 1;

    const int nxb = nx + 2 * nb;
    const int nxb1 = nx + 2 * nb + 1;

#pragma omp parallel for
    for(int k = 0; k < nz; ++k)
    {
        const double tau025 = 0.25 * tau[k];

        // Velocity
        if(tau[k] > 0.0)
            for(int i = nb; i < nxnb1; ++i)
                unew[k * nxb1 + i] = unow[k * nxb1 + i]
                                     + tau025
                                           * (unow[k * nxb1 + i - 1] - 2 * unow[k * nxb1 + i] + unow[k * nxb1 + i + 1]);
        else
            for(int i = nb; i < nxnb1; ++i)
                unew[k * nxb1 + i] = unow[k * nxb1 + i];

        // Isentropic density
        if(tau[k] > 0.0)
            for(int i = nb; i < nxnb; ++i)
                snew[k * nxb + i] = snow[k * nxb + i]
                                    + tau025 * (snow[k * nxb + i - 1] - 2 * snow[k * nxb + i] + snow[k * nxb + i + 1]);
        else
            for(int i = nb; i < nxnb; ++i)
                snew[k * nxb + i] = snow[k * nxb + i];

        if(imoist)
        {
            // qv
            if(tau[k] > 0.0)
                for(int i = nb; i < nxnb; ++i)
                    qvnew[k * nxb + i]
                        = qvnow[k * nxb + i]
                          + tau025 * (qvnow[k * nxb + i - 1] - 2 * qvnow[k * nxb + i] + qvnow[k * nxb + i + 1]);
            else
                for(int i = nb; i < nxnb; ++i)
                    qvnew[k * nxb + i] = qvnow[k * nxb + i];

            // qc
            if(tau[k] > 0.0)
                for(int i = nb; i < nxnb; ++i)
                    qcnew[k * nxb + i]
                        = qcnow[k * nxb + i]
                          + tau025 * (qcnow[k * nxb + i - 1] - 2 * qcnow[k * nxb + i] + qcnow[k * nxb + i + 1]);
            else
                for(int i = nb; i < nxnb; ++i)
                    qcnew[k * nxb + i] = qcnow[k * nxb + i];

            // qr
            if(tau[k] > 0.0)
                for(int i = nb; i < nxnb; ++i)
                    qrnew[k * nxb + i]
                        = qrnow[k * nxb + i]
                          + tau025 * (qrnow[k * nxb + i - 1] - 2 * qrnow[k * nxb + i] + qrnow[k * nxb + i + 1]);
            else
                for(int i = nb; i < nxnb; ++i)
                    qrnew[k * nxb + i] = qrnow[k * nxb + i];
        }
    }
}

void SolverCpu::horizontalDiffusion() noexcept
{
    SOLVER_DECLARE_ALL_ALIASES
    kernel_horizontalDiffusion(nx, nz, nb, unew_.data(), snew_.data(), qvnew_.data(), qcnew_.data(), qrnew_.data(),
                               unow_.data(), snow_.data(), qvnow_.data(), qcnow_.data(), qrnow_.data(), tau_.data(),
                               imoist);
}


// -------------------------------------------------- clipMoisture -----------------------------------------------------

ISEN_NO_INLINE void kernel_clipMoisture(const int nx,
                                        const int nz,
                                        const int nb,
                                        double* ISEN_RESTRICT qnow)
{
    const int nxb = nx + 2 * nb;
    
    for(int k = 0; k < nz; ++k)
        for(int i = 0; i < nxb; ++i)
            qnow[k*nxb + i] = qnow[k*nxb + i] < 0.0 ? 0.0 : qnow[k*nxb + i];
}
                                           
void SolverCpu::clipMoisture() noexcept
{
    SOLVER_DECLARE_ALL_ALIASES
    kernel_clipMoisture(nx, nz, nb, qvnew_.data());
    kernel_clipMoisture(nx, nz, nb, qcnew_.data());
    kernel_clipMoisture(nx, nz, nb, qrnew_.data());
}

// -------------------------------------------------- geometricHeight --------------------------------------------------
ISEN_NO_INLINE void kernel_geometricHeight(const int nx,
                                           const int nz,
                                           const int nb,
                                           double* ISEN_RESTRICT zhtnow,
                                           const double* ISEN_RESTRICT topo,
                                           const double* ISEN_RESTRICT th0,
                                           const double* ISEN_RESTRICT exn,
                                           const double* ISEN_RESTRICT prs,
                                           const double topofact,
                                           const double rcpg05)
{
    const int nxb = nx + 2 * nb;
    const int nz1 = nz + 1;

    for(int i = 0; i < nxb; ++i)
        zhtnow[i] = topo[i] * topofact;

    for(int k = 1; k < nz1; ++k)
    {
        double th0_kminus1 = th0[k - 1];
        double th0_center = th0[k];

        for(int i = 0; i < nxb; ++i)
        {
            double th0exn = th0_kminus1 * exn[(k - 1) * nxb + i] + th0_center * exn[k * nxb + i];
            double prs_delta = (prs[k * nxb + i] - prs[(k - 1) * nxb + i])
                               / (0.5 * (prs[k * nxb + i] + prs[(k - 1) * nxb + i]));
            zhtnow[k * nxb + i] = zhtnow[(k - 1) * nxb + i] - rcpg05 * th0exn * prs_delta;
        }
    }
}

void SolverCpu::geometricHeight() noexcept
{
    SOLVER_DECLARE_ALL_ALIASES
    kernel_geometricHeight(nx, nz, nb, zhtnow_.data(), topo_.data(), th0_.data(), exn_.data(), prs_.data(), topofact_,
                           0.5 * r / cp / g);
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

#pragma omp parallel for
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

#pragma omp parallel for
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

// -------------------------------------------------- progMoisture -----------------------------------------------------
ISEN_NO_INLINE void kernel_progMoisture(const int nx,
                                        const int nz,
                                        const int nb,
                                        double* ISEN_RESTRICT qnew,                                       
                                        const double* ISEN_RESTRICT qnow,
                                        const double* ISEN_RESTRICT qold,
                                        const double* ISEN_RESTRICT unow,
                                        const double dtdx05)
{
    const int nxb = nx + 2 * nb;
    const int nxb1 = nx + 2 * nb + 1;
    const int nxnb = nx + nb;
    
#pragma omp parallel for
    for(int k = 0; k < nz; ++k)
        for(int i = nb; i < nxnb; ++i)
            qnew[k*nxb + i] = qold[k*nxb + i] - dtdx05 * (unow[k*nxb1 + i] + unow[k*nxb1 + i + 1]) 
                                                       * (qnow[k*nxb + i + 1] - qnow[k*nxb + i - 1]);
}

void SolverCpu::progMoisture() noexcept
{
    SOLVER_DECLARE_ALL_ALIASES
    kernel_progMoisture(nx, nz, nb, qvnew_.data(), qvnow_.data(), qvold_.data(), unow_.data(), 0.5 * dtdx_);
    kernel_progMoisture(nx, nz, nb, qcnew_.data(), qcnow_.data(), qcold_.data(), unow_.data(), 0.5 * dtdx_);
    kernel_progMoisture(nx, nz, nb, qrnew_.data(), qrnow_.data(), qrold_.data(), unow_.data(), 0.5 * dtdx_);
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

#pragma omp parallel for
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
        
        // Moisture scalars
        if(imoist)
        {
            MIB_NEXT("progMoisture")        
            progMoisture();
        }
        
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
        MIB_NEXT("horizontalDiffusion")                        
        horizontalDiffusion();
        
        MIB_NEXT("applyBoundary")                                
        if(!irelax)
            applyPeriodicBoundary();

        if(imoist)
        {
            MIB_NEXT("clipMoisture")                                
            clipMoisture();
        }
        
        unow_.swap(unew_);
        snow_.swap(snew_);
        qvnow_.swap(qvnew_);
        qcnow_.swap(qcnew_);
        qrnow_.swap(qrnew_);


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
        if(imoist)
        {
            if(imicrophys == 1) // Kessler scheme
            {
                MIB_NEXT("kessler")                                                                
                kessler_->apply(
                    // Output
                    temp_, qvnew_, qcnew_, qrnew_, tot_prec_, prec_,

                    // Input
                    th0_, prs_, snow_, qvnow_, qcnow_, qrnow_, exn_, zhtnow_);
            }
        }
           
        
        qvnow_.swap(qvnew_);
        qcnow_.swap(qcnew_);
        qrnow_.swap(qrnew_);

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

    MIB_PRINT("nsec")
    
    pbar.pause();
    if(!logIsDisabled)
        Progressbar::printBar('=');

    if(logIsDisabled && itime)
        std::printf("Elapsed time: %s\n", timeString(t.stop()).c_str());

    LOG() << "Finished time loop ...";
    LOG_SUCCESS(t);
}

ISEN_NAMESPACE_END
