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

#include "FieldLoader.h"
#include "FieldVerifier.h"
#include "Test.h"
#include <Isen/Common.h>
#include <Isen/Logger.h>
#include <Isen/Parse.h>
#include <Isen/Progressbar.h>
#include <Isen/SolverRef.h>
#include <Isen/Terminal.h>
#include <boost/filesystem.hpp>

ISEN_NAMESPACE_BEGIN

// Check field
#define CHECK_FIELD(field, time)                                                                                       \
    try                                                                                                                \
    {                                                                                                                  \
        Timer t;                                                                                                       \
        bool passed = true;                                                                                            \
        LOG() << "Checking " << #field << "[t=" << #time << "] ... " << log::flush;                                    \
        auto field = FieldLoader::load((dir / boost::filesystem::path(#field "-" #time ".dat")).string());             \
        CHECK((passed = FieldVerifier::verify(#field, solver->field(), std::move((field)))));                          \
        if(passed)                                                                                                     \
        {                                                                                                              \
            LOG_SUCCESS(t);                                                                                            \
        }                                                                                                              \
        else                                                                                                           \
        {                                                                                                              \
            LOG() << log::endl;                                                                                        \
        }                                                                                                              \
    }                                                                                                                  \
    catch(...)                                                                                                         \
    {                                                                                                                  \
        LOG() << "No test data found" << log::failed;                                                                  \
    }

TEST_CASE("MATLAB verification", "[Solver]")
{
    std::string filename;
    boost::filesystem::path dir;

    if(boost::filesystem::exists("data/namelist.m"))
    {
        filename = "data/namelist.m";
        dir = "data";
    }
    // Visual Studio runs from many diffrent directories ...
    else if(boost::filesystem::exists("../data/namelist.m"))
    {
        filename = "../data/namelist.m";
        dir = "../data";
    }

    if(!filename.empty())
    {
        std::cout << Terminal::Color(Terminal::Color::getFileColor()) << "MATLAB verification" << std::endl;
        Progressbar::printBar('-');
        LOG() << log::disable;

        Parser parser;
        std::shared_ptr<NameList> namelist;

        CHECK_NOTHROW(namelist = parser.parse(filename));
        std::shared_ptr<Solver> solver(new SolverRef(namelist));

        //-------------------------------------------------
        // Check inital-conditions
        //-------------------------------------------------
        solver->init();
        LOG() << log::enable;

        CHECK_FIELD(topo, 0);

        CHECK_FIELD(zhtold, 0);
        CHECK_FIELD(zhtnow, 0);

        CHECK_FIELD(uold, 0);
        CHECK_FIELD(unow, 0);
        CHECK_FIELD(unew, 0);

        CHECK_FIELD(sold, 0);
        CHECK_FIELD(snow, 0);
        CHECK_FIELD(snew, 0);

        CHECK_FIELD(mtg, 0);
        CHECK_FIELD(mtgnew, 0);
        CHECK_FIELD(mtg0, 0);

        CHECK_FIELD(exn, 0);
        CHECK_FIELD(exn0, 0);

        CHECK_FIELD(prs, 0);
        CHECK_FIELD(prs0, 0);

        CHECK_FIELD(tau, 0);

        CHECK_FIELD(th0, 0);

        CHECK_FIELD(sbnd1, 0);
        CHECK_FIELD(sbnd2, 0);
        CHECK_FIELD(ubnd1, 0);
        CHECK_FIELD(ubnd2, 0);
        CHECK_FIELD(tbnd1, 0);
        CHECK_FIELD(tbnd2, 0);

        //-------------------------------------------------
        // Check evolution
        //-------------------------------------------------
        LOG() << log::disable;
        solver->run();
        LOG() << log::enable;
    }
    else
        std::cout << "MATLAB verification: No test data found -  Skipping" << std::endl;
}

#undef CHECK_FIELD

ISEN_NAMESPACE_END
