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
#ifndef ISEN_TEST_FIELD_VERIFIER_H
#define ISEN_TEST_FIELD_VERIFIER_H

#include <Isen/Common.h>
#include <cmath>
#include <iostream>
#include <string>

ISEN_NAMESPACE_BEGIN

class FieldVerifier
{
public:
    static constexpr double atol = 1e-10;
    static constexpr double rtol = 1e-06;

    static inline bool equal(double a, double b) noexcept
    {
        return (std::fabs(a - b) <= (FieldVerifier::atol + FieldVerifier::rtol * std::fabs(b)));
    }

    template <class DerivedType, class MatrixType>
    static bool verify(std::string name,
                       const Eigen::MatrixBase<DerivedType>& test,
                       MatrixType&& ref,
                       const bool verbose = true,
                       int maxErrors = 10) noexcept
    {
        MatrixXf test_m(test);

        if(test.cols() != ref.cols() || test.rows() != ref.rows())
        {
            if(verbose)
                std::cerr << boost::format("\nThe test field '%s' has a diffrent size than the refrence field.\n"
                                           " %-15s as: (%i, %i)\n %-15s as: (%i, %i)\n")
                                 % name % name % test.rows() % test.cols() % "refrence" % ref.rows() % ref.cols();
            return false;
        }

        bool passed = true;
        bool printedHeader = false;

        for(int j = 0; j < test_m.cols(); ++j)
        {
            for(int i = 0; i < test_m.rows(); ++i)
            {
                if(!FieldVerifier::equal(test_m(i, j), ref(i, j)))
                {
                    passed = false;
                    if(verbose && --maxErrors >= 0)
                    {
                        // Print header
                        if(!printedHeader)
                        {
                            std::cerr << boost::format("\n\n%9s | %24s | %24s\n") % "Position"
                                             % (boost::format("Actual [%s]") % name).str() % "Refrence";
                            std::cerr << std::string(63, '-') << "\n";
                            printedHeader = true;
                        }

                        // Print error
                        std::cerr << boost::format("(%3i,%3i) | %24.12f | %24.12f\n") % i % j % test_m(i, j)
                                         % ref(i, j);
                    }
                }
            }
        }

        if(verbose && !passed)
            std::cerr << std::endl;

        return passed;
    }
};

ISEN_NAMESPACE_END

#endif
