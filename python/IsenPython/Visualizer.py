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

import numpy as np
import matplotlib 
import matplotlib.pyplot as plt
from matplotlib.ticker import MultipleLocator

import sys
import argparse

class Visualizer:
    """Visualize output of simulation """
        
    def __init__(self, output):
        """Initalize vizualizer methods 
        
        Args:
            output: IsenPython.Output instance containing the data.
        """
        matplotlib.rcParams.update({'font.size': 11})

        self.__output = output
        namelist = output.getNameList()
        
        # Number of timesteps
        nt = len(self.__output.t())
        nz = namelist.nz
        nx = namelist.nx
        
        # Plotting variables
        self.xlim = [0, (namelist.nx - 1) * namelist.dx / 1000.]
        self.zlim = [0, 10]
        self.title = namelist.run_name
        
        self.tci = 2 # Theta contour interval (K)
        self.vci = 2 # Velocity contour interval (m/s)
        
        dth = namelist.thl / nz
        self.tlim = [namelist.th00 + dth / 2., 400.];
        self.vlim = [0., 60.]
        
        # X-axis
        self.__x = np.tile(np.array(range(0, namelist.nx)) * namelist.dx / 1000., [nz, 1]).transpose();

        # Destagger height
        z = self.__output.z() / 1000.
        self.__z = np.zeros((nt, nx, nz))
        for t in xrange(0, nt):
            for i in xrange(0, nx):
                    self.__z[t, i, 0:nz] = 0.5 * (z[t, i, 0:nz] + z[t, i, 1:nz+1]) 
        
        # Theta value between the coordinate surfaces
        theta = np.zeros((namelist.nz))
        for i in xrange(len(theta)):
            theta[i] = namelist.th00 + (i + 0.5) * dth
        self.__theta = np.tile(theta,[namelist.nx, 1])
        
        # Topography height
        topo = z[:, :, 0] / 1000.
        self.__topo = topo.reshape(nx, nt);
        
    def __makeFigure(self):
        fig = plt.figure(figsize=(6, 5))
        fig.subplots_adjust(left = 0.1, bottom = 0.12, right = 0.97, top = 0.96)
        ax = fig.add_subplot(1, 1, 1)
        return fig, ax
        
    def plotVelocityContour(self, timeIndex):
        """Generate velocitry contour plot
        
        Args:
            timeIndex: Specify which data set to use.
        """
        fix, ax = self.__makeFigure()
        
        plt.sca(ax)
        ax.cla()

        ax.xaxis.set_minor_locator(MultipleLocator(50))
        ax.yaxis.set_minor_locator(MultipleLocator(1))

        # Label axis
        ax.set_xlabel('x [km]')
        ax.set_ylabel('Height [km]')
        
        # Set axis
        ax.set_ylim([self.zlim[0], 0.1 + self.zlim[1]])
        ax.set_yticks(np.arange(self.zlim[0], self.zlim[1] + 1, 1))

        ax.set_xlim(self.xlim)

        clev = np.arange(self.tlim[0], self.tlim[1], self.tci)
        
        # Plot theta
        ax.contour(self.__x, self.__z[timeIndex, :, :], self.__theta, clev, colors='grey', linewidths=1)
        
        plt.title('Time = {0} seconds'.format(self.__output.t()[timeIndex]))
        plt.tight_layout()
        plt.show()
        
if __name__ == '__main__':
    pass
