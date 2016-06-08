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

#include <Isen/Common.h>
#include <boost/filesystem.hpp>
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <thread>

namespace bf = boost::filesystem;

ISEN_NAMESPACE_BEGIN

void ISEN_NORETURN error(std::string program, std::string msg)
{
    program = bf::path(program).filename().string();

    if(msg.back() == '\n')
        msg.back() = ' ';

    std::cerr << program << ": error: " << msg << std::endl;
    std::exit(EXIT_FAILURE);
}

void warning(std::string program, std::string msg)
{
    program = bf::path(program).filename().string();

    if(msg.back() == '\n')
        msg.back() = ' ';

    std::cerr << program << ": warning: " << msg << std::endl;
}

std::string timeString(double time, bool precise)
{
    if(std::isnan(time) || std::isinf(time))
        return "inf";

    std::string suffix = "ms";
    if(time > 1000)
    {
        time /= 1000;
        suffix = "s ";
        if(time > 60)
        {
            time /= 60;
            suffix = "m ";
            if(time > 60)
            {
                time /= 60;
                suffix = "h ";
                if(time > 12)
                {
                    time /= 12;
                    suffix = "d ";
                }
            }
        }
    }

    std::ostringstream os;
    os << std::setprecision(precise ? 4 : 1) << std::fixed << time << " " << suffix;

    return os.str();
}

std::string memString(std::size_t size, bool precise)
{
    double value = static_cast<double>(size);
    const char* suffixes[] = {"B", "KB", "MB", "GB", "TB", "PB"};

    int suffix = 0;
    while(suffix < 5 && value > 1024.0f)
    {
        value /= 1024.0f;
        ++suffix;
    }

    std::ostringstream os;
    os << std::setprecision(suffix == 0 ? 0 : (precise ? 4 : 1)) << std::fixed << value << " " << suffixes[suffix];

    return os.str();
}

extern unsigned int getNumThreads(int nx, int nz)
{
#ifdef ISEN_NUM_THREADS
    unsigned int numThreads = ISEN_NUM_THREADS;
#else
    unsigned int numThreads = std::thread::hardware_concurrency();
    // We assume hyper-threading
    if(numThreads > 2)
        numThreads /= 2;
#endif

    if(numThreads <= 0)
        numThreads = 1;

     // Dual-Core
    if(numThreads <= 2)
        return numThreads;
     // Quad-Core
    else if(numThreads <= 4)
        return numThreads;
    else
        return numThreads;
}


ISEN_NAMESPACE_END
