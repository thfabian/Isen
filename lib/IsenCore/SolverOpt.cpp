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

#include <Isen/SolverOpt.h>

ISEN_NAMESPACE_BEGIN

SolverOpt::SolverOpt(std::shared_ptr<NameList> namelist, Output::ArchiveType archiveType)
    : Base(namelist, archiveType)
{}

ISEN_NAMESPACE_END
