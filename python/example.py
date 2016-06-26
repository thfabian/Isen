# -*- coding: utf-8 -*-
#                        _________ _______   __
#                       /  _/ ___// ____/ | / /
#                       / / \__ \/ __/ /  |/ / 
#                     _/ / ___/ / /___/ /|  /  
#                    /___//____/_____/_/ |_/   
#            
#  Isentropic model - ETH Zürich
#  Copyright (C) 2016  Fabian Thüring (thfabian@student.ethz.ch)
# 
#  This file is distributed under the MIT Open Source License. See LICENSE.TXT for details.
#

from __future__ import print_function, division

from IsenPython import Solver, NameList, Output
from IsenPython.Visualizer import Visualizer

# Setup Namelist
namelist = NameList()
namelist.ishear = True
namelist.nx = 100

# Setup & run Solver
solver = Solver()
solver.init(namelist)

solver.run()

# Plot horizontal velocity distribution
output = solver.getOutput()
visualizer = Visualizer(output)
visualizer.plot('horizontal_velocity', 6)
