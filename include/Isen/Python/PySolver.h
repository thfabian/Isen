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

/// @brief Expose the Solver interface to python
class PySolver
{
public:
    enum SolverType
    {
        SolverTypeRef
    };

    /// Set the implementation of the solver
    PySolver(const char* type = "ref");

    /// Initialize simulation
    void init(const char* filename = "");

    /// Run simulation
    void run();

    /// Print NameList
    void print() const;

    /// Write to output file
    void write(Output::ArchiveType archiveType = Output::Unknown, const char* filename = "");

    /// Set verbosity (true : enable)
    void setVerbosity(bool verbose)
    {
        if(verbose)
            LOG() << log::enable;
        else
            LOG() << log::disable;
    }

private:
    std::shared_ptr<Solver> solver_;
    std::shared_ptr<NameList> namelist_;
    std::shared_ptr<Parser> parser_;
    bool isInitialized_;
    SolverType solverType_;

public:
    boost::python::object topo() const
    {
        if(!isInitialized_)
            throw IsenException("Solver: not initialized");
        return toNumpyArray(solver_->topo());
    }
    
    boost::python::object zhtold() const
    {
        if(!isInitialized_)
            throw IsenException("Solver: not initialized");
        return toNumpyArray(solver_->zhtold());
    }
    
    boost::python::object zhtnow() const
    {
        if(!isInitialized_)
            throw IsenException("Solver: not initialized");
        return toNumpyArray(solver_->zhtnow());
    }
    
    boost::python::object uold() const
    {
        if(!isInitialized_)
            throw IsenException("Solver: not initialized");
        return toNumpyArray(solver_->uold());
    }
    
    boost::python::object unow() const
    {
        if(!isInitialized_)
            throw IsenException("Solver: not initialized");
        return toNumpyArray(solver_->unow());
    }
    
    boost::python::object unew() const
    {
        if(!isInitialized_)
            throw IsenException("Solver: not initialized");
        return toNumpyArray(solver_->unew());
    }
    
    boost::python::object sold() const
    {
        if(!isInitialized_)
            throw IsenException("Solver: not initialized");
        return toNumpyArray(solver_->sold());
    }
    
    boost::python::object snow() const
    {
        if(!isInitialized_)
            throw IsenException("Solver: not initialized");
        return toNumpyArray(solver_->snow());
    }
    
    boost::python::object snew() const
    {
        if(!isInitialized_)
            throw IsenException("Solver: not initialized");
        return toNumpyArray(solver_->snew());
    }
    
    boost::python::object mtg() const
    {
        if(!isInitialized_)
            throw IsenException("Solver: not initialized");
        return toNumpyArray(solver_->mtg());
    }
    
    boost::python::object mtgnew() const
    {
        if(!isInitialized_)
            throw IsenException("Solver: not initialized");
        return toNumpyArray(solver_->mtgnew());
    }

    boost::python::object mtg0() const
    {
        if(!isInitialized_)
            throw IsenException("Solver: not initialized");
        return toNumpyArray(solver_->mtg0());
    }
    
    boost::python::object exn() const
    {
        if(!isInitialized_)
            throw IsenException("Solver: not initialized");
        return toNumpyArray(solver_->exn());
    }

    boost::python::object exn0() const
    {
        if(!isInitialized_)
            throw IsenException("Solver: not initialized");
        return toNumpyArray(solver_->exn0());
    }
    
    boost::python::object prs() const
    {
        if(!isInitialized_)
            throw IsenException("Solver: not initialized");
        return toNumpyArray(solver_->prs());
    }

    boost::python::object prs0() const
    {
        if(!isInitialized_)
            throw IsenException("Solver: not initialized");
        return toNumpyArray(solver_->prs0());
    }
    
    boost::python::object tau() const
    {
        if(!isInitialized_)
            throw IsenException("Solver: not initialized");
        return toNumpyArray(solver_->tau());
    }

    boost::python::object th0() const
    {
        if(!isInitialized_)
            throw IsenException("Solver: not initialized");
        return toNumpyArray(solver_->th0());
    }
    
    boost::python::object prec() const
    {
        if(!isInitialized_)
            throw IsenException("Solver: not initialized");
        return toNumpyArray(solver_->prec());
    }
    
    boost::python::object tot_prec() const
    {
        if(!isInitialized_)
            throw IsenException("Solver: not initialized");
        return toNumpyArray(solver_->tot_prec());
    }
    
    boost::python::object qvold() const
    {
        if(!isInitialized_)
            throw IsenException("Solver: not initialized");
        return toNumpyArray(solver_->qvold());
    }
    
    boost::python::object qvnow() const
    {
        if(!isInitialized_)
            throw IsenException("Solver: not initialized");
        return toNumpyArray(solver_->qvnow());
    }
    
    boost::python::object qvnew() const
    {
        if(!isInitialized_)
            throw IsenException("Solver: not initialized");
        return toNumpyArray(solver_->qvnew());
    }
    
    boost::python::object qcold() const
    {
        if(!isInitialized_)
            throw IsenException("Solver: not initialized");
        return toNumpyArray(solver_->qcold());
    }
    
    boost::python::object qcnow() const
    {
        if(!isInitialized_)
            throw IsenException("Solver: not initialized");
        return toNumpyArray(solver_->qcnow());
    }
    
    boost::python::object qcnew() const
    {
        if(!isInitialized_)
            throw IsenException("Solver: not initialized");
        return toNumpyArray(solver_->qcnew());
    }
    
    boost::python::object qrold() const
    {
        if(!isInitialized_)
            throw IsenException("Solver: not initialized");
        return toNumpyArray(solver_->qrold());
    }
    
    boost::python::object qrnow() const
    {
        if(!isInitialized_)
            throw IsenException("Solver: not initialized");
        return toNumpyArray(solver_->qrnow());
    }
    
    boost::python::object qrnew() const
    {
        if(!isInitialized_)
            throw IsenException("Solver: not initialized");
        return toNumpyArray(solver_->qrnew());
    }
    
    boost::python::object nrold() const
    {
        if(!isInitialized_)
            throw IsenException("Solver: not initialized");
        return toNumpyArray(solver_->nrold());
    }
    
    boost::python::object nrnow() const
    {
        if(!isInitialized_)
            throw IsenException("Solver: not initialized");
        return toNumpyArray(solver_->nrnow());
    }
    
    boost::python::object nrnew() const
    {
        if(!isInitialized_)
            throw IsenException("Solver: not initialized");
        return toNumpyArray(solver_->nrnew());
    }
    
    boost::python::object ncold() const
    {
        if(!isInitialized_)
            throw IsenException("Solver: not initialized");
        return toNumpyArray(solver_->ncold());
    }
    
    boost::python::object ncnow() const
    {
        if(!isInitialized_)
            throw IsenException("Solver: not initialized");
        return toNumpyArray(solver_->ncnow());
    }
    
    boost::python::object ncnew() const
    {
        if(!isInitialized_)
            throw IsenException("Solver: not initialized");
        return toNumpyArray(solver_->ncnew());
    }
    
    boost::python::object dthetadt() const
    {
        if(!isInitialized_)
            throw IsenException("Solver: not initialized");
        return toNumpyArray(solver_->dthetadt());
    }
    
    boost::python::object sbnd1() const
    {
        if(!isInitialized_)
            throw IsenException("Solver: not initialized");
        return toNumpyArray(solver_->sbnd1());
    }
    
    boost::python::object sbnd2() const
    {
        if(!isInitialized_)
            throw IsenException("Solver: not initialized");
        return toNumpyArray(solver_->sbnd2());
    }

    boost::python::object tbnd1() const
    {
        if(!isInitialized_)
            throw IsenException("Solver: not initialized");
        return toNumpyArray(solver_->tbnd1());
    }
    
    boost::python::object tbnd2() const
    {
        if(!isInitialized_)
            throw IsenException("Solver: not initialized");
        return toNumpyArray(solver_->tbnd2());
    }
    
    boost::python::object ubnd1() const
    {
        if(!isInitialized_)
            throw IsenException("Solver: not initialized");
        return toNumpyArray(solver_->ubnd1());
    }
    
    boost::python::object ubnd2() const
    {
        if(!isInitialized_)
            throw IsenException("Solver: not initialized");
        return toNumpyArray(solver_->ubnd2());
    }
    
    boost::python::object qvbnd1() const
    {
        if(!isInitialized_)
            throw IsenException("Solver: not initialized");
        return toNumpyArray(solver_->qvbnd1());
    }
    
    boost::python::object qvbnd2() const
    {
        if(!isInitialized_)
            throw IsenException("Solver: not initialized");
        return toNumpyArray(solver_->qvbnd2());
    }
    
    boost::python::object qcbnd1() const
    {
        if(!isInitialized_)
            throw IsenException("Solver: not initialized");
        return toNumpyArray(solver_->qcbnd1());
    }
    
    boost::python::object qcbnd2() const
    {
        if(!isInitialized_)
            throw IsenException("Solver: not initialized");
        return toNumpyArray(solver_->qcbnd2());
    }
    
    boost::python::object qrbnd1() const
    {
        if(!isInitialized_)
            throw IsenException("Solver: not initialized");
        return toNumpyArray(solver_->qrbnd1());
    }
    
    boost::python::object qrbnd2() const
    {
        if(!isInitialized_)
            throw IsenException("Solver: not initialized");
        return toNumpyArray(solver_->qrbnd2());
    }
    
    boost::python::object dthetadtbnd1() const
    {
        if(!isInitialized_)
            throw IsenException("Solver: not initialized");
        return toNumpyArray(solver_->dthetadtbnd1());
    }
    
    boost::python::object dthetadtbnd2() const
    {
        if(!isInitialized_)
            throw IsenException("Solver: not initialized");
        return toNumpyArray(solver_->dthetadtbnd2());
    }
    
    boost::python::object nrbnd1() const
    {
        if(!isInitialized_)
            throw IsenException("Solver: not initialized");
        return toNumpyArray(solver_->nrbnd1());
    }
    
    boost::python::object nrbnd2() const
    {
        if(!isInitialized_)
            throw IsenException("Solver: not initialized");
        return toNumpyArray(solver_->nrbnd2());
    }
    
    boost::python::object ncbnd1() const
    {
        if(!isInitialized_)
            throw IsenException("Solver: not initialized");
        return toNumpyArray(solver_->ncbnd1());
    }
    
    boost::python::object ncbnd2() const
    {
        if(!isInitialized_)
            throw IsenException("Solver: not initialized");
        return toNumpyArray(solver_->ncbnd2());
    }
};

ISEN_NAMESPACE_END

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(PySolver_overload_init, init, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(PySolver_overload_write, write, 0, 2)

#endif
