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
#include <Isen/Common.h>
#include <Isen/Logger.h>
#include <Isen/Parse.h>
#include <Isen/SolverRef.h>
#include <boost/filesystem.hpp>

ISEN_NAMESPACE_BEGIN

TEST_CASE("MATLAB verification", "[Solver]")
{
    std::string filename, dirname;

    if(boost::filesystem::exists("data/namelist.m"))
    {
        filename = "data/namelist.m";
        dirname = "data";
    }
    // Visual Studio runs from many diffrent directories ...
    else if(boost::filesystem::exists("../data/namelist.m"))
    {
        filename = "../data/namelist.m";
        dirname = "../data";
    }

    if(!filename.empty())
    {
        LOG() << log::disable;

        Parser parser;
        std::shared_ptr<NameList> namelist;

        CHECK_NOTHROW(namelist = parser.parse(filename));
        std::shared_ptr<Solver> solver(new SolverRef(namelist));

        solver->init();
        solver->run();

        LOG() << log::enable;
    }
    else
        std::cout << "MATLAB verification: No test data found -  Skipping" << std::endl;
}

ISEN_NAMESPACE_END
