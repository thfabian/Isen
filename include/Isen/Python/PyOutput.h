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
#include <Isen/Python/PyNameList.h>
#include <Isen/Python/PyType.h>

ISEN_NAMESPACE_BEGIN

/// @brief Expose the Output interface to Python
class PyOutput
{
public:
    /// Default constructor
    PyOutput(const char* file = nullptr);

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
        return internal::toNumpyArrayImpl(output_->z().data(), namelist_->nout, namelist_->nx, namelist_->nz1);
    }

    /// Horizontal velocity
    boost::python::object u() const
    {
        if(!output_)
            throw IsenException("Output: not initialized");
        return internal::toNumpyArrayImpl(output_->u().data(), namelist_->nout, namelist_->nx, namelist_->nz);
    }

    /// Isentropic density
    boost::python::object s() const
    {
        if(!output_)
            throw IsenException("Output: not initialized");
        return internal::toNumpyArrayImpl(output_->s().data(), namelist_->nout, namelist_->nx, namelist_->nz);
    }

    /// Time vector
    boost::python::object t() const
    {
        if(!output_)
            throw IsenException("Output: not initialized");
        return internal::toNumpyArrayImpl(output_->t().data(), namelist_->nout);
    }
    /// Precipitation
    boost::python::object prec() const
    {
        if(!output_)
            throw IsenException("Output: not initialized");

        if(!namelist_->imoist)
            throw IsenException("Output: prec is not available");
        return internal::toNumpyArrayImpl(output_->prec().data(), namelist_->nout, namelist_->nx);
    }

    /// Accumulated precipitation
    boost::python::object tot_prec() const
    {
        if(!output_)
            throw IsenException("Output: not initialized");

        if(!namelist_->imoist)
            throw IsenException("Output: tot_prec is not available");

        return internal::toNumpyArrayImpl(output_->tot_prec().data(), namelist_->nout, namelist_->nx);
    }

    /// Specific humidity
    boost::python::object qv() const
    {
        if(!output_)
            throw IsenException("Output: not initialized");

        if(!namelist_->imoist)
            throw IsenException("Output: qv is not available");

        return internal::toNumpyArrayImpl(output_->qv().data(), namelist_->nout, namelist_->nx, namelist_->nz);
    }

    /// Specific cloud water content
    boost::python::object qc() const
    {
        if(!output_)
            throw IsenException("Output: not initialized");

        if(!namelist_->imoist)
            throw IsenException("Output: qc is not available");

        return internal::toNumpyArrayImpl(output_->qc().data(), namelist_->nout, namelist_->nx, namelist_->nz);
    }

    /// Specific rain water content
    boost::python::object qr() const
    {
        if(!output_)
            throw IsenException("Output: not initialized");

        if(!namelist_->imoist)
            throw IsenException("Output: qr is not available");

        return internal::toNumpyArrayImpl(output_->qr().data(), namelist_->nout, namelist_->nx, namelist_->nz);
    }

    /// Rain-droplet number density
    boost::python::object nr() const
    {
        if(!output_)
            throw IsenException("Output: not initialized");

        if(!namelist_->imoist && namelist_->imicrophys != 2)
            throw IsenException("Output: nr is not available");

        return internal::toNumpyArrayImpl(output_->nr().data(), namelist_->nout, namelist_->nx, namelist_->nz);
    }

    /// Cloud droplet number density
    boost::python::object nc() const
    {
        if(!output_)
            throw IsenException("Output: not initialized");

        if(!namelist_->imoist && namelist_->imicrophys != 2)
            throw IsenException("Output: nc is not available");

        return internal::toNumpyArrayImpl(output_->nc().data(), namelist_->nout, namelist_->nx, namelist_->nz);
    }

    /// Latent heating
    boost::python::object dthetadt() const
    {
        if(!output_)
            throw IsenException("Output: not initialized");

        if(!namelist_->imoist && namelist_->idthdt)
            throw IsenException("Output: dthetadt is not available");

        return internal::toNumpyArrayImpl(output_->dthetadt().data(), namelist_->nout, namelist_->nx, namelist_->nz);
    }
};

ISEN_NAMESPACE_END

#endif
