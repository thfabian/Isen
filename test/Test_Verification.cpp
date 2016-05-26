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

#include "FieldLoader.h"
#include "FieldVerifier.h"
#include "FileProxy.h"
#include "Test.h"

ISEN_NAMESPACE_BEGIN

TEST_CASE("FieldLoader", "[Verification]")
{
    MatrixXf test(2, 3);

    // Fill matrix with some values
    for(int i = 0; i < test.rows(); ++i)
        for(int j = 0; j < test.cols(); ++j)
            test(i, j) = i + j;

    // Prepare file
    std::string lines = (boost::format("%i %i") % test.rows() % test.cols()).str();
    for(int i = 0; i < test.rows(); ++i)
    {
        for(int j = 0; j < test.cols(); ++j)
            lines += (boost::format(" %f ") % test(i, j)).str();
        lines += "\n";
    }

    SECTION("Load field")
    {
        ProxyFile f(ProxyFile::PYTHON, {lines});
        MatrixXf mat(std::move(FieldLoader::load(f.getFilename())));
        CHECK(test == mat);
    }
}

TEST_CASE("FieldVerifier", "[Verification]")
{
    MatrixXf mat1(3, 3);
    VectorXf vec1(3);

    // Fill matrix with some values
    for(int i = 0; i < mat1.rows(); ++i)
        for(int j = 0; j < mat1.cols(); ++j)
        {
            mat1(i, j) = i + j;
            vec1(i) = i;
        }

    const MatrixXf mat2(mat1);
    const VectorXf vec2(vec1);

    SECTION("Success - Matrix") 
    { 
        CHECK(FieldVerifier::verify("mat", mat1, mat2)); 
    }

    SECTION("Success - Vector") 
    { 
        CHECK(FieldVerifier::verify("vec", vec1, vec2)); 
    }

    SECTION("Fail (value) - Matrix") 
    { 
        mat1(1, 1) = 100;
        mat1(2, 2) = 100;
        CHECK_FALSE(FieldVerifier::verify("mat", mat1, mat2, false)); 
    }

    SECTION("Fail (size) - Matrix") 
    { 
        MatrixXf mat3(4, 3);
        CHECK_FALSE(FieldVerifier::verify("mat", mat3, mat2, false)); 
    }

    SECTION("Fail (value) - Vector") 
    { 
        vec1(1) = 100;
        vec1(2) = 200;
        CHECK_FALSE(FieldVerifier::verify("vec", vec1, vec2, false)); 
    }

    SECTION("Fail (size)- Vector") 
    { 
        VectorXf vec3(4);
        CHECK_FALSE(FieldVerifier::verify("vec", vec3, vec2, false)); 
    }
}

ISEN_NAMESPACE_END
