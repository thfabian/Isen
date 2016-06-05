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
#include <Isen/SolverFactory.h>
#include <Isen/Terminal.h>
#include <boost/filesystem.hpp>

ISEN_NAMESPACE_BEGIN

// Check field
#define CHECK_FIELD(field, time)                                                                                       \
    try                                                                                                                \
    {                                                                                                                  \
        Timer t;                                                                                                       \
        bool passed = true;                                                                                            \
        LOG() << "Checking " << #field << "[t=" << #time << "] ... " << logger::flush;                                 \
        auto field = FieldLoader::load((dir / boost::filesystem::path(#field "-" #time ".dat")).string());             \
        CHECK((passed = FieldVerifier::verify(#field, MatrixXf(solver->getField(#field)), std::move((field)))));       \
        if(passed)                                                                                                     \
        {                                                                                                              \
            LOG_SUCCESS(t);                                                                                            \
        }                                                                                                              \
        else                                                                                                           \
        {                                                                                                              \
            LOG() << logger::endl;                                                                                     \
        }                                                                                                              \
    }                                                                                                                  \
    catch(...)                                                                                                         \
    {                                                                                                                  \
        LOG() << "No test data found" << logger::failed;                                                               \
    }

TEST_CASE("MATLAB verification (Solver)", "[Solver]")
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

    Progressbar::printBar('-');
    if(!filename.empty())
    {
        std::cout << Terminal::Color(Terminal::Color::getFileColor()) << "Solver verification";
        std::cout << " with MATLAB" << std::endl;
        Progressbar::printBar('-');

        LOG() << logger::disable;

        Parser parser;
        std::shared_ptr<NameList> namelist;

        CHECK_NOTHROW(namelist = parser.parse(filename));
        std::shared_ptr<Solver> solver = SolverFactory::create("ref", namelist);

        //-------------------------------------------------
        // Check inital-conditions
        //-------------------------------------------------
        solver->init();
        LOG() << logger::enable;

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
        LOG() << logger::disable;
        solver->run();
        LOG() << logger::enable;

        CHECK_FIELD(zhtold, 1);
        CHECK_FIELD(zhtnow, 1);

        CHECK_FIELD(uold, 1);
        CHECK_FIELD(unow, 1);

        CHECK_FIELD(sold, 1);
        CHECK_FIELD(snow, 1);

        CHECK_FIELD(mtg, 1);

        CHECK_FIELD(exn, 1);
        CHECK_FIELD(prs, 1);

        CHECK_FIELD(tau, 1);
    }
    else
    {
        std::cout << Terminal::Color(Terminal::Color::getFileColor()) << "Solver verification";
        std::cout << " with MATLAB: No test data found -  Skipping" << std::endl;
        Progressbar::printBar('-');
    }
}

#undef CHECK_FIELD

TEST_CASE("Cross validation (SolverOpt)", "[Solver]")
{

}

TEST_CASE("Getter", "[Solver]")
{
    LOG() << logger::disable;
    std::shared_ptr<Solver> solver = SolverFactory::create("ref");
    solver->init();

    SECTION("Matrix success")
    {
        CHECK_NOTHROW(const auto& mat = solver->getMat("uold"));
    }

    SECTION("Matrix fail")
    {
        CHECK_THROWS_AS(const auto& mat = solver->getMat("uoldXXX"), IsenException);
    }

    SECTION("Vector success")
    {
        CHECK_NOTHROW(const auto& vec = solver->getVec("topo"));
    }

    SECTION("Vector fail")
    {
        CHECK_THROWS_AS(const auto& vec = solver->getVec("topoXXX"), IsenException);
    }
    
    SECTION("Field success")
    {
        CHECK_NOTHROW(bool res = (MatrixXf(solver->getField("topo")) == solver->getVec("topo")));
        CHECK_NOTHROW(bool res = (MatrixXf(solver->getField("uold")) == solver->getMat("uold")));
    }

    SECTION("Field fail")
    {
        CHECK_THROWS_AS(const auto fieldVec = solver->getField("topoXXX"), IsenException);
        CHECK_THROWS_AS(const auto fieldMat = solver->getField("uoldXXX"), IsenException);
    }

    LOG() << logger::enable;
}


ISEN_NAMESPACE_END
