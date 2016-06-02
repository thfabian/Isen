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
#include <map>

ISEN_NAMESPACE_BEGIN

/// @brief Refrence implementation of all Solvers
class Solver
{
public:
    /// @brief Allocate memory 
    ///
    /// @throw IsenException if out of memory
    Solver(std::shared_ptr<NameList> namelist, Output::ArchiveType archiveType = Output::ArchiveType::Text);

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
    // Boundary
    //------------------------------------------------------------

    /// Exchange boundaries for periodicity of prognostic fields
    virtual void applyPeriodicBoundary() noexcept;

    /// Relaxation of prognostic fields
    virtual void applyRelaxationBoundary() noexcept;

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
    const MatrixXf& getMat(const std::string& name) const;

    /// Get vector by @name
    const VectorXf& getVec(const std::string& name) const;

protected:
    std::shared_ptr<NameList> namelist_;
    std::shared_ptr<Output> output_;

    std::map<std::string, const MatrixXf&> matMap_;
    std::map<std::string, const VectorXf&> vecMap_;

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

    /// Specific humidity
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

    double tbnd1_;
    double tbnd2_;

    /// Isentropic density
    VectorXf sbnd1_;
    VectorXf sbnd2_;

    /// Horizontal velocity
    VectorXf ubnd1_;
    VectorXf ubnd2_;

    /// Specific humidity
    VectorXf qvbnd1_;
    VectorXf qvbnd2_;

    /// Specific cloud water content
    VectorXf qcbnd1_;
    VectorXf qcbnd2_;

    /// Specific rain water content
    VectorXf qrbnd1_;
    VectorXf qrbnd2_;

    /// Latent heating
    VectorXf dthetadtbnd1_;
    VectorXf dthetadtbnd2_;

    /// Rain-droplet number density
    VectorXf nrbnd1_;
    VectorXf nrbnd2_;

    /// Cloud-droplet number density
    VectorXf ncbnd1_;
    VectorXf ncbnd2_;

    //-------------------------------------------------
    // Define scalar fields
    //-------------------------------------------------
    double dtdx_;
    double topofact_;

    /// Be verbose?
    bool verbose_;

public:
    auto topo() const -> const decltype(topo_) & { return topo_; }
    auto topo() -> decltype(topo_) & { return topo_; }

    auto zhtold() const -> const decltype(zhtold_) & { return zhtold_; }
    auto zhtold() -> decltype(zhtold_) & { return zhtold_; }

    auto zhtnow() const -> const decltype(zhtnow_) & { return zhtnow_; }
    auto zhtnow() -> decltype(zhtnow_) & { return zhtnow_; }

    auto uold() const -> const decltype(uold_) & { return uold_; }
    auto uold() -> decltype(uold_) & { return uold_; }

    auto unow() const -> const decltype(unow_) & { return unow_; }
    auto unow() -> decltype(unow_) & { return unow_; }

    auto unew() const -> const decltype(unew_) & { return unew_; }
    auto unew() -> decltype(unew_) & { return unew_; }

    auto sold() const -> const decltype(sold_) & { return sold_; }
    auto sold() -> decltype(sold_) & { return sold_; }

    auto snow() const -> const decltype(snow_) & { return snow_; }
    auto snow() -> decltype(snow_) & { return snow_; }

    auto snew() const -> const decltype(snew_) & { return snew_; }
    auto snew() -> decltype(snew_) & { return snew_; }

    auto mtg() const -> const decltype(mtg_) & { return mtg_; }
    auto mtg() -> decltype(mtg_) & { return mtg_; }

    auto mtgnew() const -> const decltype(mtgnew_) & { return mtgnew_; }
    auto mtgnew() -> decltype(mtgnew_) & { return mtgnew_; }

    auto mtg0() const -> const decltype(mtg0_) & { return mtg0_; }
    auto mtg0() -> decltype(mtg0_) & { return mtg0_; }

    auto exn() const -> const decltype(exn_) & { return exn_; }
    auto exn() -> decltype(exn_) & { return exn_; }

    auto exn0() const -> const decltype(exn0_) & { return exn0_; }
    auto exn0() -> decltype(exn0_) & { return exn0_; }

    auto prs() const -> const decltype(prs_) & { return prs_; }
    auto prs() -> decltype(prs_) & { return prs_; }

    auto prs0() const -> const decltype(prs0_) & { return prs0_; }
    auto prs0() -> decltype(prs0_) & { return prs0_; }

    auto tau() const -> const decltype(tau_) & { return tau_; }
    auto tau() -> decltype(tau_) & { return tau_; }

    auto th0() const -> const decltype(th0_) & { return th0_; }
    auto th0() -> decltype(th0_) & { return th0_; }

    auto prec() const -> const decltype(prec_) & { return prec_; }
    auto prec() -> decltype(prec_) & { return prec_; }

    auto tot_prec() const -> const decltype(tot_prec_) & { return tot_prec_; }
    auto tot_prec() -> decltype(tot_prec_) & { return tot_prec_; }

    auto qvold() const -> const decltype(qvold_) & { return qvold_; }
    auto qvold() -> decltype(qvold_) & { return qvold_; }

    auto qvnow() const -> const decltype(qvnow_) & { return qvnow_; }
    auto qvnow() -> decltype(qvnow_) & { return qvnow_; }

    auto qvnew() const -> const decltype(qvnew_) & { return qvnew_; }
    auto qvnew() -> decltype(qvnew_) & { return qvnew_; }

    auto qcold() const -> const decltype(qcold_) & { return qcold_; }
    auto qcold() -> decltype(qcold_) & { return qcold_; }

    auto qcnow() const -> const decltype(qcnow_) & { return qcnow_; }
    auto qcnow() -> decltype(qcnow_) & { return qcnow_; }

    auto qcnew() const -> const decltype(qcnew_) & { return qcnew_; }
    auto qcnew() -> decltype(qcnew_) & { return qcnew_; }

    auto qrold() const -> const decltype(qrold_) & { return qrold_; }
    auto qrold() -> decltype(qrold_) & { return qrold_; }

    auto qrnow() const -> const decltype(qrnow_) & { return qrnow_; }
    auto qrnow() -> decltype(qrnow_) & { return qrnow_; }

    auto qrnew() const -> const decltype(qrnew_) & { return qrnew_; }
    auto qrnew() -> decltype(qrnew_) & { return qrnew_; }

    auto nrold() const -> const decltype(nrold_) & { return nrold_; }
    auto nrold() -> decltype(nrold_) & { return nrold_; }

    auto nrnow() const -> const decltype(nrnow_) & { return nrnow_; }
    auto nrnow() -> decltype(nrnow_) & { return nrnow_; }

    auto nrnew() const -> const decltype(nrnew_) & { return nrnew_; }
    auto nrnew() -> decltype(nrnew_) & { return nrnew_; }

    auto ncold() const -> const decltype(ncold_) & { return ncold_; }
    auto ncold() -> decltype(ncold_) & { return ncold_; }

    auto ncnow() const -> const decltype(ncnow_) & { return ncnow_; }
    auto ncnow() -> decltype(ncnow_) & { return ncnow_; }

    auto ncnew() const -> const decltype(ncnew_) & { return ncnew_; }
    auto ncnew() -> decltype(ncnew_) & { return ncnew_; }

    auto dthetadt() const -> const decltype(dthetadt_) & { return dthetadt_; }
    auto dthetadt() -> decltype(dthetadt_) & { return dthetadt_; }

    auto sbnd1() const -> const decltype(sbnd1_) & { return sbnd1_; }
    auto sbnd1() -> decltype(sbnd1_) & { return sbnd1_; }

    auto sbnd2() const -> const decltype(sbnd2_) & { return sbnd2_; }
    auto sbnd2() -> decltype(sbnd2_) & { return sbnd2_; }

    auto ubnd1() const -> const decltype(ubnd1_) & { return ubnd1_; }
    auto ubnd1() -> decltype(ubnd1_) & { return ubnd1_; }

    auto ubnd2() const -> const decltype(ubnd2_) & { return ubnd2_; }
    auto ubnd2() -> decltype(ubnd2_) & { return ubnd2_; }

    auto qvbnd1() const -> const decltype(qvbnd1_) & { return qvbnd1_; }
    auto qvbnd1() -> decltype(qvbnd1_) & { return qvbnd1_; }

    auto qvbnd2() const -> const decltype(qvbnd2_) & { return qvbnd2_; }
    auto qvbnd2() -> decltype(qvbnd2_) & { return qvbnd2_; }

    auto qcbnd1() const -> const decltype(qcbnd1_) & { return qcbnd1_; }
    auto qcbnd1() -> decltype(qcbnd1_) & { return qcbnd1_; }

    auto qcbnd2() const -> const decltype(qcbnd2_) & { return qcbnd2_; }
    auto qcbnd2() -> decltype(qcbnd2_) & { return qcbnd2_; }

    auto qrbnd1() const -> const decltype(qrbnd1_) & { return qrbnd1_; }
    auto qrbnd1() -> decltype(qrbnd1_) & { return qrbnd1_; }

    auto qrbnd2() const -> const decltype(qrbnd2_) & { return qrbnd2_; }
    auto qrbnd2() -> decltype(qrbnd2_) & { return qrbnd2_; }

    auto dthetadtbnd1() const -> const decltype(dthetadtbnd1_) & { return dthetadtbnd1_; }
    auto dthetadtbnd1() -> decltype(dthetadtbnd1_) & { return dthetadtbnd1_; }

    auto dthetadtbnd2() const -> const decltype(dthetadtbnd2_) & { return dthetadtbnd2_; }
    auto dthetadtbnd2() -> decltype(dthetadtbnd2_) & { return dthetadtbnd2_; }

    auto nrbnd1() const -> const decltype(nrbnd1_) & { return nrbnd1_; }
    auto nrbnd1() -> decltype(nrbnd1_) & { return nrbnd1_; }

    auto nrbnd2() const -> const decltype(nrbnd2_) & { return nrbnd2_; }
    auto nrbnd2() -> decltype(nrbnd2_) & { return nrbnd2_; }

    auto ncbnd1() const -> const decltype(ncbnd1_) & { return ncbnd1_; }
    auto ncbnd1() -> decltype(ncbnd1_) & { return ncbnd1_; }

    auto ncbnd2() const -> const decltype(ncbnd2_) & { return ncbnd2_; }
    auto ncbnd2() -> decltype(ncbnd2_) & { return ncbnd2_; }

    VectorXf tbnd1() const { VectorXf tbnd1(1); tbnd1(0) = tbnd1_; return tbnd1; }
    VectorXf tbnd2() const { VectorXf tbnd2(1); tbnd2(0) = tbnd2_; return tbnd2; }
};

/// This is a convenience macro to declare local aliases of the NameList class inside any Solver method
#define SOLVER_DECLARE_ALL_ALIASES                                                                                     \
    const auto run_name ISEN_UNUSED = namelist_->run_name;                                                             \
    (void) run_name;                                                                                                   \
    const auto iout ISEN_UNUSED = namelist_->iout;                                                                     \
    (void) iout;                                                                                                       \
    const auto iiniout ISEN_UNUSED = namelist_->iiniout;                                                               \
    (void) iiniout;                                                                                                    \
    const auto xl ISEN_UNUSED = namelist_->xl;                                                                         \
    (void) xl;                                                                                                         \
    const auto nx ISEN_UNUSED = namelist_->nx;                                                                         \
    (void) nx;                                                                                                         \
    const auto dx ISEN_UNUSED = namelist_->dx;                                                                         \
    (void) dx;                                                                                                         \
    const auto thl ISEN_UNUSED = namelist_->thl;                                                                       \
    (void) thl;                                                                                                        \
    const auto nz ISEN_UNUSED = namelist_->nz;                                                                         \
    (void) nz;                                                                                                         \
    const auto time ISEN_UNUSED = namelist_->time;                                                                     \
    (void) time;                                                                                                       \
    const auto dt ISEN_UNUSED = namelist_->dt;                                                                         \
    (void) dt;                                                                                                         \
    const auto diff ISEN_UNUSED = namelist_->diff;                                                                     \
    (void) diff;                                                                                                       \
    const auto topomx ISEN_UNUSED = namelist_->topomx;                                                                 \
    (void) topomx;                                                                                                     \
    const auto topowd ISEN_UNUSED = namelist_->topowd;                                                                 \
    (void) topowd;                                                                                                     \
    const auto topotim ISEN_UNUSED = namelist_->topotim;                                                               \
    (void) topotim;                                                                                                    \
    const auto u00 ISEN_UNUSED = namelist_->u00;                                                                       \
    (void) u00;                                                                                                        \
    const auto bv00 ISEN_UNUSED = namelist_->bv00;                                                                     \
    (void) bv00;                                                                                                       \
    const auto th00 ISEN_UNUSED = namelist_->th00;                                                                     \
    (void) th00;                                                                                                       \
    const auto ishear ISEN_UNUSED = namelist_->ishear;                                                                 \
    (void) ishear;                                                                                                     \
    const auto k_shl ISEN_UNUSED = namelist_->k_shl;                                                                   \
    (void) k_shl;                                                                                                      \
    const auto k_sht ISEN_UNUSED = namelist_->k_sht;                                                                   \
    (void) k_sht;                                                                                                      \
    const auto u00_sh ISEN_UNUSED = namelist_->u00_sh;                                                                 \
    (void) u00_sh;                                                                                                     \
    const auto nab ISEN_UNUSED = namelist_->nab;                                                                       \
    (void) nab;                                                                                                        \
    const auto diffabs ISEN_UNUSED = namelist_->diffabs;                                                               \
    (void) diffabs;                                                                                                    \
    const auto irelax ISEN_UNUSED = namelist_->irelax;                                                                 \
    (void) irelax;                                                                                                     \
    const auto nb ISEN_UNUSED = namelist_->nb;                                                                         \
    (void) nb;                                                                                                         \
    const auto idbg ISEN_UNUSED = namelist_->idbg;                                                                     \
    (void) idbg;                                                                                                       \
    const auto iprtcfl ISEN_UNUSED = namelist_->iprtcfl;                                                               \
    (void) iprtcfl;                                                                                                    \
    const auto itime ISEN_UNUSED = namelist_->itime;                                                                   \
    (void) itime;                                                                                                      \
    const auto imoist ISEN_UNUSED = namelist_->imoist;                                                                 \
    (void) imoist;                                                                                                     \
    const auto imoist_diff ISEN_UNUSED = namelist_->imoist_diff;                                                       \
    (void) imoist_diff;                                                                                                \
    const auto imicrophys ISEN_UNUSED = namelist_->imicrophys;                                                         \
    (void) imicrophys;                                                                                                 \
    const auto idthdt ISEN_UNUSED = namelist_->idthdt;                                                                 \
    (void) idthdt;                                                                                                     \
    const auto iern ISEN_UNUSED = namelist_->iern;                                                                     \
    (void) iern;                                                                                                       \
    const auto vt_mult ISEN_UNUSED = namelist_->vt_mult;                                                               \
    (void) vt_mult;                                                                                                    \
    const auto autoconv_th ISEN_UNUSED = namelist_->autoconv_th;                                                       \
    (void) autoconv_th;                                                                                                \
    const auto autoconv_mult ISEN_UNUSED = namelist_->autoconv_mult;                                                   \
    (void) autoconv_mult;                                                                                              \
    const auto sediment_on ISEN_UNUSED = namelist_->sediment_on;                                                       \
    (void) sediment_on;                                                                                                \
    const auto dth ISEN_UNUSED = namelist_->dth;                                                                       \
    (void) dth;                                                                                                        \
    const auto nts ISEN_UNUSED = namelist_->nts;                                                                       \
    (void) nts;                                                                                                        \
    const auto nout ISEN_UNUSED = namelist_->nout;                                                                     \
    (void) nout;                                                                                                       \
    const auto nx1 ISEN_UNUSED = namelist_->nx1;                                                                       \
    (void) nx1;                                                                                                        \
    const auto nz1 ISEN_UNUSED = namelist_->nz1;                                                                       \
    (void) nz1;                                                                                                        \
    const auto nxb ISEN_UNUSED = namelist_->nxb;                                                                       \
    (void) nxb;                                                                                                        \
    const auto nxb1 ISEN_UNUSED = namelist_->nxb1;                                                                     \
    (void) nxb1;                                                                                                       \
    const auto g ISEN_UNUSED = namelist_->g;                                                                           \
    (void) g;                                                                                                          \
    const auto cp ISEN_UNUSED = namelist_->cp;                                                                         \
    (void) cp;                                                                                                         \
    const auto r ISEN_UNUSED = namelist_->r;                                                                           \
    (void) r;                                                                                                          \
    const auto r_v ISEN_UNUSED = namelist_->r_v;                                                                       \
    (void) r_v;                                                                                                        \
    const auto rdcp ISEN_UNUSED = namelist_->rdcp;                                                                     \
    (void) rdcp;                                                                                                       \
    const auto cpdr ISEN_UNUSED = namelist_->cpdr;                                                                     \
    (void) cpdr;                                                                                                       \
    const auto pref ISEN_UNUSED = namelist_->pref;                                                                     \
    (void) pref;                                                                                                       \
    const auto z00 ISEN_UNUSED = namelist_->z00;                                                                       \
    (void) z00;                                                                                                        \
    const auto prs00 ISEN_UNUSED = namelist_->prs00;                                                                   \
    (void) prs00;                                                                                                      \
    const auto exn00 ISEN_UNUSED = namelist_->exn00;                                                                   \
    (void) exn00;

ISEN_NAMESPACE_END

#endif
