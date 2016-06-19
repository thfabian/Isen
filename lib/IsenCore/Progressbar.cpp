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

#include <Isen/Progressbar.h>
#include <Isen/Terminal.h>
#include <cstdio>
#include <iostream>
#include <string>

ISEN_NAMESPACE_BEGIN

using namespace Terminal;

bool Progressbar::disableProgressbar = false;
const float Progressbar::IntervalMs = 10; // 10 ms

Progressbar::Progressbar(int maxTimeStep) : curStep_(0), maxStep_(maxTimeStep), curBarWidth_(0)
{
    terminalWidth_ = Terminal::getWidth() - 1;
    if(terminalWidth_ <= 1)
        terminalWidth_ = 80;

    maxBarWidth_ = 3 * terminalWidth_ / 4;

    strSpace_ = std::string(terminalWidth_ - (maxBarWidth_ + 3 + 7), ' ');
    strBar_ = std::string(maxBarWidth_, ' ');
    
    // Start the timer (this assures we print the first time in advance)
    timer_.start();
}

void Progressbar::advance()
{
    if(disableProgressbar)
        return;

    if(++curStep_ == maxStep_)
        return;

    float percentage = float(curStep_) / maxStep_;
    int barWidth = percentage * maxBarWidth_;
    bool printThisStep;

    while((printThisStep = curBarWidth_ < (barWidth ? barWidth : 1)))
        strBar_[curBarWidth_++] = '=';

    // Only print if there was an update or IntervalMs milliseconds have passed
    if(printThisStep || timer_.stop() > IntervalMs)
    {
        std::printf("[%s]%s %05.2f %%\r", strBar_.c_str(), strSpace_.c_str(), 100 * percentage);
        std::fflush(stdout);
    }

    timer_.start();
}

void Progressbar::pause() const
{
    if(disableProgressbar)
        return;

    std::printf("%s\r", std::string(terminalWidth_, ' ').c_str());
    std::fflush(stdout);
}

void Progressbar::printBar(char c)
{
    if(disableProgressbar)
        return;

    int width = Terminal::getWidth() - 1;
    if(width < 0)
        width = 80;
    std::cout << Color::getFileColor() << std::string(width, c) << std::endl;
}

ISEN_NAMESPACE_END
