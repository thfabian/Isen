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

add_executable(isen main.cpp)
target_link_libraries(isen ${ISEN_LIBRARIES} 
                           ${Boost_LIBRARIES}
                           ${PYTHON_LIBRARIES})
install(TARGETS isen RUNTIME DESTINATION ${CMAKE_SYSTEM_NAME})
