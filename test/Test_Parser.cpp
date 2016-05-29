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

#include "Test.h"
#include "FileProxy.h"
#include <Isen/Logger.h>
#include <Isen/Parse.h>

#define ISEN_TEST_PRETTY_PRINT_ERRORS 0

ISEN_NAMESPACE_BEGIN

TEST_CASE("Regex and Conversion", "[Parse]")
{
    LOG() << logger::disable;
    Parser p;

    auto checkIdentifier = [&]() {
        // Valid
        CHECK(p.isIdentifier("id"));
        CHECK(p.isIdentifier("Id"));
        CHECK(p.isIdentifier("_id"));
        CHECK(p.isIdentifier("id_"));
        CHECK(p.isIdentifier("_id_"));
        CHECK(p.isIdentifier("_id2"));
        CHECK(p.isIdentifier("id2_"));
        CHECK(p.isIdentifier("_id2_"));
        CHECK(p.isIdentifier("_"));
        CHECK(p.isIdentifier("_2"));

        // Invalid
        CHECK_FALSE(p.isIdentifier("2"));
        CHECK_FALSE(p.isIdentifier("2_"));
        CHECK_FALSE(p.isIdentifier(" id"));
        CHECK_FALSE(p.isIdentifier(" id "));
    };

    auto checkEmptyOrSpaceOnly = [&]() {
        // Valid
        CHECK(p.isEmptyOrSpaceOnly(""));
        CHECK(p.isEmptyOrSpaceOnly(" "));
        CHECK(p.isEmptyOrSpaceOnly("\t"));

        // Invalid
        CHECK_FALSE(p.isEmptyOrSpaceOnly("2"));
        CHECK_FALSE(p.isEmptyOrSpaceOnly(" 2"));
        CHECK_FALSE(p.isEmptyOrSpaceOnly("2 "));
        CHECK_FALSE(p.isEmptyOrSpaceOnly(" 2 "));
        CHECK_FALSE(p.isEmptyOrSpaceOnly("a"));
        CHECK_FALSE(p.isEmptyOrSpaceOnly(" a"));
        CHECK_FALSE(p.isEmptyOrSpaceOnly("a "));
        CHECK_FALSE(p.isEmptyOrSpaceOnly(" a "));
    };

    auto checkExtractString = [&]() {
        // Valid
        CHECK(p.extractString("'a'") == "a");
        CHECK(p.extractString("'a '") == "a ");
        CHECK(p.extractString("' a'") == " a");

        // Invalid
        CHECK_THROWS(p.extractString("a"));
        CHECK_THROWS(p.extractString("a'"));
        CHECK_THROWS(p.extractString("'a"));
    };

    auto checkStringToBoolean = [&]() {
        // Valid
        CHECK(p.stringToBoolean("1") == true);
        CHECK(p.stringToBoolean("True") == true);
        CHECK(p.stringToBoolean("true") == true);
        CHECK(p.stringToBoolean("0") == false);
        CHECK(p.stringToBoolean("False") == false);
        CHECK(p.stringToBoolean("false") == false);

        // Invalid
        CHECK_THROWS(p.stringToBoolean("2"));
        CHECK_THROWS(p.stringToBoolean("a"));
        CHECK_THROWS(p.stringToBoolean("0.0"));
    };

    auto checkOperator = [&]() {
        // Valid
        CHECK(p.isOperator("+"));
        CHECK(p.isOperator("-"));
        CHECK(p.isOperator("*"));
        CHECK(p.isOperator("/"));

        // Invalid
        CHECK_FALSE(p.isOperator(" "));
        CHECK_FALSE(p.isOperator("a"));
        CHECK_FALSE(p.isOperator("0.0"));
    };

    auto checkNumber = [&]() {
        // Valid
        CHECK(p.isNumber("1"));
        CHECK(p.isNumber("-1"));
        CHECK(p.isNumber("1.0"));
        CHECK(p.isNumber("-1.0"));
        CHECK(p.isNumber("10"));
        CHECK(p.isNumber("10.55"));

        // Invalid
        CHECK_FALSE(p.isNumber(" "));
        CHECK_FALSE(p.isNumber("a"));
        CHECK_FALSE(p.isNumber("."));
    };

    SECTION("MATLAB")
    {
        ProxyFile f(ProxyFile::PYTHON, {"id = 1"});
        p.init(f.getFilename());
        checkIdentifier();
        checkEmptyOrSpaceOnly();
        checkExtractString();
        checkStringToBoolean();
        checkOperator();
        checkNumber();
    };

    SECTION("PYTHON")
    {
        ProxyFile f(ProxyFile::MATLAB, {"id = 1"});
        p.init(f.getFilename());
        checkIdentifier();
        checkEmptyOrSpaceOnly();
        checkExtractString();
        checkStringToBoolean();
        checkOperator();
        checkNumber();
    };
}

TEST_CASE("Parser - Preprocessing", "[Parse]")
{
    Parser p(ISEN_TEST_PRETTY_PRINT_ERRORS);

    SECTION("Wrong position assigment")
    {
        ProxyFile f(ProxyFile::PYTHON, {"a 1"});
        CHECK_NOTHROW(auto res = p.parse(f.getFilename()));
    }

    SECTION("Wrong position assigment")
    {
        ProxyFile f(ProxyFile::PYTHON, {"a b = 1"});
        CHECK_THROWS(auto res = p.parse(f.getFilename()));
    }

    SECTION("Empty left-hand side")
    {
        ProxyFile f(ProxyFile::PYTHON, {"= 1"});
        CHECK_THROWS(auto res = p.parse(f.getFilename()));
    }

    SECTION("Empty left-hand side - spaces")
    {
        ProxyFile f(ProxyFile::PYTHON, {" = 1"});
        CHECK_THROWS(auto res = p.parse(f.getFilename()));
    }

    SECTION("Empty left-hand side - multiple spaces")
    {
        ProxyFile f(ProxyFile::PYTHON, {"         = 1"});
        CHECK_THROWS(auto res = p.parse(f.getFilename()));
    }

    SECTION("Empty right-hand side")
    {
        ProxyFile f(ProxyFile::PYTHON, {"iout="});
        CHECK_THROWS(auto res = p.parse(f.getFilename()));
    }

    SECTION("Empty right-hand side - spaces")
    {
        ProxyFile f(ProxyFile::PYTHON, {"iout= "});
        CHECK_THROWS(auto res = p.parse(f.getFilename()));
    }

    SECTION("Empty right-hand side - multiple spaces")
    {
        ProxyFile f(ProxyFile::PYTHON, {"iout=         "});
        CHECK_THROWS(auto res = p.parse(f.getFilename()));
    }
}


TEST_CASE("Parser - Simple assignment", "[Parse]")
{
    Parser p(ISEN_TEST_PRETTY_PRINT_ERRORS);
    std::shared_ptr<NameList> res;

    SECTION("Typo")
    {
        ProxyFile f(ProxyFile::PYTHON, {"iouts = 5"});
        CHECK_THROWS(res = p.parse(f.getFilename()));
    }

    SECTION("Integer - positve")
    {
        ProxyFile f(ProxyFile::PYTHON, {"iout = 5"});
        CHECK_NOTHROW(res = p.parse(f.getFilename()));
        CHECK(res->iout == 5);
    }

    SECTION("Integer - negative")
    {
        ProxyFile f(ProxyFile::PYTHON, {"iout = -5"});
        CHECK_NOTHROW(res = p.parse(f.getFilename()));
        CHECK(res->iout == -5);
    }

    SECTION("Float - positive")
    {
        ProxyFile f(ProxyFile::PYTHON, {"thl = 150.0"});
        CHECK_NOTHROW(res = p.parse(f.getFilename()));
        CHECK(res->thl == 150.0);
    }

    SECTION("Float - negative")
    {
        ProxyFile f(ProxyFile::PYTHON, {"thl = -150.0"});
        CHECK_NOTHROW(res = p.parse(f.getFilename()));
        CHECK(res->thl == -150.0);
    }

    SECTION("Boolean")
    {
        ProxyFile f(ProxyFile::PYTHON, {"iiniout = 0"});
        CHECK_NOTHROW(res = p.parse(f.getFilename()));
        CHECK(res->iiniout == false);
    }

    SECTION("String")
    {
        ProxyFile f(ProxyFile::PYTHON, {"run_name = 'test'"});
        CHECK_NOTHROW(res = p.parse(f.getFilename()));
        CHECK(res->run_name == "test");
    }

    SECTION("Ignored")
    {
        Float dth = NameList().dth;
        ProxyFile f(ProxyFile::PYTHON, {"dth = 0.0"});
        CHECK_NOTHROW(res = p.parse(f.getFilename()));
        CHECK(res->dth == dth);
    }
}

TEST_CASE("Parser - Simple expression", "[Parse]")
{
    Parser p(ISEN_TEST_PRETTY_PRINT_ERRORS);
    std::shared_ptr<NameList> res;

    SECTION("Invalid")
    {
        ProxyFile f(ProxyFile::PYTHON, {"iout = * 5"});
        CHECK_THROWS(res = p.parse(f.getFilename()));
    }

    SECTION("Boolean - not supported")
    {
        ProxyFile f(ProxyFile::PYTHON, {"iiniout = 1 * 1"});
        CHECK_THROWS(res = p.parse(f.getFilename()));
    }

    SECTION("String - not supported")
    {
        ProxyFile f(ProxyFile::PYTHON, {"run_name = 'test' + 'test'"});
        CHECK_THROWS(res = p.parse(f.getFilename()));
    }

    SECTION("Integer - invalid")
    {
        ProxyFile f(ProxyFile::PYTHON, {"iout = 5 * "});
        CHECK_THROWS(res = p.parse(f.getFilename()));
    }

    SECTION("Integer - numerical")
    {
        ProxyFile f(ProxyFile::PYTHON, {"iout = 5 * 5"});
        CHECK_NOTHROW(res = p.parse(f.getFilename()));
        CHECK(res->iout == 25);
    }

    SECTION("Float - numerical")
    {
        ProxyFile f(ProxyFile::PYTHON, {"thl = 2.5 * 2.0"});
        CHECK_NOTHROW(res = p.parse(f.getFilename()));
        CHECK(res->thl == 5.0);
    }

    SECTION("Integer - refrence undefined")
    {
        ProxyFile f(ProxyFile::PYTHON, {"xl = 10", "nx = 5 * nxx"});
        CHECK_THROWS(res = p.parse(f.getFilename()));
    }

    SECTION("Integer - refrence/numerical (+)")
    {
        ProxyFile f(ProxyFile::PYTHON, {"xl = 10", "nx = 5 + xl"});
        CHECK_NOTHROW(res = p.parse(f.getFilename()));
        CHECK(res->xl == 10);
        CHECK(res->nx == 15);
    }

    SECTION("Integer - refrence/numerical (-)")
    {
        ProxyFile f(ProxyFile::PYTHON, {"xl = 10", "nx = 5 - xl"});
        CHECK_NOTHROW(res = p.parse(f.getFilename()));
        CHECK(res->xl == 10);
        CHECK(res->nx == -5);
    }

    SECTION("Integer - refrence/numerical (*)")
    {
        ProxyFile f(ProxyFile::PYTHON, {"xl = 10", "nx = 5 * xl"});
        CHECK_NOTHROW(res = p.parse(f.getFilename()));
        CHECK(res->xl == 10);
        CHECK(res->nx == 50);
    }

    SECTION("Integer - refrence/numerical (/)")
    {
        ProxyFile f(ProxyFile::PYTHON, {"xl = 2", "nx = 10 / xl"});
        CHECK_NOTHROW(res = p.parse(f.getFilename()));
        CHECK(res->xl == 2);
        CHECK(res->nx == 5);
    }

    SECTION("Float - refrence/numerical (+)")
    {
        ProxyFile f(ProxyFile::PYTHON, {"dt = 0.5", "time = 10.0 + dt"});
        CHECK_NOTHROW(res = p.parse(f.getFilename()));
        CHECK(res->dt == 0.5);
        CHECK(res->time == 10.5);
    }

    SECTION("Float - refrence/numerical (-)")
    {
        ProxyFile f(ProxyFile::PYTHON, {"dt = 0.5", "time = 10.0 - dt"});
        CHECK_NOTHROW(res = p.parse(f.getFilename()));
        CHECK(res->dt == 0.5);
        CHECK(res->time == 9.5);
    }

    SECTION("Float - refrence/numerical (*)")
    {
        ProxyFile f(ProxyFile::PYTHON, {"dt = 0.5", "time = 10.0 * dt"});
        CHECK_NOTHROW(res = p.parse(f.getFilename()));
        CHECK(res->dt == 0.5);
        CHECK(res->time == 5.0);
    }

    SECTION("Float - refrence/numerical (/)")
    {
        ProxyFile f(ProxyFile::PYTHON, {"dt = 0.5", "time = 10.0 / dt"});
        CHECK_NOTHROW(res = p.parse(f.getFilename()));
        CHECK(res->dt == 0.5);
        CHECK(res->time == 20.0);
    }

    SECTION("Integer - unary minus (+)")
    {
        ProxyFile f(ProxyFile::PYTHON, {"xl = 10", "nx = -5 + xl"});
        CHECK_NOTHROW(res = p.parse(f.getFilename()));
        CHECK(res->xl == 10);
        CHECK(res->nx == 5);
    }

    SECTION("Float - unary minus (/)")
    {
        ProxyFile f(ProxyFile::PYTHON, {"dt = 0.5", "time = -10.0 / dt"});
        CHECK_NOTHROW(res = p.parse(f.getFilename()));
        CHECK(res->dt == 0.5);
        CHECK(res->time == -20.0);
    }

    SECTION("Integer/Float - mixed types")
    {
        ProxyFile f(ProxyFile::PYTHON, {"xl = 2", "dt = 2.5", "time = 10.0 * xl", "nx = 10 * dt"});
        CHECK_NOTHROW(res = p.parse(f.getFilename()));
        CHECK(res->time == 20.0);
        CHECK(res->nx == 25);
    }

    SECTION("Integer/Boolean/Float - mixed types")
    {
        ProxyFile f(ProxyFile::PYTHON,
                    {"xl = 2", "dt = 2.5", "iiniout = 1", "time = iiniout * xl", "nx = iiniout * dt"});
        CHECK_NOTHROW(res = p.parse(f.getFilename()));
        CHECK(res->time == 2.0);
        CHECK(res->nx == 2);
    }
}

TEST_CASE("Parser - Nested expression", "[Parse]")
{
    Parser p(ISEN_TEST_PRETTY_PRINT_ERRORS);
    std::shared_ptr<NameList> res;

    SECTION("Integer - numerical (*)")
    {
        ProxyFile f(ProxyFile::PYTHON, {"iout = 3 * 3 * 3"});
        CHECK_NOTHROW(res = p.parse(f.getFilename()));
        CHECK(res->iout == 27);
    }

    SECTION("Integer - numerical (+)")
    {
        ProxyFile f(ProxyFile::PYTHON, {"iout = 3 + 3 + 3"});
        CHECK_NOTHROW(res = p.parse(f.getFilename()));
        CHECK(res->iout == 9);
    }

    SECTION("Integer - numerical (+) II")
    {
        ProxyFile f(ProxyFile::PYTHON, {"iout = 3 + 3 + 3 + 3 + 3"});
        CHECK_NOTHROW(res = p.parse(f.getFilename()));
        CHECK(res->iout == 15);
    }

    SECTION("Integer - numerical (mixed)")
    {
        ProxyFile f(ProxyFile::PYTHON, {"iout = 3 + 3 * 3"});
        CHECK_NOTHROW(res = p.parse(f.getFilename()));
        CHECK(res->iout == 12);
    }

    SECTION("Integer - unary (+)")
    {
        ProxyFile f(ProxyFile::PYTHON, {"iout = -3 + 3 + 3"});
        CHECK_NOTHROW(res = p.parse(f.getFilename()));
        CHECK(res->iout == 3);
    }

    SECTION("Integer - unary (mixed)")
    {
        ProxyFile f(ProxyFile::PYTHON, {"iout = - 3 + 3 * 3"});
        CHECK_NOTHROW(res = p.parse(f.getFilename()));
        CHECK(res->iout == 6);
    }

    SECTION("Float - numerical (*)")
    {
        ProxyFile f(ProxyFile::PYTHON, {"thl = 2.5 * 2.0 * 2.0"});
        CHECK_NOTHROW(res = p.parse(f.getFilename()));
        CHECK(res->thl == 10.0);
    }

    SECTION("Float - numerical (+)")
    {
        ProxyFile f(ProxyFile::PYTHON, {"thl = 2.5 + 2.0 + 2.0"});
        CHECK_NOTHROW(res = p.parse(f.getFilename()));
        CHECK(res->thl == 6.5);
    }

    SECTION("Float - numerical (mixed)")
    {
        ProxyFile f(ProxyFile::PYTHON, {"thl = 2.5 + 3.0 * 2.0"});
        CHECK_NOTHROW(res = p.parse(f.getFilename()));
        CHECK(res->thl == 8.5);
    }

    SECTION("Float - numerical unary (*)")
    {
        ProxyFile f(ProxyFile::PYTHON, {"thl = - 2.0 * 2.0 * 2.0"});
        CHECK_NOTHROW(res = p.parse(f.getFilename()));
        CHECK(res->thl == -8.0);
    }
}

TEST_CASE("Parse - Regression", "[Parse]")
{
    Parser p(ISEN_TEST_PRETTY_PRINT_ERRORS);
    std::shared_ptr<NameList> res;

    ProxyFile f(
        ProxyFile::PYTHON,
        {
            "# -*- coding: utf-8 -*-", "\"\"\"", 
            "File defining the global variables used in the main program and all subfunctions.", "\"\"\"", 
            "", 
            "# --------------------------------------------------------",
            "# --------------------- USER NAMELIST --------------------",
            "# --------------------------------------------------------", 
            "", 
            "# Output control",
            "#-------------------------------------------------", 
            "run_name    = 'Downslope'       # simulation name",
            "iout        = 360               # write every iout-th time-step "
            "into the output file",
            "iiniout     = 1                 # write initial field (0 = no, 1 = yes)",
            "", 
            "# Domain size", 
            "#-------------------------------------------------",
            "xl      = 500000.               # domain size  [m]",
            "nx      = 100                   # horizontal resolution",
            "dx      = xl/nx                 # ",
            "thl     = 100.                  # domain depth  [K]",
            "nz      = 100                   # vertical resolution",
            "time    = 6*60*60               # integration time [s]", 
            "dt      = 6                     # time step [s]",
            "diff    = 0.2                   # (horizontal) diffusion coefficient",
            "", 
            "# Topography", 
            "#-------------------------------------------------",
            "topomx  = 1400                  # mountain height [m]",
            "topowd  = 25000                 # mountain half width [m]",
            "topotim = 1800                  # mountain growth time [s]", 
            "", 
            "# Initial atmosphere",
            "#-------------------------------------------------",
            "u00     = 15.                   # initial velocity [m/s]",
            "bv00    = 0.01                  # Brunt-Vaisalla frequency [1/s]",
            "th00    = 280.                  # potential temperature at surface",
            "ishear  = 0                     # wind shear simulation (0 = no shear, 1 = shear)",
            "k_shl   = 32                    # bottom level of wind shear layer (ishear = 1)",
            "                                # bottom level of wind layer is 0 (index)",
            "k_sht   = 45                    # top level of wind shear layer (ishear = 1)",
            "                                # top level of wind layer is nz-1 (index)",
            "u00_sh  = 10.                   # initial velocity below shear layer [m/s] (ishear = 1)",
            "                                # u00 is speed above shear layer [m/s].",
            "", 
            "# Boundaries", 
            "#-------------------------------------------------",
            "nab     = 30                    # number of grid points in absorber",
            "diffabs = 1.                    # maximum value of absorber",
            "irelax  = 1                     # lateral boundaries (0 = periodic, 1 = relax)",
            "nb      = 2                     # number of boundary points on each side",
            "", 
            "# Print options", 
            "#-------------------------------------------------",
            "idbg    = 0                     # print debugging text (0 = not print, 1 = print)",
            "iprtcfl = 1                     # print Courant number (0 = not print, 1 = print)",
            "itime   = 1                     # print computation time (0 = not print, 1 = print)",
            "", 
            "# Physics: Moisture", 
            "#-------------------------------------------------",
            "imoist          = 0             # include moisture (0 = dry, 1 = moist)",
            "imoist_diff     = 0             # apply diffusion to qv, qc, qr (0 = off, 1 = on)",
            "imicrophys      = 0             # include microphysics (0 = off, 1 = kessler, 2 = two moment)",
            "idthdt          = 0             # couple physics to dynamics (0 = off, 1 = on)",
            "iern            = 0             # evaporation of rain droplets (0 = off, 1 = on)",
            "", 
            "# Options for Kessler scheme", 
            "#-------------------------------------------------",
            "vt_mult         = 1.            # multiplication factor for termianl fall velocity",
            "autoconv_th     = 0.0001        # critical cloud water mixing ratio for the onset",
            "                                # of autoconversion [kg/kg]",
            "autoconv_mult   = 1.            # multiplication factor for autoconversion",
            "sediment_on     = 1             # switch to turn on / off sedimentation",
            "", 
            "# Physical constants", 
            "#-------------------------------------------------",
            "g       = 9.81                  # gravity",
            "cp      = 1004.                 # specific heat of air at constant pressure",
            "r       = 287.                  # gas constant of air [J/kgK]",
            "r_v     = 461.                  # gas constant of vapor [J/kgK]",
            "rdcp    = r/cp                  # short cut for R/Cp",
            "cpdr    = cp/r                  # short cut for Cp/R",
            "pref    = 100*1000.             # reference pressure in SI units (Pa, not hPa!)",
            "z00     = 0.                    # surface height",
            "prs00   = pref                  # upstream surface pressure (= ref. pressure)",
            "exn00   = cp*(prs00/pref)**rdcp #", "", "# compute input parameters", 
            "#-------------------------------------------------",
            "dth     = thl/nz                # spacing between vertical layers [K]",
            "nts     = round(time/dt,0)      # number of iterations",
            "nout    = nts/iout              # number of output steps",
            "nx1     = nx + 1                # number of staggered gridpoints in x",
            "nz1     = nz + 1                # number of staggered gridpoints in z",
            "nxb     = nx + 2*nb             # x range of unstaggered variable",
            "nxb1    = nx1 + 2*nb            # x range of staggered variable",
            "",
            "# END OF NAMELIST.PY",
        });
    CHECK_NOTHROW(res = p.parse(f.getFilename()));
    LOG() << logger::enable;
}

ISEN_NAMESPACE_END
