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
#ifndef ISEN_PYTHON_SOLVER_H
#define ISEN_PYTHON_SOLVER_H

#include <Isen/Common.h>
#include <Isen/Logger.h>
#include <Isen/Python/IsenPython.h>
#include <Isen/Python/PyType.h>
#include <Isen/Solver.h>
#include <memory>
#include <string>

ISEN_NAMESPACE_BEGIN

/// @brief Expose the Solver interface to Python
class PySolver
{
public:
    /// Set the implementation of the solver
    PySolver(const char* name = "ref");

    /// Initialize simulation
    void init(const char* filename = "");

    /// Run simulation
    void run();

    /// Print NameList
    void print() const;

    /// Write to output file
    void write(Output::ArchiveType archiveType = Output::Unknown, const char* filename = "");
    
    /// Get field by name
    boost::python::object getField(const char* name) const
    {
        // This needs to be in a header file for some reason
        if(!isInitialized_)
            throw IsenException("Solver: not initialized");
        
        MatrixXf mat(solver_->getField(name));
        return toNumpyArray(mat);
    }

private:
    std::shared_ptr<Solver> solver_;
    std::shared_ptr<NameList> namelist_;
    std::shared_ptr<Parser> parser_;
    bool isInitialized_;
    std::string name_;
};

ISEN_NAMESPACE_END

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(PySolver_overload_init, init, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(PySolver_overload_write, write, 0, 2)

#endif
