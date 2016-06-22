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

from IsenPython import Solver, NameList, Output, ArchiveType
from IsenPython.Visualizer import Visualizer

def plotSimulation(filename, time):
    """ Read simulation from file and plot it """
    output = Output(filename)
    visualizer = Visualizer(output)
    visualizer.zlim = [0, 15]
    visualizer.plot('horizontal_velocity', time, file=output.getNameList().run_name + '.pdf')
    
def runSimulation(nx, dt):
    """ Run simulation and write it to file """
    # Set namelist
    namelist = NameList()
    namelist.iprtcfl = False
    
    namelist.nx = nx
    namelist.run_name = "100h-simulation-" + str(namelist.nx)

    namelist.iout = 36000 # 10h
    namelist.dt = dt

    namelist.time = 10 * namelist.iout # 100h

    print(namelist)

    # Run simulation
    solver = Solver('cpu')
    solver.init(namelist)

    solver.run()
    solver.write(ArchiveType.Binary)

def main():
    runSimulation(100, 1)
#    runSimulation(1000, 0.1)
#    runSimulation(5000, 0.02)
#    plotSimulation("100h-simulation-1000.bin", 10)
#    plotSimulation("100h-simulation-1000.bin", 10)    
#    plotSimulation("100h-simulation-1000.bin", 10)

if __name__ == '__main__':
    main()

