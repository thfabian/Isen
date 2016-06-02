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

#include <Isen/Python/PyNameList.h>
#include <Isen/Parse.h>
#include <sstream>
#include <iostream>

ISEN_NAMESPACE_BEGIN

PyNameList::PyNameList(const char* file)
{
    std::string fileS(file);
    if(fileS.empty())
        namelist_ = std::make_shared<NameList>();
    else
    {
        Parser parser;
        namelist_ = parser.parse(fileS);
    }
}

PyNameList::PyNameList(std::shared_ptr<NameList> namelist)
    : namelist_(namelist)
{}


std::string PyNameList::__str__(const PyNameList& self) noexcept
{
    std::stringstream strStream;
    self.getNameList()->print(strStream);
    return strStream.str();
}


ISEN_NAMESPACE_END
