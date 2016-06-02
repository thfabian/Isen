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

/// @brief Single threaded CPU optimized version
class SolverOpt : public Solver
{
public:
    using Base = Solver;

    /// @brief Allocate memory 
    ///
    /// @throw IsenException if out of memory
    SolverOpt(std::shared_ptr<NameList> namelist, Output::ArchiveType archiveType = Output::ArchiveType::Text);

    /// Free all memory
    virtual ~SolverOpt() {}
};

ISEN_NAMESPACE_END

#endif
