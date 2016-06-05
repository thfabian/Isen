#                        _________ _______   __
#                       /  _/ ___// ____/ | / /
#                       / / \__ \/ __/ /  |/ /
#                     _/ / ___/ / /___/ /|  /
#                    /___//____/_____/_/ |_/
#
#  Isentropic model - ETH Zurich
#  Copyright (C) 2016  Fabian Thuering (thfabian@student.ethz.ch)
#
#  This file is distributed under the MIT Open Source License. See LICENSE.TXT for details.

cmake_minimum_required(VERSION 2.8)

set(CORE_SOURCE
    CommandLine.cpp
    Common.cpp
    Logger.cpp
    NameList.cpp
    Output.cpp
    Parse.cpp
    Progressbar.cpp
    Terminal.cpp
    Solver.cpp
    SolverCpu.cpp
    )

set(CORE_HEADER
    ${ISEN_INCLUDE_DIR}/Isen/Boundary.h
    ${ISEN_INCLUDE_DIR}/Isen/Config.h
    ${ISEN_INCLUDE_DIR}/Isen/CommandLine.h
    ${ISEN_INCLUDE_DIR}/Isen/Common.h
    ${ISEN_INCLUDE_DIR}/Isen/Logger.h
    ${ISEN_INCLUDE_DIR}/Isen/MeteoUtils.h
    ${ISEN_INCLUDE_DIR}/Isen/NameList.h
    ${ISEN_INCLUDE_DIR}/Isen/Output.h
    ${ISEN_INCLUDE_DIR}/Isen/Parse.h
    ${ISEN_INCLUDE_DIR}/Isen/Progressbar.h
    ${ISEN_INCLUDE_DIR}/Isen/Terminal.h
    ${ISEN_INCLUDE_DIR}/Isen/Timer.h
    ${ISEN_INCLUDE_DIR}/Isen/Type.h
    ${ISEN_INCLUDE_DIR}/Isen/Solver.h
    ${ISEN_INCLUDE_DIR}/Isen/SolverCpu.h    
    ${ISEN_INCLUDE_DIR}/Isen/SolverFactory.h
    )

add_library(IsenCore ${CORE_SOURCE} ${CORE_HEADER})
