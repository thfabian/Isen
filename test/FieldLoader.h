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
#ifndef ISEN_TEST_FIELD_LOADER_H
#define ISEN_TEST_FIELD_LOADER_H

#include <Isen/Common.h>
#include <fstream>
#include <string>

ISEN_NAMESPACE_BEGIN

class FieldLoader
{
public:
    static MatrixXf load(std::string filename)
    {
        std::ifstream fin(filename);
        if(!fin.is_open())
            throw IsenException("cannot open file: '%s'", filename);
        
        // Read header
        int n, m;
        fin >> n >> m;
        MatrixXf mat(n, m);
 
        // Read content (we read row-wise)
        for(int i = 0; i < mat.rows(); ++i)
            for(int j = 0; j < mat.cols(); ++j)
                fin >> mat(i, j);

        fin.close();
        return mat;
    }
};

ISEN_NAMESPACE_END

#endif
