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
struct MeteoUtils
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
    /// @param humv     Humidity variable, either in [K] if humidityKind == HumidityKind::EDewPoint or between [0,1] if
    ///                 humidityKind == HumidityKind::ERelative
    /// @param humidityKind Kind of humidity
    static inline double rrmixv1(double p, double T, double humv, HumidityKind humidityKind) noexcept
    {
        constexpr double eps = 0.62198;
        double mixv1 = 0.0;
                
        if(humidityKind == EDewPoint)
        {
            double esat = eswat1(humv);
            mixv1 = eps * esat / (p - esat);
        }
        else /* ERelative */
        {
            assert(humv >= 0.0 && humv <= 1.0);
            
            double esat = eswat1(T);
            if(esat >= 0.616 * p)
                mixv1 = 0.0;
            else /* esat < 0.616 * p */
                mixv1 = eps * humv * esat / (p - humv * esat);
        }
        return mixv1;
    }

    /// @brief Saturation vapor pressure over water
    ///
    /// Using Goff-Gratch formulation which is based on exact integration of Clausius-Clapeyron equation.
    ///
    /// @param T    Temperature [K]
    static inline double eswat1(double T) noexcept
    {
        // Define local constants
        constexpr double C1 = 7.90298;
        constexpr double C2 = 5.02808;
        constexpr double C3 = 1.3816e-7;
        constexpr double C4 = 11.344;
        constexpr double C5 = 8.1328e-3;
        constexpr double C6 = 3.49149;

        double rmixv = 373.16 / T;

        double ES = -C1 * (rmixv - 1.0) + C2 * std::log10(rmixv) - C3 * (std::pow(10, C4 * (1.0 - 1.0 / rmixv)) - 1.0)
                    + C5 * (std::pow(10, -C6 * (rmixv - 1.0)) - 1.0);

        return (1013.246 * std::pow(10, ES));
    }
};

ISEN_NAMESPACE_END

#endif
