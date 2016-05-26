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

#include <Isen/CommandLine.h>
#include <Isen/Common.h>
#include <Isen/Logger.h>
#include <Isen/NameList.h>
#include <Isen/Parse.h>
#include <Isen/Progressbar.h>
#include <Isen/SolverRef.h>
#include <Isen/Terminal.h>
#include <Isen/Timer.h>
#include <functional>
#include <iostream>
#include <list>

using namespace Isen;

/// Tokenize comma seperated files
template <class Container>
void tokenizeFiles(Container& files)
{
    Container newFiles;
    Tokenizer tokenizer(",");
    for(const auto& file : files)
    {
        auto tokens = tokenizer.tokenize(file);
        newFiles.insert(newFiles.end(), tokens.begin(), tokens.end());
    }
    using std::swap; // Allow ADL
    swap(files, newFiles);
}

/// Main entry-point
int main(int argc, char* argv[])
{
    auto fatalError = [&](const std::string& str) { error(argv[0], str); };

    // Parse command-line arguments
    CommandLine cl(argc, argv);

    if(cl.has("help"))
        cl.printHelp();

    if(cl.has("version"))
        cl.printVersion();

    Output::ArchiveType archiveType = Output::Text; // default
    if(cl.has("archive"))
    {
        auto archiveStr = cl.as<std::string>("archive");
        if(archiveStr == "text")
            archiveType = Output::Text;
        else if(archiveStr == "xml")
            archiveType = Output::Xml;
        else
            archiveType = Output::Binary;
    }

    Parser::Style parsingStyle = Parser::UNKNOWN; // deduce from file extension
    if(cl.has("parsing-style"))
    {
        auto parsingStr = cl.as<std::string>("parsing-style");
        if(parsingStr == "matlab")
            parsingStyle = Parser::MATLAB;
        else
            parsingStyle = Parser::PYTHON;
    }

    Terminal::Color::disableColor = cl.has("no-color");

    if((Progressbar::disableProgressbar = cl.has("quiet")))
        LOG() << Isen::log::disable;

    if(!cl.has("file"))
        fatalError("no input files");
    auto files = cl.as<std::vector<std::string>>("file");
    tokenizeFiles(files);

    Parser parser;
    std::shared_ptr<NameList> namelist;
    std::shared_ptr<Solver> solver;

    for(const auto& file : files)
    {
        // Parse the input file and create solver
        try
        {
            parser.setStyle(parsingStyle);
            namelist = parser.parse(file);
            if(cl.has("print-namelist"))
                namelist->print();

            solver = std::make_shared<SolverRef>(namelist, archiveType);
        }
        catch(const std::exception& e)
        {
            fatalError(e.what());
        }

        // Run simulation
        solver->init();
        solver->run();

        // Write simulation to outputfile
        try
        {
            if(!cl.has("no-output"))
                solver->write();
        }
        catch(const std::exception& e)
        {
            fatalError(e.what());
        }
    }

    return 0;
}
