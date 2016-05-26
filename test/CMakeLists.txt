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
#

file(GLOB ISEN_TEST_SOURCE "*.cpp")
set(ISEN_TEST_HEADER Test.h FileProxy.h FieldLoader.h FieldVerifier.h)
    
# Build tests
add_executable(isen_test ${ISEN_TEST_SOURCE} ${ISEN_TEST_HEADER})
target_link_libraries(isen_test ${ISEN_LIBRARIES} 
                                ${Boost_LIBRARIES}
                                ${PYTHON_LIBRARIES})
                                
# Copy test data
add_custom_target(isen_test_data
                  COMMAND ${CMAKE_COMMAND} -E copy_directory
                  ${CMAKE_SOURCE_DIR}/test/data
                  ${CMAKE_BINARY_DIR}/data
                  )
add_dependencies(isen_test isen_test_data)
