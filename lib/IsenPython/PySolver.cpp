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

#include <Isen/Parse.h>
#include <Isen/Python/PySolver.h>
#include <Isen/SolverFactory.h>

ISEN_NAMESPACE_BEGIN

PySolver::PySolver(const char* name) : parser_(new Parser), isInitialized_(false), name_(name)
{
}

void PySolver::initWithNameList(PyNameList namelist)
{
    // Copy NameList
    namelist_ = namelist.getNameList();

    if(solver_)
        solver_.reset();
    
    // Construct Solver
    solver_ = SolverFactory::create(name_, namelist_);

    solver_->init();
    isInitialized_ = true;
}

void PySolver::initWithFile(const char* filename)
{
    // Construct NameList
    std::string filenameS(filename);
    if(filenameS.empty())
        namelist_ = std::make_shared<NameList>();
    else
    {
        try
        {
            namelist_ = parser_->parse(filenameS);
        }
        catch(const IsenException& err)
        {
            throw IsenException("Solver: %s", err.what());
        }
    }

    if(solver_)
        solver_.reset();

    // Construct Solver
    solver_ = SolverFactory::create(name_, namelist_);

    solver_->init();
    isInitialized_ = true;
}

void PySolver::run()
{
    if(!isInitialized_)
        throw IsenException("Solver: not initialized");
    solver_->run();
}

void PySolver::write(Output::ArchiveType archiveType, const char* filename)
{
    if(!isInitialized_)
        throw IsenException("Solver: not initialized");

    if(archiveType != Output::Unknown)
        solver_->getOutput()->setArchiveType(archiveType);
    solver_->write(filename);
}


PyNameList PySolver::getNameList() const
{
    if(!isInitialized_)
        throw IsenException("Solver: not initialized");
    return PyNameList(namelist_);
}

PyOutput PySolver::getOutput() const
{
    if(!isInitialized_)
        throw IsenException("Solver: not initialized");
    PyOutput output;
    output.set(solver_->getOutput());
    return output;
}

ISEN_NAMESPACE_END
