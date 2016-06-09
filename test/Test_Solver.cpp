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
        LOG() << "Checking " << #field << "[t=" << time << "] ... " << logger::flush;                                  \
        auto field = FieldLoader::load(                                                                                \
            (dir / boost::filesystem::path(std::string(#field "-") + time + std::string(".dat"))).string());           \
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

        // Adjust namelist
        namelist->iprtcfl = false;
        namelist->setByName("iout", namelist->nout * 2);

        std::shared_ptr<Solver> solver = SolverFactory::create("ref", namelist);

        //-------------------------------------------------
        // Check inital-conditions
        //-------------------------------------------------
        solver->init();
        LOG() << logger::enable;

        std::string initial("0");

        CHECK_FIELD(topo, initial);

        CHECK_FIELD(zhtold, initial);
        CHECK_FIELD(zhtnow, initial);

        CHECK_FIELD(uold, initial);
        CHECK_FIELD(unow, initial);
        CHECK_FIELD(unew, initial);

        CHECK_FIELD(sold, initial);
        CHECK_FIELD(snow, initial);
        CHECK_FIELD(snew, initial);

        CHECK_FIELD(mtg, initial);
        CHECK_FIELD(mtgnew, initial);
        CHECK_FIELD(mtg0, initial);

        CHECK_FIELD(exn, initial);
        CHECK_FIELD(exn0, initial);

        CHECK_FIELD(prs, initial);
        CHECK_FIELD(prs0, initial);

        CHECK_FIELD(tau, initial);

        CHECK_FIELD(th0, initial);
        
        CHECK_FIELD(qvold, initial);
        CHECK_FIELD(qvnow, initial);
        CHECK_FIELD(qvnew, initial);

        CHECK_FIELD(qcold, initial);
        CHECK_FIELD(qcnow, initial);
        CHECK_FIELD(qcnew, initial);
        
        CHECK_FIELD(qrold, initial);
        CHECK_FIELD(qrnow, initial);
        CHECK_FIELD(qrnew, initial);
        
        CHECK_FIELD(qvbnd1, initial);
        CHECK_FIELD(qvbnd2, initial);
        CHECK_FIELD(qcbnd1, initial);
        CHECK_FIELD(qcbnd2, initial);
        CHECK_FIELD(qrbnd1, initial);
        CHECK_FIELD(qrbnd2, initial);
        CHECK_FIELD(sbnd1, initial);
        CHECK_FIELD(sbnd2, initial);
        CHECK_FIELD(ubnd1, initial);
        CHECK_FIELD(ubnd2, initial);
        CHECK_FIELD(tbnd1, initial);
        CHECK_FIELD(tbnd2, initial);

        //-------------------------------------------------
        // Check evolution
        //-------------------------------------------------
        solver->run();

        std::string nout = std::to_string(namelist->nts);

        CHECK_FIELD(zhtold, nout);
        CHECK_FIELD(zhtnow, nout);

        CHECK_FIELD(uold, nout);
        CHECK_FIELD(unow, nout);

        CHECK_FIELD(sold, nout);
        CHECK_FIELD(snow, nout);

        //CHECK_FIELD(qvold, nout);
        //CHECK_FIELD(qvnow, nout);

        //CHECK_FIELD(qcold, nout);
        //CHECK_FIELD(qcnow, nout);
        //
        //CHECK_FIELD(qrold, nout);
        //CHECK_FIELD(qrnow, nout);

        //CHECK_FIELD(prec, nout);
        //CHECK_FIELD(tot_prec, nout);

        CHECK_FIELD(mtg, nout);

        CHECK_FIELD(exn, nout);
        CHECK_FIELD(prs, nout);

        CHECK_FIELD(tau, nout);
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
