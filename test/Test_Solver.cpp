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

// Check field by loading the refrence field from disk
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

    Progressbar::disableProgressbar = false;            
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
        namelist->iprtcfl = false;
        
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
        
        CHECK_FIELD(qvold, 0);
        CHECK_FIELD(qvnow, 0);
        CHECK_FIELD(qvnew, 0);
        
        CHECK_FIELD(qcold, 0);
        CHECK_FIELD(qcnow, 0);
        CHECK_FIELD(qcnew, 0);
        
        CHECK_FIELD(qrold, 0);
        CHECK_FIELD(qrnow, 0);
        CHECK_FIELD(qrnew, 0);
        
        CHECK_FIELD(qvbnd1, 0);
        CHECK_FIELD(qvbnd2, 0);
        CHECK_FIELD(qcbnd1, 0);
        CHECK_FIELD(qcbnd2, 0);
        CHECK_FIELD(qrbnd1, 0);
        CHECK_FIELD(qrbnd2, 0);
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

// Check field by comparing to refrence implemention
#define CHECK_FIELD_IMPL(field, solverTest)                                                                            \
    do                                                                                                                 \
    {                                                                                                                  \
        t.start();                                                                                                     \
        bool passed = true;                                                                                            \
        LOG() << "Checking " << #field << " ... " << logger::flush;                                                    \
        CHECK((passed = FieldVerifier::verify(#field, solverRef->getField(#field), solverTest->getField(#field))));    \
        if(passed)                                                                                                     \
        {                                                                                                              \
            LOG_SUCCESS(t);                                                                                            \
        }                                                                                                              \
        else                                                                                                           \
        {                                                                                                              \
            LOG() << logger::failed;                                                                                   \
        }                                                                                                              \
    } while(0)

#define CHECK_FIELD_CPU(field) CHECK_FIELD_IMPL(field, solverOpt)

TEST_CASE("Cross verification (SolverCpu)", "[Solver]")
{    
    Timer t;         
    LOG() << logger::disable;
    
    Progressbar::disableProgressbar = false;        
    Progressbar::printBar('-');
    std::cout << Terminal::Color(Terminal::Color::getFileColor()) << "SolverCpu verification";
    std::cout << " with Solver" << std::endl;
    Progressbar::printBar('-');
    
    auto namelist = std::make_shared<NameList>();
    namelist->setByName("time", 100.0); // 10 timesteps
    namelist->setByName("iprtcfl", false);    
    
    std::shared_ptr<Solver> solverRef = SolverFactory::create("ref", namelist);
    std::shared_ptr<Solver> solverOpt = SolverFactory::create("cpu", namelist);
    
    solverRef->init();
    solverOpt->init();
 
    solverRef->run();
    solverOpt->run();    
    LOG() << logger::enable;    

    CHECK_FIELD_CPU(zhtold);
    CHECK_FIELD_CPU(zhtnow);

    CHECK_FIELD_CPU(uold);
    CHECK_FIELD_CPU(unow);

    CHECK_FIELD_CPU(sold);
    CHECK_FIELD_CPU(snow);

    CHECK_FIELD_CPU(mtg);

    CHECK_FIELD_CPU(exn);
    CHECK_FIELD_CPU(prs);

    CHECK_FIELD_CPU(tau);
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
