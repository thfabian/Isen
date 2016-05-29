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

#include <Isen/Logger.h>
#include <Isen/Output.h>
#include <Isen/Solver.h>
#include <array>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/filesystem.hpp>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <sstream>

ISEN_NAMESPACE_BEGIN

Output::Output(Output::ArchiveType archiveType) : archiveType_(archiveType), curIt_(0)
{
}

Output::Output(std::shared_ptr<NameList> namelist, Output::ArchiveType archiveType)
    : archiveType_(archiveType), namelist_(internal::make_shared_ptr(namelist)), curIt_(0)
{
    SOLVER_DECLARE_ALL_ALIASES

    Timer t;
    LOG() << "Preparing output ... " << logger::flush;

    // Allocate memory
    try
    {
        u_out_ = MatrixXf::Zero(nx, nz);

        outputData_.z.resize(nout * nz1 * nx);
        outputData_.u.resize(nout * nz * nx);
        outputData_.s.resize(nout * nz * nx);
        outputData_.t.resize(nout);

        if(imoist)
        {
            outputData_.prec.resize(nout * nx);
            outputData_.tot_prec.resize(nout * nx);
            outputData_.qv.resize(nout * nz * nx);
            outputData_.qc.resize(nout * nz * nx);
            outputData_.qr.resize(nout * nz * nx);

            if(imicrophys == 2)
            {
                outputData_.nr.resize(nout * nz * nx);
                outputData_.nc.resize(nout * nz * nx);
            }

            if(idthdt)
            {
                dthetadt_out_ = MatrixXf::Zero(nxb, nz);
                outputData_.dthetadt.resize(nout * nz * nx);
            }
        }
    }
    catch(std::bad_alloc&)
    {
        LOG() << logger::failed;
        throw IsenException("out of memory");
    }

    if(archiveType_ == Unknown)
        archiveType_ = Text;

    LOG_SUCCESS(t);
}

void Output::makeOutput(const Solver* solver) noexcept
{
    SOLVER_DECLARE_ALL_ALIASES

    // Horizontal destagger
    //------------------------------------------------------------
    u_out_ = 0.5 * (solver->unow().block(nb, 0, nx, nz).array() + solver->unow().block(nb + 1, 0, nx, nz).array());

    // Vertical destagger
    //------------------------------------------------------------
    if(imoist && idthdt)
        dthetadt_out_ = 0.5 * (solver->dthetadt().block(0, 0, nxb, nz) + solver->dthetadt().block(0, 1, nxb, nz + 1));

    // Height in z-coordinates
    auto it_z = outputData_.z.begin() + curIt_ * nz1 * nx;
    for(int j = 0; j < nz1; ++j)
        for(int i = nb; i < (nx + nb); ++i, ++it_z)
            *it_z = solver->zhtnow()(i, j);

    // Horizontal velocity
    auto it_u = outputData_.u.begin() + curIt_ * nz * nx;
    for(int j = 0; j < nz; ++j)
        for(int i = 0; i < nx; ++i, ++it_u)
            *it_u = u_out_(i, j);

    // Isentropic density
    auto it_s = outputData_.s.begin() + curIt_ * nz * nx;
    for(int j = 0; j < nz; ++j)
        for(int i = nb; i < (nx + nb); ++i, ++it_s)
            *it_s = solver->snow()(i, j);

    // Time vector
    outputData_.t[curIt_] = curIt_ * iout * dt;

    // Precipitation

    // Accumulated precipitation

    // Specific humidity

    // Specific cloud water content

    // Specific rain water content

    // Rain-droplet number density

    // Cloud droplet number density

    // Latent heating

    ++curIt_;
}

void Output::write(std::string filename)
{
    std::ios_base::openmode flags = std::ios::out;

    if(filename.empty())
    {
        std::string ext;
        switch(archiveType_)
        {
            case ArchiveType::Text:
                ext = ".txt";
                break;
            case ArchiveType::Xml:
                ext = ".xml";
                break;
            case ArchiveType::Binary:
                ext = ".bin";
                flags |= std::ios::binary;
                break;
            default:
                throw IsenException("unknown archive type");
        }

        // Create (unique) file
        filename = namelist_->run_name;
        if(boost::filesystem::exists(filename + ext))
        {
            std::array<char, 80> buffer;
            auto t = std::time(nullptr);
            auto tm = std::localtime(&t);

            std::strftime(buffer.data(), buffer.size(), "-%H-%M-%S", tm);
            filename += std::string(buffer.data());
        }
        filename += ext;
    }

    Timer t;
    LOG() << "Writing to '" << filename << "' ..." << logger::flush;

    std::ofstream fout(filename, flags);
    if(!fout.good())
    {
        LOG() << logger::failed;
        throw IsenException("failed to open file: %s", filename);
    }

    // Serialize
    switch(archiveType_)
    {
        case ArchiveType::Text:
        {
            boost::archive::text_oarchive oa(fout);
            oa << outputData_;
            oa << namelist_;
            break;
        }
        case ArchiveType::Xml:
        {
            boost::archive::xml_oarchive oa(fout);
            oa << boost::serialization::make_nvp("OutputData", outputData_);
            oa << boost::serialization::make_nvp("NameList", namelist_);
            break;
        }
        case ArchiveType::Binary:
        {
            boost::archive::binary_oarchive oa(fout);
            oa << outputData_;
            oa << namelist_;
            break;
        }
        default:
            LOG() << logger::failed;
            throw IsenException("unknown archive type");
    }

    fout.close();
    LOG_SUCCESS(t);
}

void Output::read(const std::string& filename)
{
    Timer t;
    LOG() << "Reading from '" << filename << "' ..." << logger::flush;

    // Deduce archive type
    if(archiveType_ == ArchiveType::Unknown)
    {
        std::string ext = boost::filesystem::path(filename).extension().string();

        if(ext == ".txt")
            archiveType_ = ArchiveType::Text;
        else if(ext == ".xml")
            archiveType_ = ArchiveType::Xml;
        else if(ext == ".bin")
            archiveType_ = ArchiveType::Binary;
        else
        {
            LOG() << logger::failed;
            throw IsenException("couldn't deduce archive type from file extension: %s", filename);
        }
    }

    std::ios_base::openmode flags
        = archiveType_ == ArchiveType::Binary ? std::ios::in | std::ios::binary : std::ios::in;

    std::ifstream fin(filename, flags);
    if(!fin.is_open())
    {
        LOG() << logger::failed;
        throw IsenException("no such file: %f", filename.c_str());
    }

    // Deserialize
    switch(archiveType_)
    {
        case ArchiveType::Text:
        {
            boost::archive::text_iarchive ia(fin);
            ia >> outputData_;
            ia >> namelist_;
            break;
        }
        case ArchiveType::Xml:
        {
            boost::archive::xml_iarchive ia(fin);
            ia >> boost::serialization::make_nvp("OutputData", outputData_);
            ia >> boost::serialization::make_nvp("NameList", namelist_);
            break;
        }
        case ArchiveType::Binary:
        {
            boost::archive::binary_iarchive ia(fin);
            ia >> outputData_;
            ia >> namelist_;
            break;
        }
        default:
        {
            LOG() << logger::failed;
            throw IsenException("couldn't deduce archive type from file extension: %s", filename);
        }
    }

    fin.close();
    LOG_SUCCESS(t);
}


ISEN_NAMESPACE_END
