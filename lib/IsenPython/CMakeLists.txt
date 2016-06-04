#                        _________ _______   __
#                       /  _/ ___// ____/ | / /
#                       / / \__ \/ __/ /  |/ / 
#                     _/ / ___/ / /___/ /|  /  
#                    /___//____/_____/_/ |_/   
#            
#  Isentropic model - ETH Zurich
#  Copyright (C) 2016  Fabian Thuering (thfabian@student.ethz.ch)
# 
#  This file is distributed under the MIT Open Source License. See
#  LICENSE.TXT for details.

cmake_minimum_required(VERSION 2.8)

set(ISEN_PYTHON_SOURCE 
    IsenPython.cpp
    PySolver.cpp
    PyLogger.cpp
    PyNameList.cpp
    PyOutput.cpp
    )

set(ISEN_PYTHON_HEADER
    ${ISEN_INCLUDE_DIR}/Isen/Python/IsenPython.h
    ${ISEN_INCLUDE_DIR}/Isen/Python/PySolver.h
    ${ISEN_INCLUDE_DIR}/Isen/Python/PyLogger.h
    ${ISEN_INCLUDE_DIR}/Isen/Python/PyType.h
    ${ISEN_INCLUDE_DIR}/Isen/Python/PyNameList.h
    ${ISEN_INCLUDE_DIR}/Isen/Python/PyOutput.h
    )
    
set(ISEN_PYTHON_PYFILES
    ${CMAKE_SOURCE_DIR}/python/IsenPython/__init__.py 
    ${CMAKE_SOURCE_DIR}/python/IsenPython/Visualizer.py       
    )

source_group("Python" FILES ${ISEN_PYTHON_PYFILES})  
 
## Add shared library
add_library(IsenPython SHARED ${ISEN_PYTHON_SOURCE} 
                              ${ISEN_PYTHON_HEADER}
                              ${ISEN_HEADERS})

target_link_libraries(IsenPython ${ISEN_LIBRARIES} 
                                 ${Boost_LIBRARIES}
                                 ${PYTHON_LIBRARIES})

# Select the correct output name of the library
if(WIN32)
    set_target_properties(IsenPython PROPERTIES 
                          PREFIX "" 
                          SUFFIX ".pyd"
                          OUTPUT_NAME "IsenPythonCxx")
else(WIN32)
    set_target_properties(IsenPython PROPERTIES 
                          PREFIX "" 
                          SUFFIX ".so"
                          OUTPUT_NAME "IsenPythonCxx")
endif(WIN32)

## Copy pure python code to python build dir
add_custom_target(IsenPythonCopy
                  COMMAND ${CMAKE_COMMAND} -E copy_directory
                  ${CMAKE_SOURCE_DIR}/python
                  ${CMAKE_BINARY_DIR}/python
                  DEPENDS ${ISEN_PYTHON_PYFILES}
                  )
add_dependencies(IsenPython IsenPythonCopy)
                   
## Copy library to python build dir
add_custom_command(TARGET IsenPython POST_BUILD
           COMMAND ${CMAKE_COMMAND} -E copy
           $<TARGET_FILE:IsenPython> 
           ${CMAKE_BINARY_DIR}/python/IsenPython/$<TARGET_FILE_NAME:IsenPython>)
         
# -- TEMPORARY --- This copies the library into the source tree         
add_custom_command(TARGET IsenPython POST_BUILD
           COMMAND ${CMAKE_COMMAND} -E copy
           $<TARGET_FILE:IsenPython> 
           ${CMAKE_SOURCE_DIR}/python/IsenPython/$<TARGET_FILE_NAME:IsenPython>)
           
## Installation
install(TARGETS IsenPython LIBRARY DESTINATION ${CMAKE_SYSTEM_NAME}/python/IsenPython)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/python DESTINATION ${CMAKE_SYSTEM_NAME})

