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
#ifndef ISEN_SOLVER_OPT_H
#define ISEN_SOLVER_OPT_H

#include <Isen/Common.h>
#include <Isen/Solver.h>

ISEN_NAMESPACE_BEGIN

/// @brief Multi threaded CPU optimized version
class SolverCpu : public Solver
{
public:
    using Base = Solver;

    /// @brief Allocate memory 
    ///
    /// @throw IsenException if out of memory
    SolverCpu(std::shared_ptr<NameList> namelist, Output::ArchiveType archiveType = Output::ArchiveType::Text);

    //------------------------------------------------------------
    // Diffusion
    //------------------------------------------------------------

    /// Horizontal diffusion
    virtual void horizontalDiffusion() noexcept override;
    
    //------------------------------------------------------------
    // Geometric height 
    //------------------------------------------------------------
    
    /// Calculate geometric height 
    virtual void geometricHeight() noexcept override;
    
    //------------------------------------------------------------
    // Boundary
    //------------------------------------------------------------

    /// Clip negative values of moisture variables
    virtual void clipMoisture() noexcept override;
    
    //------------------------------------------------------------
    // Diagnostic
    //------------------------------------------------------------

    /// Diagnostic computation of Montgomery
    virtual void diagMontgomery() noexcept override;

    /// Diagnostic computation of pressure
    virtual void diagPressure() noexcept override;

    //------------------------------------------------------------
    // Prognostic
    //------------------------------------------------------------

    /// Prognostic step for isentropic mass density
    virtual void progIsendens() noexcept override;

    /// Prognostic step for momentum
    virtual void progVelocity() noexcept override;
    
    /// Prognostic step for hydrometeors
    virtual void progMoisture() noexcept override;
    
    /// Free all memory
    virtual ~SolverCpu() {}
};

ISEN_NAMESPACE_END

#endif
