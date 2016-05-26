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
    Float curTime = 0;

    // Loop over all time steps
    //------------------------------------------------------------
    for(int i = 1; i < (nts + 1); ++i)
    {
        curTime += dt;
        pbar.advance();
        
//        usleep(500);
        //Sleep(1);

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

    // untested
    VectorXf tau = VectorXf::Constant(nxb1, tau_.sum());
    VectorXf sel = (tau.array() > 0.0).cast<Float>();
    
    // Solve diffusion equation
    //------------------------------------------------------------


    // Exchange periodic boundaries
    //------------------------------------------------------------


    // Update fields
    //------------------------------------------------------------
    unow_ = unew_;
    snow_ = snew_;
}

void SolverRef::diagMontgomery() noexcept
{
}

void SolverRef::diagPressure() noexcept
{
}

void SolverRef::progIsendens() noexcept
{
}

void SolverRef::progVelocity() noexcept
{
}

void SolverRef::progMoisture() noexcept
{
}

void SolverRef::progNumdens() noexcept
{
}

void SolverRef::write(std::string filename)
{
    Base::write(filename);
}

ISEN_NAMESPACE_END
