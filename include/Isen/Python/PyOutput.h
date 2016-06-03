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

#pragma once
#ifndef ISEN_PYTHON_OUTPUT_H
#define ISEN_PYTHON_OUTPUT_H

#include <Isen/Common.h>
#include <Isen/Output.h>
#include <Isen/Python/PyType.h>
#include <Isen/Python/PyNameList.h>

ISEN_NAMESPACE_BEGIN

/// @brief Expose the Output interface to Python
class PyOutput
{
public:
    /// Default constructor
    PyOutput() = default;

    /// Copy constructor
    PyOutput(const PyOutput& other) = default;

    /// Copy assignment
    PyOutput& operator=(const PyOutput& other) = default;

    /// Get the NameList (ReadOnly)
    PyNameList getNameList() const;

    /// Set the Output
    void set(std::shared_ptr<Output> output) noexcept;

    /// Read Output from file 
    void read(const char* file);

private:
    std::shared_ptr<NameList> namelist_;
    std::shared_ptr<Output> output_;

public:
    /// Height in z-coordinates
    boost::python::object z() const
    {
        if(!output_)
            throw IsenException("Output: not initialized");
        return internal::toNumpyArrayImpl(output_->z().data(), namelist_->nx, namelist_->nz1, namelist_->nout);
    }

    ///// Horizontal velocity
    //const std::vector<double>& u() const { return outputData_.u; }

    ///// Isentropic density
    //const std::vector<double>& s() const { return outputData_.s; }

    ///// Time vector
    //const std::vector<double>& t() const { return outputData_.t; }

    ///// Accumulated precipitation
    //const std::vector<double>& prec() const  { return outputData_.prec; }

    ///// Specific humidity
    //const std::vector<double>& tot_prec() const { return outputData_.tot_prec; }

    ///// Specific cloud water content
    //const std::vector<double>& qv() const { return outputData_.qv; }

    ///// Specific rain water content
    //const std::vector<double>& qc() const { return outputData_.qc; }

    ///// Rain-droplet number density
    //const std::vector<double>& qr() const { return outputData_.qr; }

    ///// Rain-droplet number density
    //const std::vector<double>& nr() const { return outputData_.nr; }

    ///// Cloud droplet number density
    //const std::vector<double>& nc() const { return outputData_.nc; }

    ///// Latent heating
    //const std::vector<double>& dthetadt() const { return outputData_.dthetadt; }
};

ISEN_NAMESPACE_END

#endif
