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
#ifndef ISEN_NAMELIST_H
#define ISEN_NAMELIST_H

#include <Isen/Common.h>
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/version.hpp>
#include <string>

ISEN_NAMESPACE_BEGIN

/// @brief This scruct holds all the variables used to control the flow of the simulation.
///
/// The Parser class can read Python or MATLAB Namelist file. All variables have a default initialization.
struct NameList
{
    /// Default constructor
    NameList() = default;

    /// Copy constructor
    NameList(const NameList& other) = default;

    /// Copy assignment
    NameList& operator=(const NameList& other) = default;

    //-------------------------------------------------
    // Output control
    //-------------------------------------------------

    /// Simulation name
    std::string run_name = "DownSlope";
    /// Write every iout-th time-step into the output file
    int iout = 360;
    /// Write initial field
    bool iiniout = true;

    //-------------------------------------------------
    // Domain size
    //-------------------------------------------------

    /// Domain size [m]
    int xl = 500000;
    /// Horizontal resolution
    int nx = 100;
    /// Horizontal spacing
    double dx = xl / nx;
    /// Domain depth [K]
    double thl = 60.0;
    /// Vertical Resolution
    int nz = 60;
    /// Integration time [s]
    double time = 6 * 60 * 60;
    /// Time step [s]
    double dt = 10;
    /// Horizontal diffusion coefficient
    double diff = 0.02;

    //-------------------------------------------------
    // Topography
    //-------------------------------------------------

    /// Mountain height [m]
    int topomx = 500;
    /// Mountain half width [m]
    int topowd = 50000;
    /// Mountain growth time [s]
    int topotim = 1800;

    //-------------------------------------------------
    // Initial atmosphere
    //-------------------------------------------------

    /// Initial velocity [m/s]
    double u00 = 15;
    /// Brunt - Vaisalla frequency [1/s]
    double bv00 = 0.01;
    /// Potential temperature at surface
    double th00 = 300;
    /// Wind shear simulation
    bool ishear = false;
    /// Bottom level of wind shear layer (bottom level of wind layer is 0)
    int k_shl = 5;
    /// Top level of wind shear layer (top level of wind layer is nz - 1)
    int k_sht = 8;
    /// Initial velocity below shear layer [m/s]
    double u00_sh = 10;

    //-------------------------------------------------
    // Boundaries
    //-------------------------------------------------

    /// Number of grid points in absorber
    int nab = 0;
    /// Maximum value of absorber
    double diffabs = 1.0;
    /// Lateral boundaries (false = periodic)
    bool irelax = false;
    /// Number of boundary points on each side
    int nb = 2;

    //-------------------------------------------------
    // Print options
    //-------------------------------------------------

    /// Print debugging text (unused)
    bool idbg = false;
    /// Print CFL number
    bool iprtcfl = true;
    /// Print computation time
    bool itime = true;

    //-------------------------------------------------
    // Physics: Moisture
    //-------------------------------------------------

    /// Include moisture
    bool imoist = false;
    /// Apply diffusion to qv, qc, qr
    bool imoist_diff = false;
    /// Include microphysics(0 = off, 1 = kessler, 2 = two moment)
    int imicrophys = 0;
    /// Couple physics to dynamics
    bool idthdt = false;
    /// Evaporation of rain droplets
    bool iern = false;

    //-------------------------------------------------
    // Options for Kessler scheme
    //-------------------------------------------------

    /// Multiplication factor for terminal fall velocity
    double vt_mult = 1.0;
    /// Critical cloud water mixing ratio for the onset of autoconversion [kg/kg]
    double autoconv_th = 0.0001;
    /// Multiplication factor for autoconversion
    double autoconv_mult = 1.0;
    /// Switch to turn on / off sedimentation
    bool sediment_on = true;

    //-------------------------------------------------
    // Computed input parameters
    //-------------------------------------------------

    /// Spacing between vertical layers[K]
    double dth = thl / nz;
    /// Number of iterations
    int nts = static_cast<int>(std::round(time / dt));
    /// Number of output steps
    int nout = nts / iout + iiniout;
    /// Number of staggered gridpoints in x
    int nx1 = nx + 1;
    /// Number of staggered gridpoints in z
    int nz1 = nz + 1;
    /// x range of unstaggered variable
    int nxb = nx + 2 * nb;
    /// x range of staggered variable
    int nxb1 = nx1 + 2 * nb;

    //----------------------

    /// Update computed variables
    void update();

    //-------------------------------------------------
    // Physical constants
    //-------------------------------------------------

    /// Gravity
    const double g = 9.81;
    /// Specific heat of air at constant pressure
    const double cp = 1004.0;
    /// Gas constant of air [J/kgK]
    const double r = 287.0;
    /// Gas constant of vapor [J/kgK]
    const double r_v = 461.0;
    /// Short cut for R / Cp
    const double rdcp = r / cp;
    /// short cut for Cp / R
    const double cpdr = cp / r;
    /// Reference pressure in SI units(Pa, not hPa!)
    const double pref = 100 * 1000;
    /// Surface height
    const double z00 = 0.0;
    /// Upstream surface pressure (= reference pressure)
    const double prs00 = pref;
    ///
    const double exn00 = cp * std::pow(prs00 / pref, rdcp);

    //-------------------------------------------------

    /// Stream NameList to @out
    void print(std::stringstream& out) const;

    /// Stream the NameList to @out with colored output
    void print(std::ostream& out, bool color = true) const;

    /// Set a variable by name [potentially slow]
    void setByName(const std::string& name, const int& value);
    void setByName(const std::string& name, const double& value);
    void setByName(const std::string& name, const bool& value);
    void setByName(const std::string& name, const std::string& value);

    friend class boost::serialization::access;

    /// Serialize the NameList (used by Output)
    template <class Archive>
    void serialize(Archive& ar, const unsigned int /* version */)
    {
        ar& BOOST_SERIALIZATION_NVP(run_name);
        ar& BOOST_SERIALIZATION_NVP(iout);
        ar& BOOST_SERIALIZATION_NVP(iiniout);
        ar& BOOST_SERIALIZATION_NVP(xl);
        ar& BOOST_SERIALIZATION_NVP(nx);
        ar& BOOST_SERIALIZATION_NVP(dx);
        ar& BOOST_SERIALIZATION_NVP(thl);
        ar& BOOST_SERIALIZATION_NVP(nz);
        ar& BOOST_SERIALIZATION_NVP(time);
        ar& BOOST_SERIALIZATION_NVP(dt);
        ar& BOOST_SERIALIZATION_NVP(diff);
        ar& BOOST_SERIALIZATION_NVP(topomx);
        ar& BOOST_SERIALIZATION_NVP(topowd);
        ar& BOOST_SERIALIZATION_NVP(topotim);
        ar& BOOST_SERIALIZATION_NVP(u00);
        ar& BOOST_SERIALIZATION_NVP(bv00);
        ar& BOOST_SERIALIZATION_NVP(th00);
        ar& BOOST_SERIALIZATION_NVP(ishear);
        ar& BOOST_SERIALIZATION_NVP(k_shl);
        ar& BOOST_SERIALIZATION_NVP(k_sht);
        ar& BOOST_SERIALIZATION_NVP(u00_sh);
        ar& BOOST_SERIALIZATION_NVP(nab);
        ar& BOOST_SERIALIZATION_NVP(diffabs);
        ar& BOOST_SERIALIZATION_NVP(irelax);
        ar& BOOST_SERIALIZATION_NVP(nb);
        ar& BOOST_SERIALIZATION_NVP(idbg);
        ar& BOOST_SERIALIZATION_NVP(iprtcfl);
        ar& BOOST_SERIALIZATION_NVP(itime);
        ar& BOOST_SERIALIZATION_NVP(imoist);
        ar& BOOST_SERIALIZATION_NVP(imoist_diff);
        ar& BOOST_SERIALIZATION_NVP(imicrophys);
        ar& BOOST_SERIALIZATION_NVP(idthdt);
        ar& BOOST_SERIALIZATION_NVP(iern);
        ar& BOOST_SERIALIZATION_NVP(vt_mult);
        ar& BOOST_SERIALIZATION_NVP(autoconv_th);
        ar& BOOST_SERIALIZATION_NVP(autoconv_mult);
        ar& BOOST_SERIALIZATION_NVP(sediment_on);
        ar& BOOST_SERIALIZATION_NVP(dth);
        ar& BOOST_SERIALIZATION_NVP(nts);
        ar& BOOST_SERIALIZATION_NVP(nout);
        ar& BOOST_SERIALIZATION_NVP(nx1);
        ar& BOOST_SERIALIZATION_NVP(nz1);
        ar& BOOST_SERIALIZATION_NVP(nxb);
        ar& BOOST_SERIALIZATION_NVP(nxb1);
    }
};

ISEN_NAMESPACE_END

// Current version of NameList
BOOST_CLASS_VERSION(Isen::NameList, 0);

/// This is a convenience macro to declare local aliases of the NameList class
#define ISEN_NAMELIST_DECLARE_ALIAS(namelist)                                                                          \
    const auto run_name ISEN_UNUSED = namelist->run_name;                                                              \
    (void) run_name;                                                                                                   \
    const auto iout ISEN_UNUSED = namelist->iout;                                                                      \
    (void) iout;                                                                                                       \
    const auto iiniout ISEN_UNUSED = namelist->iiniout;                                                                \
    (void) iiniout;                                                                                                    \
    const auto xl ISEN_UNUSED = namelist->xl;                                                                          \
    (void) xl;                                                                                                         \
    const auto nx ISEN_UNUSED = namelist->nx;                                                                          \
    (void) nx;                                                                                                         \
    const auto dx ISEN_UNUSED = namelist->dx;                                                                          \
    (void) dx;                                                                                                         \
    const auto thl ISEN_UNUSED = namelist->thl;                                                                        \
    (void) thl;                                                                                                        \
    const auto nz ISEN_UNUSED = namelist->nz;                                                                          \
    (void) nz;                                                                                                         \
    const auto time ISEN_UNUSED = namelist->time;                                                                      \
    (void) time;                                                                                                       \
    const auto dt ISEN_UNUSED = namelist->dt;                                                                          \
    (void) dt;                                                                                                         \
    const auto diff ISEN_UNUSED = namelist->diff;                                                                      \
    (void) diff;                                                                                                       \
    const auto topomx ISEN_UNUSED = namelist->topomx;                                                                  \
    (void) topomx;                                                                                                     \
    const auto topowd ISEN_UNUSED = namelist->topowd;                                                                  \
    (void) topowd;                                                                                                     \
    const auto topotim ISEN_UNUSED = namelist->topotim;                                                                \
    (void) topotim;                                                                                                    \
    const auto u00 ISEN_UNUSED = namelist->u00;                                                                        \
    (void) u00;                                                                                                        \
    const auto bv00 ISEN_UNUSED = namelist->bv00;                                                                      \
    (void) bv00;                                                                                                       \
    const auto th00 ISEN_UNUSED = namelist->th00;                                                                      \
    (void) th00;                                                                                                       \
    const auto ishear ISEN_UNUSED = namelist->ishear;                                                                  \
    (void) ishear;                                                                                                     \
    const auto k_shl ISEN_UNUSED = namelist->k_shl;                                                                    \
    (void) k_shl;                                                                                                      \
    const auto k_sht ISEN_UNUSED = namelist->k_sht;                                                                    \
    (void) k_sht;                                                                                                      \
    const auto u00_sh ISEN_UNUSED = namelist->u00_sh;                                                                  \
    (void) u00_sh;                                                                                                     \
    const auto nab ISEN_UNUSED = namelist->nab;                                                                        \
    (void) nab;                                                                                                        \
    const auto diffabs ISEN_UNUSED = namelist->diffabs;                                                                \
    (void) diffabs;                                                                                                    \
    const auto irelax ISEN_UNUSED = namelist->irelax;                                                                  \
    (void) irelax;                                                                                                     \
    const auto nb ISEN_UNUSED = namelist->nb;                                                                          \
    (void) nb;                                                                                                         \
    const auto idbg ISEN_UNUSED = namelist->idbg;                                                                      \
    (void) idbg;                                                                                                       \
    const auto iprtcfl ISEN_UNUSED = namelist->iprtcfl;                                                                \
    (void) iprtcfl;                                                                                                    \
    const auto itime ISEN_UNUSED = namelist->itime;                                                                    \
    (void) itime;                                                                                                      \
    const auto imoist ISEN_UNUSED = namelist->imoist;                                                                  \
    (void) imoist;                                                                                                     \
    const auto imoist_diff ISEN_UNUSED = namelist->imoist_diff;                                                        \
    (void) imoist_diff;                                                                                                \
    const auto imicrophys ISEN_UNUSED = namelist->imicrophys;                                                          \
    (void) imicrophys;                                                                                                 \
    const auto idthdt ISEN_UNUSED = namelist->idthdt;                                                                  \
    (void) idthdt;                                                                                                     \
    const auto iern ISEN_UNUSED = namelist->iern;                                                                      \
    (void) iern;                                                                                                       \
    const auto vt_mult ISEN_UNUSED = namelist->vt_mult;                                                                \
    (void) vt_mult;                                                                                                    \
    const auto autoconv_th ISEN_UNUSED = namelist->autoconv_th;                                                        \
    (void) autoconv_th;                                                                                                \
    const auto autoconv_mult ISEN_UNUSED = namelist->autoconv_mult;                                                    \
    (void) autoconv_mult;                                                                                              \
    const auto sediment_on ISEN_UNUSED = namelist->sediment_on;                                                        \
    (void) sediment_on;                                                                                                \
    const auto dth ISEN_UNUSED = namelist->dth;                                                                        \
    (void) dth;                                                                                                        \
    const auto nts ISEN_UNUSED = namelist->nts;                                                                        \
    (void) nts;                                                                                                        \
    const auto nout ISEN_UNUSED = namelist->nout;                                                                      \
    (void) nout;                                                                                                       \
    const auto nx1 ISEN_UNUSED = namelist->nx1;                                                                        \
    (void) nx1;                                                                                                        \
    const auto nz1 ISEN_UNUSED = namelist->nz1;                                                                        \
    (void) nz1;                                                                                                        \
    const auto nxb ISEN_UNUSED = namelist->nxb;                                                                        \
    (void) nxb;                                                                                                        \
    const auto nxb1 ISEN_UNUSED = namelist->nxb1;                                                                      \
    (void) nxb1;                                                                                                       \
    const auto g ISEN_UNUSED = namelist->g;                                                                            \
    (void) g;                                                                                                          \
    const auto cp ISEN_UNUSED = namelist->cp;                                                                          \
    (void) cp;                                                                                                         \
    const auto r ISEN_UNUSED = namelist->r;                                                                            \
    (void) r;                                                                                                          \
    const auto r_v ISEN_UNUSED = namelist->r_v;                                                                        \
    (void) r_v;                                                                                                        \
    const auto rdcp ISEN_UNUSED = namelist->rdcp;                                                                      \
    (void) rdcp;                                                                                                       \
    const auto cpdr ISEN_UNUSED = namelist->cpdr;                                                                      \
    (void) cpdr;                                                                                                       \
    const auto pref ISEN_UNUSED = namelist->pref;                                                                      \
    (void) pref;                                                                                                       \
    const auto z00 ISEN_UNUSED = namelist->z00;                                                                        \
    (void) z00;                                                                                                        \
    const auto prs00 ISEN_UNUSED = namelist->prs00;                                                                    \
    (void) prs00;                                                                                                      \
    const auto exn00 ISEN_UNUSED = namelist->exn00;                                                                    \
    (void) exn00;

#endif
