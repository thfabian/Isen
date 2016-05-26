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
#ifndef ISEN_TERMINAL_H
#define ISEN_TERMINAL_H

#include <Isen/Common.h>
#include <ostream>

ISEN_NAMESPACE_BEGIN

/// Various terminal manipulation methods, most notably changing the color.
namespace Terminal
{

/// @brief Change the terminal output color.
///
/// The Terminal color will be changed either by injecting ANSI/VT100 console color codes to std::cout (Linux/Mac) or by
/// using the Win32 API (Windows). Note that the destructor will reset the color to the state before.
///
/// @code{.cpp}
///     std::cout << Terminal::Color(Terminal::Color::Red) << "This is red!\n";
///     std::cout << "This is the normal color again\n";
///     Terminal::Color color(Terminal::Color::Green);
///     std::cout << "This is green until end of scope\n";
/// @endcode
///
/// Colored output can be globally disabled by setting the static member Color::disableColor to false.
class Color
{
public:
    /// Disable colored output (default: false)
    static bool disableColor;

    enum Code
    {
        None = 0,
        White,
        Red,
        Green,
        Blue,
        Magenta,
        Yellow,
        Cyan,
        Grey,

        Bold = 0x10,
        BoldWhite = Bold | White,
        BoldRed = Bold | Red,
        BoldGreen = Bold | Green,
        BoldBlue = Bold | Blue,
        BoldMagenta = Bold | Magenta,
        BoldYellow = Bold | Yellow,
        BoldCyan = Bold | Cyan,
        BoldGrey = Bold | Grey
    };

    /// Set the console color
    Color(Code colorCode);

    /// Copy constructor
    Color(const Color& other);

    /// Reset the console color to the state before.
    ~Color();

    /// Change color persistently
    static void use(Code c);

    /// Get the color usually associated with files (BoldWhite on Windows/Linux and Grey on Mac OSX)
    static Color getFileColor();

    void setMoved(bool moved) { moved_ = moved; }
private:
    bool moved_; ///< Indicate whether the object has been moved
};

/// Allow usage in streams
/// @code{.cpp}
///     std::cout << Terminal::Color(Color::Red) << "This is red" << std::endl;
/// @endcode
inline std::ostream& operator<<(std::ostream& os, Color const&)
{
    return os;
}

/// Get the current width of the terminal
extern int getWidth();

} // end namespace Terminal

ISEN_NAMESPACE_END

#endif
