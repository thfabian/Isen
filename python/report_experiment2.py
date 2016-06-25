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

import numpy as np
import matplotlib.pyplot as plt

def plotSimulation(filename100, filename1000, time):
    """ Plot total precipitation of 100 and 1000 simulations """
    output100 = Output(filename100)
    output1000 = Output(filename1000)
    
    visualizer100 = Visualizer(output100)
    visualizer1000 = Visualizer(output1000)
    
    fig = plt.figure(figsize=(6.5, 5.25))
    fig.subplots_adjust(left = 0.1, bottom = 0.12, right = 0.97, top = 0.96)
    ax = fig.add_subplot(1, 1, 1)
    plt.sca(ax)
    ax.cla()
    
    max_prec = 2
    
    ax.set_ylabel('Accumulated Rain [mm]')
    ax.set_xlabel('x [km]')
            
    ax.set_ylim([0, max_prec])
    ax.set_xlim(visualizer100.xlim)
    
    ax.plot(visualizer100.x[:, 0], 
            0.1 * max_prec * visualizer100.topo[time, :] / max(visualizer100.topo[time, :]), 
            linewidth=2, color='k')
    
    ax.plot(visualizer100.x[:, 0], visualizer100.tot_prec[time, :], 'r-', label=r"5 km")
    ax.plot(visualizer1000.x[:, 0], visualizer1000.tot_prec[time, :], 'g-', label=r"500 m")
    
    plt.legend(frameon=False)
    plt.title('Time = {0} seconds'.format(int(visualizer100.output.t()[time])))
    plt.tight_layout()

    plt.savefig("total-precipitation.pdf")
    plt.show()
    
def runSimulation(nx, dt):
    """ Run simulation and write it to file """
    # Set namelist
    namelist = NameList()
    namelist.iprtcfl = False
    namelist.imoist = True
    namelist.imoist_diff = True
    namelist.imicrophys = 1
        
    namelist.nx = nx
    namelist.run_name = "60h-simulation-moist-" + str(namelist.nx)

    outsteps = 36000 / dt
    namelist.iout = int(outsteps) # 10h
    namelist.dt = dt

    namelist.time = 6 * namelist.iout # 60h

    print(namelist)

    # Run simulation
    solver = Solver('cpu')
    solver.init(namelist)

    solver.run()
    solver.write(ArchiveType.Binary)

def main():
#    runSimulation(100, 1)
#    runSimulation(1000, 0.1)
    plotSimulation("60h-simulation-moist-100.bin", "60h-simulation-moist-1000.bin", 6)

if __name__ == '__main__':
    main()

