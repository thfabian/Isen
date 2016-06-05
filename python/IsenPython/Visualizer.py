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
import inspect

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
        
        # NameList parameters
        self.nt = len(self.__output.t())
        self.nz = namelist.nz
        self.nx = namelist.nx
        self.u00 = namelist.u00
        
        # Plotting variables
        self.xlim = [0, (self.nx - 1) * namelist.dx / 1000.]
        self.zlim = [0, 10]
        self.title = namelist.run_name
        
        self.tci = 2 # Theta contour interval (K)
        self.vci = 2 # Velocity contour interval (m/s)
        
        dth = namelist.thl / self.nz
        self.tlim = [namelist.th00 + dth / 2., 400.];
        self.vlim = [0., 60.]
        
        # X-axis
        self.__x = np.tile(np.array(range(0, self.nx)) * namelist.dx / 1000., [self.nz, 1]).transpose();

        # Geometric height (destaggered)
        z = self.__output.z() / 1000.
        self.__z = np.zeros((self.nt, self.nx, self.nz))
        for t in xrange(0, self.nt):
            for i in xrange(0, self.nx):
                    self.__z[t, i, 0:self.nz] = 0.5 * (z[t, i, 0:self.nz] + z[t, i, 1:self.nz+1])
        
        # Theta value between the coordinate surfaces
        theta = np.zeros((self.nz))
        for i in xrange(len(theta)):
            theta[i] = namelist.th00 + (i + 0.5) * dth
        self.__theta = np.tile(theta,[self.nx, 1])
        
        # Topography height
        self.__topo = z[:, :, 0]
        
        # Velocity (destaggered)
        self.u = self.__output.u()
        
    def __makeFigure(self):
        fig = plt.figure(figsize=(6.5, 5.25))
        fig.subplots_adjust(left = 0.1, bottom = 0.12, right = 0.97, top = 0.96)
        ax = fig.add_subplot(1, 1, 1)
        return fig, ax
        
    def __generateDictonary(self, name):
        if name.lower() == 'horizontal_velocity':
            vclev1 = np.arange(self.vlim[0], self.u00, self.vci)
            vclev2 = np.arange(self.u00 + self.vci, self.vlim[1] + self.vci, self.vci)
            
            plotDict= dict(fmt = '%2i',
                           clev = np.array(list(vclev1) + [self.u00] + list(vclev2)),
                           color = ['lime'] * len(vclev1) + ['k'] + ['r'] * len(vclev2),
                           scale = 1,
                           data = 'u') 
            return plotDict
        
        raise RuntimeError("Visualizer: invalid plot name '{0}'")
        
    def plot(self, name, timestep, file = None):
        """Generate velocitry contour plot
        
        Args:
            name: Name of the plot to generate:
                - horizontal_velocity
            timestep: Specify which data set to use.
            file: Save to file if not None
        """
        if timestep > self.nt:
            raise RuntimeError("Visualizer: requested timestep '{0}' exceeds available timesteps '{1}'".format(
                  timestep, self.nt - 1))
        
        fig, ax = self.__makeFigure()
        
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
                
        # Plot theta
        clev = np.arange(self.tlim[0], self.tlim[1], self.tci)
        ax.contour(self.__x, self.__z[timestep, :, :], self.__theta, clev, colors='grey', linewidths=1)
        
        # Plot topography
        ax.plot(self.__x[:, 0], self.__topo[timestep, :], linewidth=2, color='k');
        
        # Plot quantity 'name'
        pd = self.__generateDictonary(name)
     
        cs = ax.contour(self.__x, self.__z[timestep, :, :], 
                        pd['scale'] * getattr(self, pd['data'])[timestep, :, :], 
                        pd['clev'], colors=pd['color'], linewidths=1)
                        
        ax.clabel(cs, pd['clev'], fmt=pd['fmt'], inline_spacing=2, fontsize=8)             
        
        plt.title('Time = {0} seconds'.format(int(self.__output.t()[timestep])))
        plt.tight_layout()
        
        if file:
            plt.savefig(file)
        plt.show()
        
if __name__ == '__main__':
    pass
