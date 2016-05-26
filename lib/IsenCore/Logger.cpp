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

#include <Isen/Logger.h>
#include <Isen/Terminal.h>
#include <iostream>

ISEN_NAMESPACE_BEGIN

using namespace Terminal;

Logger* loggerInstance = new Logger;

Logger& Logger::operator<<(log loggerEnum) noexcept
{
    if(loggerEnum != log::enable && disableLogger_)
        return (*this);

    static int terminalWidth = getWidth();

    const int RightIndent = 0;

    switch(loggerEnum)
    {
        case log::ok:
        {
            std::string str = logStack_.str();
            std::string::size_type whiteSpace
                = terminalWidth - ((str.size() + curlen_) % 80 + timeStr_.size() + 1) - (7 + RightIndent);

            if(whiteSpace > terminalWidth)
                whiteSpace = 1;

            std::cout << str << std::string(whiteSpace, ' ') << timeStr_ << " ";
            std::cout << Color::getFileColor() << "[ OK ]" << std::endl;
            reset();
            break;
        }
        case log::failed:
        {
            std::string str = logStack_.str();
            std::string::size_type whiteSpace = terminalWidth - ((str.size() + curlen_) % 80) - (11 + RightIndent);

            if(whiteSpace > terminalWidth)
                whiteSpace = 1;

            std::cout << str << std::string(whiteSpace, ' ');
            std::cout << Color(Color::BoldRed) << "[ FAILED ]" << std::endl;
            reset();
            break;
        }
        case log::endl:
        {
            std::cout << logStack_.str() << std::endl;
            reset();
            break;
        }
        case log::flush:
        {
            std::string str = logStack_.str();
            curlen_ += str.size();
            std::cout << str << std::flush;
            clear();
            break;
        }
        case log::disable:
            disableLogger_ = true;
            break;
        case log::enable:
            disableLogger_ = false;
            reset();
            break;
        case log::nop:
        default:
            break;
    }
    return (*this);
}

ISEN_NAMESPACE_END
