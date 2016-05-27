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
    Float dx = xl / nx;
    /// Domain depth [K]
    Float thl = 60.0;
    /// Vertical Resolution
    int nz = 60;
    /// Integration time [s]
    Float time = 6 * 60 * 60;
    /// Time step [s]
    Float dt = 10;
    /// Horizontal diffusion coefficient
    Float diff = 0.02;

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
    Float u00 = 15;
    /// Brunt - Vaisalla frequency [1/s]
    Float bv00 = 0.01;
    /// Potential temperature at surface
    Float th00 = 300;
    /// Wind shear simulation
    bool ishear = false;
    /// Bottom level of wind shear layer (bottom level of wind layer is 0)
    int k_shl = 5;
    /// Top level of wind shear layer (top level of wind layer is nz - 1)
    int k_sht = 8;
    /// Initial velocity below shear layer [m/s]
    Float u00_sh = 10;

    //-------------------------------------------------
    // Boundaries
    //-------------------------------------------------

    /// Number of grid points in absorber
    int nab = 0;
    /// Maximum value of absorber
    Float diffabs = 1.0;
    /// Lateral boundaries (false = periodic)
    bool irelax = false;
    /// Number of boundary points on each side
    int nb = 2;

    //-------------------------------------------------
    // Print options
    //-------------------------------------------------

    /// Print debugging text
    bool idbg = false;
    /// Print CFL number
    bool iprtcfl = true;
    /// Print computation time (unused)
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
    Float vt_mult = 1.0;
    /// Critical cloud water mixing ratio for the onset of autoconversion [kg/kg]
    Float autoconv_th = 0.0001;
    /// Multiplication factor for autoconversion
    Float autoconv_mult = 1.0;
    /// Switch to turn on / off sedimentation
    bool sediment_on = 1;

    //-------------------------------------------------
    // Computed input parameters
    //-------------------------------------------------

    /// Spacing between vertical layers[K]
    Float dth = thl / nz;
    /// Number of iterations
    int nts = static_cast<int>(std::round(time / dt));
    /// Number of output steps
    int nout = nts / iout + iiniout;

    // REMOVE THE FOLLOWING !

    //----------------------

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
    const Float g = 9.81;
    /// Specific heat of air at constant pressure
    const Float cp = 1004.0;
    /// Gas constant of air [J/kgK]
    const Float r = 287.0;
    /// Gas constant of vapor [J/kgK]
    const Float r_v = 461.0;
    /// Short cut for R / Cp
    const Float rdcp = r / cp;
    /// short cut for Cp / R
    const Float cpdr = cp / r;
    /// Reference pressure in SI units(Pa, not hPa!)
    const Float pref = 100 * 1000;
    /// Surface height
    const Float z00 = 0.0;
    /// Upstream surface pressure (= reference pressure)
    const Float prs00 = pref;
    ///
    const Float exn00 = cp * std::pow(prs00 / pref, rdcp);

    //-------------------------------------------------

    /// Print the NameList
    void print() const;

    /// Set a variable by name [potentially slow]
    void setByName(const std::string& name, const int& value);
    void setByName(const std::string& name, const Float& value);
    void setByName(const std::string& name, const bool& value);
    void setByName(const std::string& name, const std::string& value);

    friend class boost::serialization::access;

    /// Serialize the NameList (used by Output)
    template <class Archive>
    void serialize(Archive& ar, const unsigned int /* version */)
    {
        ar & BOOST_SERIALIZATION_NVP(run_name);
        ar & BOOST_SERIALIZATION_NVP(iout);
        ar & BOOST_SERIALIZATION_NVP(iiniout);
        ar & BOOST_SERIALIZATION_NVP(xl);
        ar & BOOST_SERIALIZATION_NVP(nx);
        ar & BOOST_SERIALIZATION_NVP(dx);
        ar & BOOST_SERIALIZATION_NVP(thl);
        ar & BOOST_SERIALIZATION_NVP(nz);
        ar & BOOST_SERIALIZATION_NVP(time);
        ar & BOOST_SERIALIZATION_NVP(dt);
        ar & BOOST_SERIALIZATION_NVP(diff);
        ar & BOOST_SERIALIZATION_NVP(topomx);
        ar & BOOST_SERIALIZATION_NVP(topowd);
        ar & BOOST_SERIALIZATION_NVP(topotim);
        ar & BOOST_SERIALIZATION_NVP(u00);
        ar & BOOST_SERIALIZATION_NVP(bv00);
        ar & BOOST_SERIALIZATION_NVP(th00);
        ar & BOOST_SERIALIZATION_NVP(ishear);
        ar & BOOST_SERIALIZATION_NVP(k_shl);
        ar & BOOST_SERIALIZATION_NVP(k_sht);
        ar & BOOST_SERIALIZATION_NVP(u00_sh);
        ar & BOOST_SERIALIZATION_NVP(nab);
        ar & BOOST_SERIALIZATION_NVP(diffabs);
        ar & BOOST_SERIALIZATION_NVP(irelax);
        ar & BOOST_SERIALIZATION_NVP(nb);
        ar & BOOST_SERIALIZATION_NVP(idbg);
        ar & BOOST_SERIALIZATION_NVP(iprtcfl);
        ar & BOOST_SERIALIZATION_NVP(itime);
        ar & BOOST_SERIALIZATION_NVP(imoist);
        ar & BOOST_SERIALIZATION_NVP(imoist_diff);
        ar & BOOST_SERIALIZATION_NVP(imicrophys);
        ar & BOOST_SERIALIZATION_NVP(idthdt);
        ar & BOOST_SERIALIZATION_NVP(iern);
        ar & BOOST_SERIALIZATION_NVP(vt_mult);
        ar & BOOST_SERIALIZATION_NVP(autoconv_th);
        ar & BOOST_SERIALIZATION_NVP(autoconv_mult);
        ar & BOOST_SERIALIZATION_NVP(sediment_on);
        ar & BOOST_SERIALIZATION_NVP(dth);
        ar & BOOST_SERIALIZATION_NVP(nts);
        ar & BOOST_SERIALIZATION_NVP(nout);
        ar & BOOST_SERIALIZATION_NVP(nx1);
        ar & BOOST_SERIALIZATION_NVP(nz1);
        ar & BOOST_SERIALIZATION_NVP(nxb);
        ar & BOOST_SERIALIZATION_NVP(nxb1);
    }
};

ISEN_NAMESPACE_END

// Current version of NameList
BOOST_CLASS_VERSION(Isen::NameList, 0);

#endif
