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
#ifndef ISEN_SOLVER_H
#define ISEN_SOLVER_H

#include <Isen/Common.h>
#include <Isen/NameList.h>
#include <Isen/Output.h>
#include <Isen/Kessler.h>
#include <map>

ISEN_NAMESPACE_BEGIN

/// @brief Refrence implementation of all Solvers
class Solver
{
public:
    /// @brief Allocate memory 
    ///
    /// @throw IsenException if out of memory
    Solver(const std::shared_ptr<NameList>& namelist, Output::ArchiveType archiveType = Output::ArchiveType::Text);

    /// Free all memory
    virtual ~Solver() {}

    /// @brief Initialize the simulation
    ///
    /// Generates initial conditions for isentropic density (sigma) and velocity (u), initializes the boundaries and
    /// generates the topography.
    virtual void init() noexcept;

    /// Run the simulation
    virtual void run();

    /// @brief Write simulation to output file
    ///
    /// If no filename is provided, NameList::run_name is being used.
    virtual void write(std::string filename = "");

    /// Compute CFL condition
    virtual double computeCFL() const noexcept;

    //------------------------------------------------------------
    // Diffusion
    //------------------------------------------------------------

    /// Horizontal diffusion
    virtual void horizontalDiffusion() noexcept;
    
    //------------------------------------------------------------
    // Geometric height 
    //------------------------------------------------------------
    
    /// Calculate geometric height 
    virtual void geometricHeight() noexcept;
    
    //------------------------------------------------------------
    // Boundary
    //------------------------------------------------------------

    /// Exchange boundaries for periodicity of prognostic fields
    virtual void applyPeriodicBoundary() noexcept;

    /// Relaxation of prognostic fields
    virtual void applyRelaxationBoundary() noexcept;

    /// Clip negative values of moisture variables
    virtual void clipMoisture() noexcept;

    //------------------------------------------------------------
    // Diagnostic
    //------------------------------------------------------------

    /// Diagnostic computation of Montgomery
    virtual void diagMontgomery() noexcept;

    /// Diagnostic computation of pressure
    virtual void diagPressure() noexcept;

    //------------------------------------------------------------
    // Prognostic
    //------------------------------------------------------------

    /// Prognostic step for isentropic mass density
    virtual void progIsendens() noexcept;

    /// Prognostic step for momentum
    virtual void progVelocity() noexcept;

    /// Prognostic step for hydrometeors
    virtual void progMoisture() noexcept;

    /// Prognostic step for number densities
    virtual void progNumdens() noexcept;

    //------------------------------------------------------------
    // Getter
    //------------------------------------------------------------

    /// Access the output
    std::shared_ptr<Output> getOutput() const { return output_; }

    /// Get matrix by @name
    const MatrixXf& getMat(std::string name) const;

    /// Get vector by @name
    const VectorXf& getVec(std::string name) const;
    
    /// Get matrix or vector by @name and return an Eigen::Map of the data 
    Eigen::Map<MatrixXf> getField(std::string name) const;

protected:
    std::shared_ptr<NameList> namelist_;
    std::shared_ptr<Output> output_;

    std::map<std::string, MatrixXf*> matMap_;
    std::map<std::string, VectorXf*> vecMap_;

    //-------------------------------------------------
    // Parametrizations
    //-------------------------------------------------
    std::shared_ptr<Kessler> kessler_;

    //-------------------------------------------------
    // Define physical fields
    //-------------------------------------------------

    /// Topography
    VectorXf topo_;

    /// Height in z-coordinates
    MatrixXf zhtold_;
    MatrixXf zhtnow_;

    /// Horizontal velocity
    MatrixXf uold_;
    MatrixXf unow_;
    MatrixXf unew_;

    /// Isentropic density
    MatrixXf sold_;
    MatrixXf snow_;
    MatrixXf snew_;

    /// Montgomery potential
    MatrixXf mtg_;
    MatrixXf mtgnew_;
    VectorXf mtg0_;

    /// Exner function
    MatrixXf exn_;
    VectorXf exn0_;

    /// Pressure
    MatrixXf prs_;
    VectorXf prs0_;

    /// Height-dependent diffusion coefficient
    VectorXf tau_;

    /// Upstream profile for theta 
    VectorXf th0_;

    /// Precipitation
    VectorXf prec_;

    /// Accumulated precipitation
    VectorXf tot_prec_;

    /// Water vapor
    MatrixXf qvold_;
    MatrixXf qvnow_;
    MatrixXf qvnew_;

    /// Specific cloud water content
    MatrixXf qcold_;
    MatrixXf qcnow_;
    MatrixXf qcnew_;

    /// Specific rain water content
    MatrixXf qrold_;
    MatrixXf qrnow_;
    MatrixXf qrnew_;

    /// Temperature
    MatrixXf temp_;

    /// Rain-droplet number density
    MatrixXf nrold_;
    MatrixXf nrnow_;
    MatrixXf nrnew_;

    /// Cloud-droplet number density
    MatrixXf ncold_;
    MatrixXf ncnow_;
    MatrixXf ncnew_;

    /// Latent heating
    MatrixXf dthetadt_;

    //-------------------------------------------------
    // Define fields at lateral boundaries
    //  1 denotes the leftern boundary
    //  2 denotes the rightern boundary
    //-------------------------------------------------

    /// Topography boundaries  
    VectorXf tbnd1_;
    VectorXf tbnd2_;

    /// Isentropic density boundaries
    VectorXf sbnd1_;
    VectorXf sbnd2_;

    /// Horizontal velocity boundaries
    VectorXf ubnd1_;
    VectorXf ubnd2_;

    /// Specific humidity boundaries
    VectorXf qvbnd1_;
    VectorXf qvbnd2_;

    /// Specific cloud water content boundaries
    VectorXf qcbnd1_;
    VectorXf qcbnd2_;

    /// Specific rain water content boundaries
    VectorXf qrbnd1_;
    VectorXf qrbnd2_;

    /// Latent heating boundaries
    VectorXf dthetadtbnd1_;
    VectorXf dthetadtbnd2_;

    /// Rain-droplet number density boundaries
    VectorXf nrbnd1_;
    VectorXf nrbnd2_;

    /// Cloud-droplet number density boundaries
    VectorXf ncbnd1_;
    VectorXf ncbnd2_;

    //-------------------------------------------------
    // Define scalar fields
    //-------------------------------------------------
    double dtdx_;
    double topofact_;

    /// Be verbose?
    bool verbose_;
};

/// This is a convenience macro to declare local aliases of the NameList class inside any Solver method
#define SOLVER_DECLARE_ALL_ALIASES ISEN_NAMELIST_DECLARE_ALIAS(namelist_)

ISEN_NAMESPACE_END

#endif
