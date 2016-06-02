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
#ifndef ISEN_SOLVER_FACTORY_H
#define ISEN_SOLVER_FACTORY_H

#include <Isen/Common.h>
#include <Isen/Output.h>
#include <Isen/Solver.h>
#include <Isen/SolverOpt.h>
#include <string>

ISEN_NAMESPACE_BEGIN

/// @brief Create the correct version of the Solver
struct SolverFactory
{
    /// @brief Create the Solver instance given by @c name
    ///
    /// @param name         Name of the Solver. If the name is empty, the refrence implementation will be used.
    /// @param namelist     Namelist containing the simulation variables.
    /// @param archiveType  Archive used for serializing the output [default: Output::ArchiveType::Text].
    ///
    /// @return pointer to the newly created Solver
    static std::shared_ptr<Solver> create(std::string name,
                                          std::shared_ptr<NameList> namelist,
                                          Output::ArchiveType archiveType = Output::ArchiveType::Text)
    {
        if(name.empty() || name == "ref")
            return std::make_shared<Solver>(namelist, archiveType);
        else if(name == "opt")
            return std::make_shared<SolverOpt>(namelist, archiveType);
        else
            throw IsenException("invalid Solver name '%s'", name);
    }
};

ISEN_NAMESPACE_END

#endif
