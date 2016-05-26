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
#include "FieldLoader.h"

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

ISEN_NAMESPACE_END
