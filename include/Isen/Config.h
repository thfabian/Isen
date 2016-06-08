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
#ifndef ISEN_CONFIG_H
#define ISEN_CONFIG_H

#define _QUOTE_IMPL(S) #S
#define _STR_IMPL(S) _QUOTE_IMPL(S)

// Version information
#define ISEN_VERSION_MAJOR 0
#define ISEN_VERSION_MINOR 0
#define ISEN_VERSION_PATCH 1
#define ISEN_VERSION_STRING                                                                                            \
    _STR_IMPL(ISEN_VERSION_MAJOR) "." _STR_IMPL(ISEN_VERSION_MINOR) "." _STR_IMPL(ISEN_VERSION_PATCH)

#define ISEN_NAMESPACE_BEGIN namespace Isen {
#define ISEN_NAMESPACE_END }

// Define platform
#if defined(_MSC_VER) || defined(_WIN32) || defined(_WIN64)
#define ISEN_PLATFORM_WINDOWS 1
#elif defined(__linux__) || defined(__linux)
#define ISEN_PLATFORM_LINUX 1
#elif defined(__APPLE__)
#define ISEN_PLATFORM_APPLE 1
#endif

#if defined(__unix__) || defined(ISEN_PLATFORM_APPLE)
#define ISEN_PLATFORM_POSIX 1
#endif

// Define compiler
#if defined(__clang__)
#define ISEN_COMPILER_CLANG 1
#endif

#if defined(__ICC) || defined(__INTEL_COMPILER)
#define ISEN_COMPILER_INTEL 1
#endif

#if defined(__GNUC__) || defined(__GNUG__)
#define ISEN_COMPILER_GNU 1
#endif

#if defined(_MSC_VER)
#define ISEN_COMPILER_MSVC 1
#endif

// INLINE compiler intrinsic
#if defined(ISEN_COMPILER_GNU)
#define ISEN_INLINE inline __attribute__((always_inline))
#elif defined(ISEN_COMPILER_MSVC)
#define ISEN_INLINE inline __forceinline
#else
#define ISEN_INLINE inline
#endif

// NO_INLINE compiler intrinsic
#if defined(ISEN_COMPILER_GNU)
#define ISEN_NO_INLINE __attribute__((noinline))
#elif defined(ISEN_COMPILER_MSVC)
#define ISEN_NO_INLINE __declspec(noinline)
#else
#define ISEN_NO_INLINE
#endif

// NORETURN compiler intrinsic
#if defined(ISEN_COMPILER_MSVC)
#define ISEN_NORETURN __declspec(noreturn)
#elif defined(ISEN_COMPILER_GNU)
#define ISEN_NORETURN __attribute__((noreturn))
#else
#define ISEN_NORETURN
#endif

// UNUSED compiler instrinsic
#if defined(ISEN_COMPILER_GNU)
#define ISEN_UNUSED __attribute__((unused))
#else
#define ISEN_UNUSED
#endif

// RESTRICT keyword
#if defined(ISEN_COMPILER_MSVC)
#define ISEN_RESTRICT __restrict
#elif defined(ISEN_COMPILER_GNU)
#define ISEN_RESTRICT __restrict__
#else
#define ISEN_RESTRICT
#endif

// VECTORIZE_LOOP compiler hint
#if defined(ISEN_COMPILER_MSVC)
#define ISEN_VECTORIZE_LOOP __pragma(loop(ivdep))
#elif defined(ISEN_COMPILER_INTEL)
#define ISEN_VECTORIZE_LOOP _Pragma("simd")
#elif defined(ISEN_COMPILER_CLANG)
#define ISEN_VECTORIZE_LOOP _Pragma("clang loop vectorize(enable)")
#elif defined(ISEN_COMPILER_GNU)
#define ISEN_VECTORIZE_LOOP _Pragma("simd") 
#else
#define ISEN_VECTORIZE_LOOP
#endif

// Exclude unnecessary APIs / Macros on Windows
#ifdef ISEN_PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN 1
#define NOMINMAX 1
#define _CRT_SECURE_NO_WARNINGS 1
#define _SCL_SECURE_NO_WARNINGS 1
#endif

// Ignore some annoying warnings
#ifdef ISEN_PLATFORM_WINDOWS
#pragma warning(disable : 4267) // conversion from 'size_t' to 'int'
#pragma warning(disable : 4244) // conversion from 'double' to 'float'
#pragma warning(disable : 4305) // truncation from 'double' to 'float'
#endif

// We want variadic macro support from the Boost.Preprorccessor library.
// However, in older Boost versions Clang is not listed to support such macros due to lack of "testing".
#ifdef ISEN_COMPILER_CLANG
#define BOOST_PP_VARIADICS 1
#endif

// Define number of threads for Windows (assume 4 cores)
#ifdef ISEN_PLATFORM_WINDOWS
#define ISEN_NUM_THREADS 4
#endif

#endif
