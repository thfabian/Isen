#                        _________ _______   __
#                       /  _/ ___// ____/ | / /
#                       / / \__ \/ __/ /  |/ / 
#                     _/ / ___/ / /___/ /|  /  
#                    /___//____/_____/_/ |_/   
#            
#  Isentropic model - ETH Zurich
#  Copyright (C) 2016  Fabian Thuering (thfabian@student.ethz.ch)
# 
#  This file is distributed under the MIT Open Source License. See LICENSE.TXT for details.

cmake_minimum_required(VERSION 2.8)

set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/bin/" CACHE PATH "CMake installation prefix")

project(Isen)

if(NOT(CMAKE_MAJOR_VERSION LESS 3) AND NOT(CMAKE_MINOR_VERSION LESS 2))
    cmake_policy(SET CMP0054 OLD)
endif()

########################################################################################################################
# Compiler Flags
########################################################################################################################

# Build Python bindings?
option(ISEN_PYTHON  "Build python bindings for Isen (requires Boost.Python and Python/Numpy)" OFF)

# Use static runtime?
option(ISEN_LINK_STATIC "Statically link against the C++/Boost runtime library" ${MSVC})
       
if(${ISEN_PYTHON})
    message(STATUS "Boost.Python requires dynamic runtime")
    set(ISEN_LINK_STATIC OFF)
endif(${ISEN_PYTHON})

# Use certain flags to enhance profilerexperience
option(ISEN_BENCHMARK "Use certain flags (-fno-omit-frame-prointer) for improved profiling" OFF)

# Report on SIMD vectorization
option(ISEN_VEC_REPORT "Optimization reports on vectorization" OFF)

if(NOT(${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC"))
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -march=native -Wall")
    
    # Disable some unavoidable warnings, mainly from boost
    if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} "
                            "-Wno-sign-compare"
                            "-Wno-unused-function"
                            # Eigen3
                            "-Wno-enum-compare"
                            # Boost.Python
                            "-Wno-unused-local-typedefs"
                            )
        string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} "
                            "-Wno-sign-compare"
                            "-Wno-unused-function"
                            "-Wno-ignored-qualifiers"
                            # Eigen3
                            "-Wno-deprecated-declarations"
                            # Boost.Python
                            "-Wno-unused-local-typedefs"
                            )
        string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -wd1478 -simd")
        
        # Report on vectorization
        if(ISEN_VEC_REPORT)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -qopt-report-phase=vec -qopt-report5 -qopt-report-per-object")
        endif(ISEN_VEC_REPORT)
    endif()
    
    # Produce position independent code
    if(NOT ISEN_LINK_STATIC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
    endif(NOT ISEN_LINK_STATIC)
else()
    if(ISEN_LINK_STATIC)
        # We will link statically against the C++ runtime library on Windows. 
        # The behaviour can be altered with ISEN_LINK_STATIC=OFF
        set(CompilerFlags
            CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
            CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO
            CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
            CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO)
        foreach(CompilerFlag ${CompilerFlags})
            string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
            string(REPLACE "/MDd" "/MTd" ${CompilerFlag} "${${CompilerFlag}}")
        endforeach()
    else(ISEN_LINK_STATIC)
        # Instruct Boost to not perform auto-link
        add_definitions(-DBOOST_ALL_NO_LIB)
        add_definitions(-DBOOST_ALL_DYN_LINK)
    endif(ISEN_LINK_STATIC)
endif()

if(ISEN_BENCHMARK)
    if(NOT(${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC"))
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer")
    endif(NOT(${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC"))
endif(ISEN_BENCHMARK)

########################################################################################################################
# CMake options
########################################################################################################################
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Set build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING
        "Choose the type of build, options are: Debug Release" FORCE)
    message(STATUS "Setting build type to 'Release' as none was specified")
endif(NOT CMAKE_BUILD_TYPE)

# Set runtime paths 
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_SYSTEM_NAME}")
set(CMAKE_MACOSX_RPATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_SYSTEM_NAME}")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# Inlcude all Find*.cmake modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

########################################################################################################################
# Boost (required)
########################################################################################################################
set(Boost_USE_STATIC_LIBS ${ISEN_LINK_STATIC}) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME ${ISEN_LINK_STATIC})
    
if(ISEN_PYTHON)
    add_definitions(-DISEN_PYTHON)

    # Python2
    find_package(PythonLibs 2 REQUIRED)
    include_directories(${PYTHON_INCLUDE_DIRS})

    # Numpy
    find_package(NumPy REQUIRED)
    include_directories(${NUMPY_INCLUDE_DIRS})

    find_package(Boost COMPONENTS program_options filesystem system regex python serialization REQUIRED)
else(ISEN_PYTHON)
    find_package(Boost COMPONENTS program_options filesystem system regex serialization REQUIRED)
endif(ISEN_PYTHON)

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

########################################################################################################################
# Find Eigen3 (required)
########################################################################################################################
if(NOT(EIGEN3_INCLUDE_DIR))
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/ext/eigen3)
        set(EIGEN3_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ext/eigen3")
    endif()
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/ext/eigen)
        set(EIGEN3_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ext/eigen")
    endif()    
endif(NOT(EIGEN3_INCLUDE_DIR))

find_package(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR})

########################################################################################################################
# Doxygen (optional, needed to build the documentation)
########################################################################################################################
find_package(Doxygen)
option(ISEN_DOCUMENTATION "Create and install the HTML based API documentation (requires Doxygen)" ${DOXYGEN_FOUND})
set(DOC_DIR ${PROJECT_SOURCE_DIR}/doc)                
                     
########################################################################################################################
# Compilation
########################################################################################################################

# External includes
include_directories(${PROJECT_SOURCE_DIR}/ext/catch)
include_directories(${PROJECT_SOURCE_DIR}/ext/mib)

# Isen
set(ISEN_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
include_directories(${ISEN_INCLUDE_DIR})
set(ISEN_LIBRARIES IsenCore)

add_subdirectory(${PROJECT_SOURCE_DIR}/lib/IsenCore)
add_subdirectory(${PROJECT_SOURCE_DIR}/lib)
add_subdirectory(${PROJECT_SOURCE_DIR}/test)

if(DOXYGEN_FOUND)
    add_subdirectory(${PROJECT_SOURCE_DIR}/doc)
endif(DOXYGEN_FOUND)
    
if(ISEN_PYTHON)
    add_subdirectory(${PROJECT_SOURCE_DIR}/lib/IsenPython)
endif(ISEN_PYTHON)

