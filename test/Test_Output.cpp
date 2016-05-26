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

#include "Test.h"
#include "FileProxy.h"
#include <Isen/Output.h>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <iterator>

ISEN_NAMESPACE_BEGIN

#define CHECK_VEC(name, ...) CHECK(iData.name == decltype(iData.name)({__VA_ARGS__}))

TEST_CASE("OutputData serialize/unserialize", "[Output]")
{
    internal::OutputData oData, iData;
    oData.z = {1, 2};
    oData.u = {3, 4};
    oData.s = {5, 6};
    oData.t = {7, 8};

    oData.prec = {1, 2};
    oData.tot_prec = {3, 4};
    oData.qv = {5, 6};
    oData.qc = {7, 8};
    oData.qr = {1, 2};
    oData.nr = {3, 4};
    oData.nc = {5, 6};
    oData.dthetadt = {7, 8};

    SECTION("Text")
    {
        ProxyFile f;

        // Serialize
        std::ofstream fout(f.getFilename());
        boost::archive::text_oarchive oa(fout);
        oa << oData;
        fout.close();

        // Deserialize
        std::ifstream fin(f.getFilename());
        boost::archive::text_iarchive ia(fin);
        ia >> iData;
        fin.close();

        CHECK_VEC(z, 1, 2);
        CHECK_VEC(u, 3, 4);
        CHECK_VEC(s, 5, 6);
        CHECK_VEC(t, 7, 8);
        CHECK_VEC(prec, 1, 2);
        CHECK_VEC(tot_prec, 3, 4);
        CHECK_VEC(qv, 5, 6);
        CHECK_VEC(qc, 7, 8);
        CHECK_VEC(qr, 1, 2);
        CHECK_VEC(nr, 3, 4);
        CHECK_VEC(nc, 5, 6);
        CHECK_VEC(dthetadt, 7, 8);
    }

    SECTION("Xml")
    {
        ProxyFile f;

        // Serialize
        std::ofstream fout(f.getFilename());
        boost::archive::xml_oarchive oa(fout);
        oa << BOOST_SERIALIZATION_NVP(oData);
        fout.close();

        // Deserialize
        std::ifstream fin(f.getFilename());
        boost::archive::xml_iarchive ia(fin);
        ia >> BOOST_SERIALIZATION_NVP(iData);
        fin.close();

        CHECK_VEC(z, 1, 2);
        CHECK_VEC(u, 3, 4);
        CHECK_VEC(s, 5, 6);
        CHECK_VEC(t, 7, 8);
        CHECK_VEC(prec, 1, 2);
        CHECK_VEC(tot_prec, 3, 4);
        CHECK_VEC(qv, 5, 6);
        CHECK_VEC(qc, 7, 8);
        CHECK_VEC(qr, 1, 2);
        CHECK_VEC(nr, 3, 4);
        CHECK_VEC(nc, 5, 6);
        CHECK_VEC(dthetadt, 7, 8);
    }

    SECTION("Binary")
    {
        ProxyFile f;

        // Serialize
        std::ofstream fout(f.getFilename(), std::ios::out | std::ios::binary);
        boost::archive::binary_oarchive oa(fout);
        oa << oData;
        fout.close();

        // Deserialize
        std::ifstream fin(f.getFilename(), std::ios::in | std::ios::binary);
        boost::archive::binary_iarchive ia(fin);
        ia >> iData;
        fin.close();

        CHECK_VEC(z, 1, 2);
        CHECK_VEC(u, 3, 4);
        CHECK_VEC(s, 5, 6);
        CHECK_VEC(t, 7, 8);
        CHECK_VEC(prec, 1, 2);
        CHECK_VEC(tot_prec, 3, 4);
        CHECK_VEC(qv, 5, 6);
        CHECK_VEC(qc, 7, 8);
        CHECK_VEC(qr, 1, 2);
        CHECK_VEC(nr, 3, 4);
        CHECK_VEC(nc, 5, 6);
        CHECK_VEC(dthetadt, 7, 8);
    }
}

#undef CHECK_VEC

TEST_CASE("Namelist serialize/unserialize", "[Output]")
{
    boost::shared_ptr<NameList> iNameList(new NameList);
    boost::shared_ptr<NameList> oNameList;

    iNameList->run_name = "test"; // String
    iNameList->iout = 2;          // Integer
    iNameList->iiniout = false;   // Boolean
    iNameList->dx = 2.2;          // Float

    SECTION("Text")
    {
        ProxyFile f;

        // Serialize
        std::ofstream fout(f.getFilename());
        boost::archive::text_oarchive oa(fout);
        oa << iNameList;
        fout.close();

        // Deserialize
        std::ifstream fin(f.getFilename());
        boost::archive::text_iarchive ia(fin);
        ia >> oNameList;
        fin.close();

        CHECK(oNameList->run_name == oNameList->run_name);
        CHECK(oNameList->iout == oNameList->iout);
        CHECK(oNameList->iiniout == oNameList->iiniout);
        CHECK(oNameList->dx == oNameList->dx);
    }

    SECTION("Xml")
    {
        ProxyFile f;

        // Serialize
        std::ofstream fout(f.getFilename());
        boost::archive::xml_oarchive oa(fout);
        oa << BOOST_SERIALIZATION_NVP(iNameList);
        fout.close();

        // Deserialize
        std::ifstream fin(f.getFilename());
        boost::archive::xml_iarchive ia(fin);
        ia >> BOOST_SERIALIZATION_NVP(oNameList);
        fin.close();

        CHECK(oNameList->run_name == oNameList->run_name);
        CHECK(oNameList->iout == oNameList->iout);
        CHECK(oNameList->iiniout == oNameList->iiniout);
        CHECK(oNameList->dx == oNameList->dx);
    }

    SECTION("Binary")
    {
        ProxyFile f;

        // Serialize
        std::ofstream fout(f.getFilename(), std::ios::out | std::ios::binary);
        boost::archive::binary_oarchive oa(fout);
        oa << iNameList;
        fout.close();

        // Deserialize
        std::ifstream fin(f.getFilename(), std::ios::in | std::ios::binary);
        boost::archive::binary_iarchive ia(fin);
        ia >> oNameList;
        fin.close();

        CHECK(oNameList->run_name == oNameList->run_name);
        CHECK(oNameList->iout == oNameList->iout);
        CHECK(oNameList->iiniout == oNameList->iiniout);
        CHECK(oNameList->dx == oNameList->dx);
    }
}

ISEN_NAMESPACE_END
