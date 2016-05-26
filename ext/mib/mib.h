/**              _   _
 *              (_) | |
 *   _ __ ___    _  | |__
 *  | '_ ` _ \  | | | '_ \
 *  | | | | | | | | | |_) |
 *  |_| |_| |_| |_| |_.__/   - Micro Benchmark
 *
 *  Copyright (C) 2016  Fabian Thuring (thfabian@student.ethz.ch)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#ifndef __INCLUDED_MIB_H__
#define __INCLUDED_MIB_H__

#ifdef __clang__
#pragma clang system_header
#elif defined __GNUC__
#pragma GCC system_header
#endif

/////////////////////////////////// MACROS /////////////////////////////////////

#ifdef MIB_OFF

#define MIB_START(name) ((void)0);
#define MIB_STOP(name) ((void)0);
#define MIB_NEXT(name) ((void)0);
#define MIB_PRINT(name) ((void)0);

#else

#define MIB_START(name)                                                        \
    mib::internal::mib_Profiler.timer_.stop();                                 \
    mib::internal::mib_Profiler.start(name);                                   \
    mib::internal::mib_Profiler.timer_.start();

#define MIB_NEXT(name)                                                         \
    mib::internal::mib_Profiler.timer_.stop();                                 \
    mib::internal::mib_Profiler.next(name);                                    \
    mib::internal::mib_Profiler.timer_.start();

#define MIB_STOP(name)                                                         \
    mib::internal::mib_Profiler.timer_.stop();                                 \
    mib::internal::mib_Profiler.stop(name);                                    \
    mib::internal::mib_Profiler.timer_.start();

#define MIB_PRINT(mode)                                                        \
    mib::internal::mib_Profiler.setTimerMode(mode);                            \
    mib::internal::mib_Profiler.printTree();


////////////////////////////////// VERSION /////////////////////////////////////

#define MIB_QUOTE_IMPL(S) #S
#define MIB_STR_IMPL(S) MIB_QUOTE_IMPL(S)

#define MIB_VERSION_MAJOR 0
#define MIB_VERSION_MINOR 0
#define MIB_VERSION_PATCH 1
#define MIB_VERSION_STRING MIB_STR_IMPL(MIB_VERSION_MAJOR) "."                 \
                           MIB_STR_IMPL(MIB_VERSION_MINOR) "."                 \
                           MIB_STR_IMPL(MIB_VERSION_PATCH)

////////////////////////////////// COMPILER ////////////////////////////////////

#if defined(__clang__)
#define MIB_COMPILER_CLANG 1
#endif

#if defined(__ICC) || defined(__INTEL_COMPILER)
#define MIB_COMPILER_INTEL 1
#endif 

#if defined(__GNUC__) || defined(__GNUG__)
#define MIB_COMPILER_GNU 1
#endif

#if defined(_MSC_VER)
#define MIB_COMPILER_MSVC 1
#endif

// INLINE compiler intrinsic
#if defined(MIB_COMPILER_GNU)
#define MIB_INLINE inline __attribute__((always_inline))
#elif defined(MIB_COMPILER_MSVC)
#define MIB_INLINE inline __forceinline
#else
#define MIB_INLINE inline
#endif

// NORETURN compiler intrinsic
#if defined(MIB_COMPILER_GNU)
#define MIB_NORETURN __attribute__((noreturn))
#elif defined(MIB_COMPILER_MSVC)
#define MIB_NORETURN __declspec(noreturn)
#else
#define MIB_NORETURN
#endif

// UNUSED compiler instrinsic
#if defined(MIB_COMPILER_GNU)
#define MIB_UNUSED __attribute__((unused))
#else
#define MIB_UNUSED
#endif

/////////////////////////////////// CONFIG /////////////////////////////////////

// Namespaces
#define MIB_INTERNAL_NAMESPACE_BEGIN namespace mib { namespace internal {
#define MIB_INTERNAL_NAMESPACE_END } }

// Define platform
#if defined(_MSC_VER) || defined(_WIN32) || defined(_WIN64)
#define MIB_PLATFORM_WINDOWS 1
#elif defined(__linux__ ) || defined(__linux)
#define MIB_PLATFORM_LINUX 1
#elif defined(__APPLE__)
#define MIB_PLATFORM_APPLE 1
#endif

#if defined (__unix__) || defined(MIB_PLATFORM_APPLE)
#define MIB_PLATFORM_POSIX 1
#endif

////////////////////////////////// WARNINGS ////////////////////////////////////

#if defined(__GNUC__) || defined(__GNUG__)

#if __cplusplus < 201103L
#error This file requires compiler and library support for the \
ISO C++ 2011 standard. This support is currently experimental, and must be \
enabled with the -std=c++11 or -std=gnu++11 compiler options.
#endif

#elif defined(_MSC_VER)

#if _MSC_VER < 1900
#error Unsupported Microsoft(R) Visual Studio version! Only versions 2015 \
(MSVC 14) and up are supported!
#endif

#endif

////////////////////////////////// INCLUDES ////////////////////////////////////

#include <algorithm>
#include <cassert>
#include <cinttypes>
#include <cmath>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <ios>
#include <map>
#include <string>
#include <set>
#include <sstream>
#include <utility>
#include <vector>

#if defined(MIB_PLATFORM_WINDOWS) 

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <windows.h>
#include <intrin.h>

#ifdef MIB_UTF8
#error "UTF8 printing is not supported on Windows"
#endif

#elif defined(MIB_PLATFORM_POSIX)
#include <sys/ioctl.h>
#include <unistd.h>

#ifndef MIB_NO_UTF8
#define MIB_UTF8
#endif

#endif

///////////////////////////////// ACCUMULATOR //////////////////////////////////

MIB_INTERNAL_NAMESPACE_BEGIN

class Accumulator
{
public:
    Accumulator() : n_(0), sum_(0.0), sum2_(0.0) {}

    /// Add a new value
    void operator<<(double value)
    {
        ++n_;
        sum_ += value;
        sum2_ += value * value;
    }

    /// Reset all values
    void clear()
    {
        n_ = 0;
        sum_ = 0.0;
        sum2_ = 0.0;
    }

    /// Compute mean
    double mean() const { return sum_ / n_; }

    /// Compute standard deviation
    double dev() const
    {
        double sc0 = double(n_) - 1.0;
        return std::sqrt(sum2_ / sc0 - sum_ * sum_ / sc0 / n_);
    }

    /// Compute standard error of the mean
    double std() const { return dev() / std::sqrt(n_); }

    /// Return number of samples
    uint64_t count() const { return n_; }

    /// Return accumulated sum
    double sum() const { return sum_; }

private:
    uint64_t n_;  ///< Number of samples
    double sum_;  ///< Accumulated sum
    double sum2_; ///< Accumulated squared sum
};

MIB_INTERNAL_NAMESPACE_END

/////////////////////////////////// ERROR //////////////////////////////////////

MIB_INTERNAL_NAMESPACE_BEGIN

struct Error
{
#if defined(MIB_NO_EXCEPTION)
    static MIB_NORETURN void error(const char* msg) noexcept 
    {
        std::cout << "mib: fatal error: " << msg << std::endl;
        std::abort();
    }
#else
    static void error(const char* msg) { throw std::runtime_error(msg); }
#endif
};

MIB_INTERNAL_NAMESPACE_END

////////////////////////////////// TERMINAL ////////////////////////////////////

MIB_INTERNAL_NAMESPACE_BEGIN

struct Terminal
{
    /// Change the terminal output color.
    class Color
    {
    public:
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
        Color(Code colorCode) noexcept
            : moved_(false)
        {
#if !defined(MIB_NO_COLOR) && defined(MIB_PLATFORM_WINDOWS)
            stdoutHandle_ = GetStdHandle(STD_OUTPUT_HANDLE);
            CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
            GetConsoleScreenBufferInfo(stdoutHandle_, &csbiInfo);
            foreAtrr_ = csbiInfo.wAttributes
                        & ~(BACKGROUND_GREEN | BACKGROUND_RED | BACKGROUND_BLUE
                            | BACKGROUND_INTENSITY);
            backAttr_ = csbiInfo.wAttributes
                        & ~(FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE
                            | FOREGROUND_INTENSITY);
#else
#endif

#if !defined(MIB_NO_COLOR)
            use(colorCode);
#endif
        }

        /// Copy constructor
        Color(const Color& other) noexcept
            : moved_(false)
        {
#if !defined(MIB_NO_COLOR)
            const_cast<Terminal::Color&>(other).setMoved(true);
#endif
        }

        /// Reset the console color to the state before.
        ~Color()
        {
#if !defined(MIB_NO_COLOR)
            if(!moved_)
                use(Terminal::Color::None);
#endif
        }

        /// Change color persistently
        void use(Code c) noexcept
        {
#if !defined(MIB_NO_COLOR) && defined(MIB_PLATFORM_WINDOWS)
            auto setAttr = [&](WORD textAttr) -> void {
                SetConsoleTextAttribute(stdoutHandle_, textAttr | backAttr_);
            };

            switch(c)
            {
            case Terminal::Color::None:
                setAttr(foreAtrr_);
                break;
            case Terminal::Color::White:
                setAttr(FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
                break;
            case Terminal::Color::Red:
                setAttr(FOREGROUND_RED);
                break;
            case Terminal::Color::Blue:
                setAttr(FOREGROUND_BLUE);
                break;
            case Terminal::Color::Green:
                setAttr(FOREGROUND_GREEN);
                break;
            case Terminal::Color::Magenta:
                setAttr(FOREGROUND_RED | FOREGROUND_BLUE);
                break;
            case Terminal::Color::Yellow:
                setAttr(FOREGROUND_RED | FOREGROUND_GREEN);
                break;
            case Terminal::Color::Cyan:
                setAttr(FOREGROUND_BLUE | FOREGROUND_GREEN);
                break;
            case Terminal::Color::Grey:
                setAttr(FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_RED
                    | FOREGROUND_BLUE);                
                break;
            case Terminal::Color::BoldWhite:
                setAttr(FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_RED
                    | FOREGROUND_BLUE);
                break;
            case Terminal::Color::BoldRed:
                setAttr(FOREGROUND_INTENSITY | FOREGROUND_RED);
                break;
            case Terminal::Color::BoldBlue:
                setAttr(FOREGROUND_INTENSITY | FOREGROUND_BLUE);
                break;
            case Terminal::Color::BoldGreen:
                setAttr(FOREGROUND_INTENSITY | FOREGROUND_GREEN);
                break;
            case Terminal::Color::BoldMagenta:
                setAttr(FOREGROUND_INTENSITY | FOREGROUND_RED
                    | FOREGROUND_BLUE);
                break;
            case Terminal::Color::BoldYellow:
                setAttr(FOREGROUND_INTENSITY | FOREGROUND_RED
                    | FOREGROUND_GREEN);
                break;
            case Terminal::Color::BoldCyan:
                setAttr(FOREGROUND_INTENSITY | FOREGROUND_BLUE
                    | FOREGROUND_GREEN);
                break;
            case Terminal::Color::BoldGrey:
                setAttr(FOREGROUND_INTENSITY);
                break;
            default: // Color::Bold
                assert("invalid color");
            }
#elif !defined(MIB_NO_COLOR) && defined(MIB_PLATFORM_POSIX)
            auto setAttr = [&](const char* escapeCode) -> void {
                std::cout << "\033" << escapeCode;
            };

            switch(c)
            {
            case Terminal::Color::None:
                setAttr("[0;39m");
                break;
            case Terminal::Color::White:
                setAttr("[0;37m");
                break;
            case Terminal::Color::Red:
                setAttr("[0;31m");
                break;
            case Terminal::Color::Green:
                setAttr("[0;32m");
                break;
            case Terminal::Color::Blue:
                setAttr("[0;34m");
                break;
            case Terminal::Color::Magenta:
                setAttr("[0;35m");
                break;
            case Terminal::Color::Yellow:
                setAttr("[0;33m");
                break;
            case Terminal::Color::Cyan:
                setAttr("[0;36m");
                break;
            case Terminal::Color::Grey:
                setAttr("[1;30m");
                break;
            case Terminal::Color::BoldWhite:
                setAttr("[1;37m");
                break;
            case Terminal::Color::BoldRed:
                setAttr("[1;31m");
                break;
            case Terminal::Color::BoldBlue:
                setAttr("[1;34m");
                break;
            case Terminal::Color::BoldGreen:
                setAttr("[1;32m");
                break;
            case Terminal::Color::BoldMagenta:
                setAttr("[1;35m");
                break;
            case Terminal::Color::BoldYellow:
                setAttr("[1;33m");
                break;
            case Terminal::Color::BoldCyan:
                setAttr("[1;36m");
                break;
            case Terminal::Color::BoldGrey:
                setAttr("[0;37m");
                break;
            default: // Color::Bold
                assert("invalid color");
            }
#else
#endif
        }

        /// Get the color usually associated with files. Returns BoldWhite
        /// on Windows/Linux and Grey on Mac OSX.
        static Color getFileColor() noexcept
        {
#ifdef MIB_PLATFORM_APPLE
            return Terminal::Color(Color::Grey);
#else
            return Terminal::Color(Color::BoldWhite);
#endif
        }

        /// Indicate whether the Color has been moved
        void setMoved(bool moved) { moved_ = moved; }

    private:
        bool moved_; ///< Indicate whether the object has been moved

#if !defined(MIB_NO_COLOR) && defined(MIB_PLATFORM_WINDOWS)
        HANDLE stdoutHandle_;
        WORD foreAtrr_;
        WORD backAttr_;
#endif
    };

    /// Get the current width of the terminal
    static int getWidth()
    {
        int width = 80; // default
#if defined(MIB_PLATFORM_WINDOWS)
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
        width = (int)(csbi.srWindow.Right - csbi.srWindow.Left + 1);
#elif defined(MIB_PLATFORM_POSIX)
        struct winsize ws;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
        width = (int)ws.ws_col;
#endif
        return (width == 0 ? 80 : width);
    }
};

/// Allow usage in streams
MIB_INLINE std::ostream& operator<<(std::ostream& os, const Terminal::Color&)
{
    return os;
}

using Color = Terminal::Color;

MIB_INTERNAL_NAMESPACE_END

/////////////////////////////////// TIMER //////////////////////////////////////

MIB_INTERNAL_NAMESPACE_BEGIN

struct rdtsc_struct
{
#if defined(MIB_PLATFORM_WINDOWS)
    double cycle() const { return double(ticks); }
    unsigned __int64 ticks;
#else
    double cycle() const { return (uint64_t(hi) << 32) + lo; }
    uint32_t hi;
    uint32_t lo;
#endif
};

MIB_INLINE double operator-(const rdtsc_struct& end, const rdtsc_struct& begin)
{
    return end.cycle() - begin.cycle();
}

class Timer
{
public:
    using chrono_clock = std::chrono::high_resolution_clock;

	/// Start timer
    MIB_INLINE void start() noexcept
    {
        chronoStart_ = chrono_clock::now();
#if defined(MIB_PLATFORM_WINDOWS)
        cycStart_.ticks = __rdtsc();
#else
        asm volatile("cpuid"
                     :
                     : "a"(0)
                     : "bx", "cx", "dx"); // CPUID exec-barrier
        asm volatile("rdtsc"
                     : "=a"((cycStart_).lo), "=d"((cycStart_).hi)); // RDTSC
#endif
    }

	/// Stop timer
    MIB_INLINE void stop() noexcept
    {
#if defined(MIB_PLATFORM_WINDOWS)
        cycStop_.ticks = __rdtsc();
#else
        asm volatile("rdtsc"
                     : "=a"((cycStop_).lo), "=d"((cycStop_).hi)); // RDTSC
        asm volatile("cpuid"
                     :
                     : "a"(0)
                     : "bx", "cx", "dx"); // CPUID exec-barrier
#endif
        chronoStop_ = chrono_clock::now();
    }

    double cycles() const noexcept { return cycStop_ - cycStart_; }

    double sec() const noexcept
    {
        using sec_dur = std::chrono::duration<double, std::ratio<1, 1>>;
        auto res
            = std::chrono::duration_cast<sec_dur>(chronoStop_ - chronoStart_);
        return res.count();
    }

    double usec() const noexcept
    {
        using usec_dur = std::chrono::duration<double, std::ratio<1, 1000000>>;
        auto res
            = std::chrono::duration_cast<usec_dur>(chronoStop_ - chronoStart_);
        return res.count();
    }

    double nsec() const noexcept
    {
        using usec_dur
            = std::chrono::duration<double, std::ratio<1, 1000000000>>;
        auto res
            = std::chrono::duration_cast<usec_dur>(chronoStop_ - chronoStart_);
        return res.count();
    }

private:
    rdtsc_struct cycStart_, cycStop_;
    chrono_clock::time_point chronoStart_, chronoStop_;
};

MIB_INTERNAL_NAMESPACE_END

////////////////////////////////// PROFILER ////////////////////////////////////

MIB_INTERNAL_NAMESPACE_BEGIN

struct Node
{
    using PairType = std::pair<double, double>;

    /// Construct an empty Node
    Node()
        : name(""),
          parent(nullptr),
          currentRun(PairType(0, 0))
    {
    }

    std::string name;
    Node* parent;
    PairType currentRun;

    std::set<Node*> children;
    Accumulator acc[2];
};

/// Return the instance of the global timer
MIB_INLINE Timer& getTimer()
{
    static Timer t;
    return t;
}

class Profiler
{
    using PairType = Node::PairType;

    enum class TimerMode : uint8_t
    {
        cycle = 0,
        nsec = 1
    };

    TimerMode mod_ = TimerMode::cycle;
    Node sentinel_;
    std::vector<Node*> stack_;
    std::map<std::string, Node> tree_;

    std::ostream& outStream = std::cout;

public:    
    Timer& timer_; ///< Refrence to global timer

    /// Default constructor
    Profiler() : timer_(getTimer())
    {
        sentinel_.name = "__Sentinel_root__";
        stack_.push_back(&sentinel_);
    }

    Profiler(const Profiler&) = delete;

    /// Start profiling process
    MIB_INLINE void start(std::string const &name)  
    { 
        processStart(name, getCycle()); 
    }

    /// Stop the currently running process and launch a new one
    MIB_INLINE void next(std::string const &name) 
    {
        processStop(stack_.back()->name, getCycle());
        processStart(name, PairType(0, 0));
    }

    /// Stop the current profiling process
    MIB_INLINE void stop(std::string const &name) 
    { 
        processStop(name, getCycle()); 
    }

    /// Set the Mode of the timer (cycle or nsec)
    void setTimerMode(const std::string& smod = "cycle")
    {
        if(smod == "cycle")
            mod_ = TimerMode::cycle;
        else if(smod == "nsec")
            mod_ = TimerMode::nsec;
        else
        {
            std::stringstream ss;
            ss << "Profiler: invalid mode '" << smod << "'";
            Error::error(ss.str().c_str());
        }
    }
    
    /// Print result to console
    void printTree() const
    {
        int width = Terminal::getWidth();

#if defined(MIB_PLATFORM_WINDOWS)
        width--;
#endif
        auto prec = std::cout.precision();

        std::cout << Color(Color::BoldGreen) << std::string(width, '=') 
                  << std::endl;
                          
        std::vector<bool> marker;
        std::string timerModeStr
            = mod_ == TimerMode::cycle ? "  cycle_mean" : "   nsec_mean";
        
#if defined(MIB_UTF8)
        outStream << "┬ (% of parent) name"
#else
        outStream << "  (% of parent) name"
#if defined(MIB_NO_COLOR)
                  << " "
#endif
#endif
                  << std::string(width - 22 - 33, ' ')  
                  << timerModeStr << std::string(3, ' ')
                  << "spread" << std::string(4, ' ')
                  << "run_cnt" << std::endl;

        auto i = sentinel_.children.size();
        auto mod = int(mod_);

#if defined(MIB_PLATFORM_WINDOWS)
        width--;
#endif
        for(auto const& p : sentinel_.children)
        {
            --i;
            printTreeHelper(p, i == 0, marker, mod ? sentinel_.currentRun.second
                                                   : sentinel_.currentRun.first,
                            p->acc[mod].mean() * p->acc[mod].count(), width);
        }
        outStream.precision(prec);
        outStream.flush();
    }

private:
    MIB_INLINE PairType getCycle() noexcept
    {
        auto cyc = timer_.cycles();
        auto nsec = timer_.nsec();
        timer_.start();
        timer_.stop();
        cyc -= timer_.cycles();
        return PairType(cyc, nsec);
    }

    void processStart(const std::string& name, const PairType& msm)
    {
        static bool firstStart = true;
        if(firstStart)
            firstStart = false;
        else
            for(auto ptr : stack_)
            {
                ptr->currentRun.first += msm.first;
                ptr->currentRun.second += msm.second;
            }

        tree_[name].name = name;
        tree_[name].parent = stack_.back();
        stack_.push_back(&tree_[name]);
        tree_[name].parent->children.insert(&tree_[name]);
    }

    void processStop(const std::string& name, const PairType& msm)
    {
        if(name != stack_.back()->name)
        {
            std::stringstream ss;
            ss << "Profiler: stopped '" << name << "' instead of '"
               << stack_.back()->name << "'";
            Error::error(ss.str().c_str());
        }

        for(auto ptr : stack_)
        {
            ptr->currentRun.first += msm.first;
            ptr->currentRun.second += msm.second;
        }

        auto node = stack_.back();
        node->acc[0] << node->currentRun.first;
        node->acc[1] << node->currentRun.second;
        node->currentRun = PairType(0, 0);
        stack_.pop_back();
    }

    void printTreeHelper(const Node* parent,
                         const bool lastchild,
                         std::vector<bool>& marker,
                         const double& parent_time,
                         const double& self_time,
                         int width) const noexcept
    {
        auto mod = int(mod_);
        for(auto c : marker)
        {
            if(!c)
#if defined(MIB_UTF8)
                outStream << "│" << std::string(4 - 1, ' ');
#else
                outStream << " " << std::string(4 - 1, ' ');
#endif
            else
                outStream << std::string(4, ' ');
        }
       
        int percentage = std::lround(100 * self_time / parent_time);
        double spread
            = 100 * (parent->acc[mod].std() / parent->acc[mod].mean());

#if defined(MIB_NO_COLOR)
        int colorWs = 1;
#else
        int colorWs = 2;
#endif

        int whitespace = width - 34 - 4 * (int) marker.size()
                         - (int) parent->name.size() - colorWs - 5
                         - (percentage == 100);

        if(whitespace < 0)
            whitespace = 0;

#if defined(MIB_UTF8)
        outStream << (lastchild ? "└" : "├") << "─" << std::setw(colorWs);
#else
        outStream << " -" << std::setw(colorWs);
#endif
        // Percentage / Name
        outStream << Color(Color::getFileColor())
                  << std::string(2 - colorWs, ' ')
                  << (percentage < 10 ? " " : "") << percentage << "% ";
        outStream << parent->name;
        // Mean
        outStream << std::string(whitespace, ' ') << "[" << std::fixed 
                  << std::setprecision(parent->acc[mod].mean() < 10 ? 1 : 0)
                  << std::right << std::setw(10)
                  << parent->acc[mod].mean();
        // Spread
        outStream << std::right << std::setw(8);
        if(std::isnan(spread))
            outStream << "- " << std::right << std::setw(12);  
        else
            outStream << spread << "%" << std::right << std::setw(11);  
              
        // Run count 
        outStream << parent->acc[mod].count() << " ]" << std::endl;

        auto i = parent->children.size();
        marker.push_back(lastchild);
        for(auto const& p : parent->children)
        {
            --i;
            printTreeHelper(p, i == 0, marker, self_time,
                            double(p->acc[mod].mean() * p->acc[mod].count()),
                            width);
        }
        marker.pop_back();
  }
};

/// Return the instance of the global profiler
MIB_INLINE Profiler& getProfiler()
{
    static Profiler p;
    return p;
}

static auto& MIB_UNUSED mib_Profiler = getProfiler();

MIB_INTERNAL_NAMESPACE_END

#endif // MIB_OFF

#endif // __INCLUDED_MIB_H__
