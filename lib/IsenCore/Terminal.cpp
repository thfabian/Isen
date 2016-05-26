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

#include <Isen/Terminal.h>
#include <iostream>

#ifdef ISEN_PLATFORM_WINDOWS
#include <windows.h>
#elif defined(ISEN_PLATFORM_POSIX)
#include <sys/ioctl.h>
#include <unistd.h>
#endif

ISEN_NAMESPACE_BEGIN

using namespace Terminal;

int Terminal::getWidth()
{
    int width = 80; // default
#ifdef ISEN_PLATFORM_WINDOWS
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    width = (int) (csbi.srWindow.Right - csbi.srWindow.Left + 1);
#elif defined(ISEN_PLATFORM_POSIX)
    struct winsize ws;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
    width = (int) ws.ws_col;
#endif
    return (width <= 80 || width >= 256 ? 80 : width);
}

Color Color::getFileColor()
{
#ifdef ISEN_PLATFORM_APPLE
    return Color(Color::Grey);
#else
    return Color(Color::BoldWhite);
#endif
}

bool Terminal::Color::disableColor = false;

namespace internal
{

struct ColorImpl
{
    virtual ~ColorImpl() {}
    virtual void use(Color::Code colorCode) = 0;
};

// Empty implementation
struct ColorImplEmpty : public ColorImpl
{
    virtual ~ColorImplEmpty() {}
    virtual void use(Color::Code colorCode) override {}
};

#ifdef ISEN_PLATFORM_WINDOWS

// Windows implementation
struct ColorImplWin32 : public ColorImpl
{
    ColorImplWin32() : stdoutHandle_(GetStdHandle(STD_OUTPUT_HANDLE))
    {
        CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
        GetConsoleScreenBufferInfo(stdoutHandle_, &csbiInfo);
        foreAtrr_
            = csbiInfo.wAttributes & ~(BACKGROUND_GREEN | BACKGROUND_RED | BACKGROUND_BLUE | BACKGROUND_INTENSITY);
        backAttr_
            = csbiInfo.wAttributes & ~(FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    }

    virtual ~ColorImplWin32() {}
    virtual void use(Color::Code colorCode) override
    {
        auto setAttr = [&](WORD textAttr) -> void { SetConsoleTextAttribute(stdoutHandle_, textAttr | backAttr_); };

        switch(colorCode)
        {
            case Color::None:
                setAttr(foreAtrr_);
                break;
            case Color::White:
                setAttr(FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
                break;
            case Color::Red:
                setAttr(FOREGROUND_RED);
                break;
            case Color::Blue:
                setAttr(FOREGROUND_BLUE);
                break;
            case Color::Green:
                setAttr(FOREGROUND_GREEN);
                break;
            case Color::Magenta:
                setAttr(FOREGROUND_RED | FOREGROUND_BLUE);
                break;
            case Color::Yellow:
                setAttr(FOREGROUND_RED | FOREGROUND_GREEN);
                break;
            case Color::Cyan:
                setAttr(FOREGROUND_BLUE | FOREGROUND_GREEN);
                break;
            case Color::Grey:
                setAttr(FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
                break;
            case Color::BoldWhite:
                setAttr(FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
                break;
            case Color::BoldRed:
                setAttr(FOREGROUND_INTENSITY | FOREGROUND_RED);
                break;
            case Color::BoldBlue:
                setAttr(FOREGROUND_INTENSITY | FOREGROUND_BLUE);
                break;
            case Color::BoldGreen:
                setAttr(FOREGROUND_INTENSITY | FOREGROUND_GREEN);
                break;
            case Color::BoldMagenta:
                setAttr(FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE);
                break;
            case Color::BoldYellow:
                setAttr(FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
                break;
            case Color::BoldCyan:
                setAttr(FOREGROUND_INTENSITY | FOREGROUND_BLUE | FOREGROUND_GREEN);
                break;
            case Color::BoldGrey:
                setAttr(FOREGROUND_INTENSITY);
                break;
            default: // Color::Bold
                throw std::logic_error("invalid color");
        }
    }

private:
    HANDLE stdoutHandle_;
    WORD foreAtrr_;
    WORD backAttr_;
};

#elif defined(ISEN_PLATFORM_POSIX)

// POSIX implementation
struct ColorImplPosix : public ColorImpl
{
    virtual ~ColorImplPosix() {}
    virtual void use(Color::Code colorCode) override
    {
        auto setAttr = [&](const char* escapeCode) -> void { std::cout << "\033" << escapeCode; };

        switch(colorCode)
        {
            case Color::None:
                setAttr("[0;39m");
                break;
            case Color::White:
                setAttr("[0;37m");
                break;
            case Color::Red:
                setAttr("[0;31m");
                break;
            case Color::Green:
                setAttr("[0;32m");
                break;
            case Color::Blue:
                setAttr("[0;34m");
                break;
            case Color::Magenta:
                setAttr("[0;35m");
                break;
            case Color::Yellow:
                setAttr("[0;33m");
                break;
            case Color::Cyan:
                setAttr("[0;36m");
                break;
            case Color::Grey:
                setAttr("[1;30m");
                break;
            case Color::BoldWhite:
                setAttr("[1;37m");
                break;
            case Color::BoldRed:
                setAttr("[1;31m");
                break;
            case Color::BoldBlue:
                setAttr("[1;34m");
                break;
            case Color::BoldGreen:
                setAttr("[1;32m");
                break;
            case Color::BoldMagenta:
                setAttr("[1;35m");
                break;
            case Color::BoldYellow:
                setAttr("[1;33m");
                break;
            case Color::BoldCyan:
                setAttr("[1;36m");
                break;
            case Color::BoldGrey:
                setAttr("[0;37m");
                break;
            default: // Color::Bold
                throw std::logic_error("invalid color");
        }
    }
};

#endif

// Factory method to get the platform specific implementation
ColorImpl* ColorFactory()
{
    if(Terminal::Color::disableColor)
    {
        static ColorImplEmpty colorInstance;
        return static_cast<ColorImpl*>(&colorInstance);
    }
#ifdef ISEN_PLATFORM_WINDOWS
    static ColorImplWin32 colorInstance;
#elif defined(ISEN_PLATFORM_POSIX)
    static ColorImplPosix colorInstance;
#else
    static ColorImplEmpty colorInstance;
#endif
    return static_cast<ColorImpl*>(&colorInstance);
}

} // end namespace internal

Color::Color(Color::Code colorCode) : moved_(false)
{
    use(colorCode);
}

Color::Color(const Color& other) : moved_(false)
{
    const_cast<Color&>(other).setMoved(true);
}

Color::~Color()
{
    if(!moved_)
        use(Color::None);
}

void Color::use(Color::Code colourCode)
{
    static internal::ColorImpl* colorInstance = internal::ColorFactory();
    colorInstance->use(colourCode);
}

ISEN_NAMESPACE_END
