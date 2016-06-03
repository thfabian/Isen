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
#ifndef ISEN_OUTPUT_H
#define ISEN_OUTPUT_H

#include <Isen/Common.h>
#include <Isen/NameList.h>
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/version.hpp>
#include <boost/shared_ptr.hpp>
#include <string>
#include <vector>

ISEN_NAMESPACE_BEGIN

namespace internal
{

/// Conversion between boost and std::shared_ptr (boost versions < 1.56 don't upport serialization of std::shared_ptr)
template <typename T>
boost::shared_ptr<T> make_shared_ptr(std::shared_ptr<T>& ptr)
{
    return boost::shared_ptr<T>(ptr.get(), [ptr](T*) mutable { ptr.reset(); });
}

template <typename T>
std::shared_ptr<T> make_shared_ptr(boost::shared_ptr<T>& ptr)
{
    return std::shared_ptr<T>(ptr.get(), [ptr](T*) mutable { ptr.reset(); });
}

/// This class will be serialized
class OutputData
{
public:
    friend class boost::serialization::access;

    template <class Archive>
    void serialize(Archive& ar, const unsigned int /* version */)
    {
        ar& BOOST_SERIALIZATION_NVP(z);
        ar& BOOST_SERIALIZATION_NVP(u);
        ar& BOOST_SERIALIZATION_NVP(s);
        ar& BOOST_SERIALIZATION_NVP(t);
        ar& BOOST_SERIALIZATION_NVP(prec);
        ar& BOOST_SERIALIZATION_NVP(tot_prec);
        ar& BOOST_SERIALIZATION_NVP(qv);
        ar& BOOST_SERIALIZATION_NVP(qc);
        ar& BOOST_SERIALIZATION_NVP(qr);
        ar& BOOST_SERIALIZATION_NVP(nr);
        ar& BOOST_SERIALIZATION_NVP(nc);
        ar& BOOST_SERIALIZATION_NVP(dthetadt);
    }

    //-------------------------------------------------
    // Define output fields
    //-------------------------------------------------
    std::vector<double> z;        ///< Height in z-coordinates
    std::vector<double> u;        ///< Horizontal velocity
    std::vector<double> s;        ///< Isentropic density
    std::vector<double> t;        ///< Time vector
    std::vector<double> prec;     ///< Precipitation
    std::vector<double> tot_prec; ///< Accumulated precipitation
    std::vector<double> qv;       ///< Specific humidity
    std::vector<double> qc;       ///< Specific cloud water content
    std::vector<double> qr;       ///< Specific rain water content
    std::vector<double> nr;       ///< Rain-droplet number density
    std::vector<double> nc;       ///< Cloud droplet number density
    std::vector<double> dthetadt; ///< Latent heating
};
}

/// @brief Handle writing and reading of the simulation to an output file
class Output
{
public:
    /// Archive type
    enum ArchiveType
    {
        Unknown = 0,
        Text,
        Xml,
        Binary
    };

    /// Initialize output engine (ReadWrite Mode)
    Output(std::shared_ptr<NameList> namelist, ArchiveType archiveType = Text);

    /// Initialize output engine (ReadOnly Mode)
    Output(ArchiveType archiveType = Unknown);

    /// @brief Append current fields to the output fields.
    ///
    /// The Output needs to be initalized in ReadWrite mode
    void makeOutput(const Solver* solver) noexcept;

    /// @brief Open the output archive and serialize the fields.
    ///
    /// This will produce an output file named after NameList::run_name (if the file exists already a timestemp will be
    /// appended), optionally you can directly pass a filename. The simulation will be serialized to an archive and can
    /// be retrived using Output::read().
    void write(std::string filename = "");

    /// @brief Read the input archive and deserialize the fields.
    ///
    /// After this operation the fields will be available via the getter methods
    void read(const std::string& filename);

    /// Access NameList (ReadOnly)
    const NameList* getNameList() const { return namelist_.get(); }

    /// Set the archive type
    void setArchiveType(ArchiveType arch) { archiveType_ = arch; }

    /// Get the archive type
    ArchiveType getArchiveType() const { return archiveType_; }

private:
    ArchiveType archiveType_;
    boost::shared_ptr<NameList> namelist_;

    int curIt_;                       ///< Output step
    MatrixXf u_out_;                  ///< Destaggered velocity
    MatrixXf dthetadt_out_;           ///< Destaggered latent heating
    internal::OutputData outputData_; ///< Store the actual data

public:
    /// Height in z-coordinates
    const std::vector<double>& z() const { return outputData_.z; }

    /// Horizontal velocity
    const std::vector<double>& u() const { return outputData_.u; }

    /// Isentropic density
    const std::vector<double>& s() const { return outputData_.s; }

    /// Time vector
    const std::vector<double>& t() const { return outputData_.t; }

    /// Accumulated precipitation
    const std::vector<double>& prec() const  { return outputData_.prec; }

    /// Specific humidity
    const std::vector<double>& tot_prec() const { return outputData_.tot_prec; }

    /// Specific cloud water content
    const std::vector<double>& qv() const { return outputData_.qv; }

    /// Specific rain water content
    const std::vector<double>& qc() const { return outputData_.qc; }

    /// Rain-droplet number density
    const std::vector<double>& qr() const { return outputData_.qr; }

    /// Rain-droplet number density
    const std::vector<double>& nr() const { return outputData_.nr; }

    /// Cloud droplet number density
    const std::vector<double>& nc() const { return outputData_.nc; }

    /// Latent heating
    const std::vector<double>& dthetadt() const { return outputData_.dthetadt; }
};

ISEN_NAMESPACE_END

// Current version of OutputData
BOOST_CLASS_VERSION(Isen::internal::OutputData, 0);

#endif
