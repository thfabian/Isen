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
#include <Isen/Terminal.h>
#include <iostream>

ISEN_NAMESPACE_BEGIN

TEST_CASE("Terminal Width", "[!hide][Terminal]")
{
    int width = Terminal::getWidth();
    std::cout << "The following '#' should not line-break:" << std::endl;
    for(int i = 0; i < width; ++i)
        std::cout << "#";
}

// Simplify testing of colors
#define TERMINAL_SECTION_IMPL(ColorName, ColorType)                                                                    \
SECTION(ColorName)                                                                                                     \
{                                                                                                                      \
    {                                                                                                                  \
        Terminal::Color color(ColorType);                                                                              \
        std::cout << "This is " << ColorName << "." << std::endl;                                                      \
    }                                                                                                                  \
    {                                                                                                                  \
        std::cout << Terminal::Color(ColorType) << "This is " << ColorName << "." << std::endl;                        \
    }                                                                                                                  \
}

#define TERMINAL_SECTION(ColorName) TERMINAL_SECTION_IMPL(#ColorName, Terminal::Color::ColorName)


TEST_CASE("Terminal Coloring", "[!hide][Terminal]")
{
    TERMINAL_SECTION(None)
    TERMINAL_SECTION(White)
    TERMINAL_SECTION(Red)
    TERMINAL_SECTION(Green)
    TERMINAL_SECTION(Magenta)
    TERMINAL_SECTION(Yellow)
    TERMINAL_SECTION(Cyan)
    TERMINAL_SECTION(Grey)
    TERMINAL_SECTION(BoldWhite)
    TERMINAL_SECTION(BoldRed)
    TERMINAL_SECTION(BoldGreen)
    TERMINAL_SECTION(BoldMagenta)
    TERMINAL_SECTION(BoldYellow)
    TERMINAL_SECTION(BoldCyan)
    TERMINAL_SECTION(BoldGrey)
}

ISEN_NAMESPACE_END
