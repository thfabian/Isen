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
#ifndef ISEN_METEO_UTILS_H
#define ISEN_METEO_UTILS_H

#include <Isen/Common.h>
#include <cmath>

ISEN_NAMESPACE_BEGIN

/// @brief General meteorological utility functions
struct MetoUtils
{
    /// Kind of humidity
    enum HumidityKind
    {
        EDewPoint = 1,
        ERelative = 2
    };

    /// @brief Computes mixing ratio in (g/g)
    ///
    /// @param p        Pressure in [hPa]
    /// @param T        Temperature [K]
    /// @param humv     Humidity variable, either in [K] if humidityKind == HumidityKind::DEWPOINT or between [0,1] if
    ///                 humidityKind == HumidityKind::RELATIVE
    /// @param humidityKind Kind of humidity
    static inline VectorXf rrmixv1(const VectorXf& p, Float T, Float humv, HumidityKind humidityKind) noexcept
    {
        // Local constant
        const Float eps = 0.62198;

        Float esat;

        VectorXf mixv1(p.size());

        switch(humidityKind)
        {
            case EDewPoint:
                esat = eswat1(humv);
                // mixv1 = eps * esat / (p - esat);
                break;
            case ERelative:
                esat = eswat1(T);
        }

        // mixv1 = np.where(esat >= 0.616*p, 0., eps*humv*esat / (p - humv*esat))
        return mixv1;
    }

    /// @brief Saturation vapor pressure over water
    ///
    /// Using Goff-Gratch formulation which is based on exact integration of Clausius-Clapeyron equation.
    ///
    /// @param T    Temperature [K]
    static inline Float eswat1(Float T) noexcept
    {
        // Define local constants
        const Float C1 = 7.90298;
        const Float C2 = 5.02808;
        const Float C3 = 1.3816e-7;
        const Float C4 = 11.344;
        const Float C5 = 8.1328e-3;
        const Float C6 = 3.49149;

        Float rmixv = 373.16 / T;

        Float ES = -C1 * (rmixv - 1.0) + C2 * std::log10(rmixv) - C3 * (std::pow(10, C4 * (1.0 - 1.0 / rmixv)) - 1.0)
                   + C5 * (std::pow(10, -C6 * (rmixv - 1.0)) - 1.0);

        return (1013.246 * std::pow(10, ES));
    }
};

ISEN_NAMESPACE_END

#endif
