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
#include <Isen/SolverRef.h>
#include <Isen/Terminal.h>
#include <boost/filesystem.hpp>

ISEN_NAMESPACE_BEGIN

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
        LOG() << log::disable;

        Parser parser;
        std::shared_ptr<NameList> namelist;

        CHECK_NOTHROW(namelist = parser.parse(filename));
        std::shared_ptr<Solver> solver(new SolverRef(namelist));

        //-------------------------------------------------
        // Check inital-conditions
        //-------------------------------------------------
        solver->init();

        try
        {
            auto mat = FieldLoader::load((dir / boost::filesystem::path("unow-0.dat")).string());
            CHECK(FieldVerifier::verify("unow", solver->unow(), mat));
        }
        catch(IsenException& e)
        {
            std::cout << Terminal::Color(Terminal::Color::getFileColor()) << "unow";
            std::cout << " : No test data found - Skipping\n";
        }

        //-------------------------------------------------
        // Check evolution
        //-------------------------------------------------
        solver->run();

        LOG() << log::enable;
    }
    else
        std::cout << "MATLAB verification: No test data found -  Skipping" << std::endl;
}

ISEN_NAMESPACE_END
