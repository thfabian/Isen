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
#ifndef ISEN_SOLVER_REF_H
#define ISEN_SOLVER_REF_H

#include <Isen/Common.h>
#include <Isen/Solver.h>

ISEN_NAMESPACE_BEGIN

/// @brief Single threaded refrence implementation of the Solver
class SolverRef : public Solver
{
public:
    using Base = Solver;

    /// Allocate necessary memory
    SolverRef(std::shared_ptr<NameList> namelist, Output::ArchiveType archiveType = Output::ArchiveType::Text);

    /// Free all memory
    ~SolverRef() {}

    /// Initialize the simulation (See Solver::init)
    virtual void init() noexcept override;

    /// Run the simulation (See Solver::run)
    virtual void run() noexcept override;

    /// Write simulation to output file (See Solver::write)
    virtual void write(std::string filename = "") override;

    //------------------------------------------------------------
    // Diffusion
    //------------------------------------------------------------

    /// Horizontal diffusion for dry model
    void horizontalDiffusion() noexcept;

    //------------------------------------------------------------
    // Diagnostic
    //------------------------------------------------------------

    /// Diagnostic computation of Montgomery
    void diagMontgomery() noexcept;

    /// Diagnostic computation of pressure
    void diagPressure() noexcept;

    //------------------------------------------------------------
    // Prognostic
    //------------------------------------------------------------

    /// Prognostic step for isentropic mass density
    void progIsendens() noexcept;

    /// Prognostic step for momentum
    void progVelocity() noexcept;

    /// Prognostic step for hydrometeors
    void progMoisture() noexcept;

    /// Prognostic step for number densities
    void progNumdens() noexcept;
};

ISEN_NAMESPACE_END

#endif
