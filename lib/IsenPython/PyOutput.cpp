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

#include <Isen/Python/PyOutput.h>
#include <new>

ISEN_NAMESPACE_BEGIN

PyNameList PyOutput::getNameList() const
{
    if(!output_)
        throw IsenException("Output: not initialized");
    return PyNameList(namelist_);
}

void PyOutput::set(std::shared_ptr<Output> output) noexcept
{
    output_ = output;
    namelist_ = std::make_shared<NameList>(*output_->getNameList());
}

void PyOutput::read(const char* file)
{
    output_ = std::make_shared<Output>();
    output_->read(std::string(file));
    namelist_ = std::make_shared<NameList>(*output_->getNameList());
}

ISEN_NAMESPACE_END
