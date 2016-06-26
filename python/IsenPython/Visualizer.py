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

import sys

import numpy as np
import matplotlib 
import matplotlib.pyplot as plt
import matplotlib.gridspec as gridspec
from matplotlib.ticker import MultipleLocator

class Visualizer:
    """Visualize output of simulation """
        
    def __init__(self, output):
        """Initalize vizualizer methods 
        
        Args:
            output: IsenPython.Output instance containing the data.
        """
        matplotlib.rcParams.update({'font.size': 11})

        self.output = output
        namelist = output.getNameList()
        
        # NameList parameters
        self.nt = len(self.output.t())
        self.nz = namelist.nz
        self.nx = namelist.nx
        self.u00 = namelist.u00
        
        # Plotting variables
        dx = namelist.xl / namelist.nx
        self.xlim = [0, (self.nx - 1) * dx / 1000.]
        self.zlim = [0, 10]
        self.title = namelist.run_name
        
        self.tci = 2        # Theta contour interval (K)
        self.vci = 2        # Velocity contour interval (m/s)
        self.qvci = 0.5	    # Vapor contour interval (g/kg)
        self.qcci = 0.1	    # Cloud water contour interval (g/kg)
        self.qrci = 0.005	# Rain contour interval (g/kg)
        
        dth = namelist.thl / self.nz
        self.tlim = [namelist.th00 + dth / 2., 400.];
        self.vlim = [0., 60.]
        self.qvlim = [self.qvci, 10 * self.qvci]
        self.qclim = [self.qcci, 10 * self.qcci]
        self.qrlim = [self.qrci, 100 * self.qrci]
        
        # X-axis
        self.x = np.tile(np.array(range(0, self.nx)) * dx / 1000., [self.nz, 1]).transpose();

        # Geometric height (destaggered)
        z = self.output.z() / 1000.
        self.z = np.zeros((self.nt, self.nx, self.nz))
        for t in xrange(0, self.nt):
            for i in xrange(0, self.nx):
                    self.z[t, i, 0:self.nz] = 0.5 * (z[t, i, 0:self.nz] + z[t, i, 1:self.nz+1])
        
        # Theta value between the coordinate surfaces
        theta = np.zeros((self.nz))
        for i in xrange(len(theta)):
            theta[i] = namelist.th00 + (i + 0.5) * dth
        self.theta = np.tile(theta,[self.nx, 1])
        
        # Topography height
        self.topo = z[:, :, 0]
        
        # Velocity (destaggered)
        self.u = self.output.u()
        
        # Moist variables
        if namelist.imoist: 
          self.qv = 1000 * self.output.qv()      # Convert to g/kg
          self.qc = 1000 * self.output.qc()      # Convert to g/kg
          self.qr = 1000 * self.output.qr()      # Convert to g/kg
          self.prec = self.output.prec()         # mm/h
          self.tot_prec = self.output.tot_prec()  # mm
        
    def __generateDictonary(self, name):
        if name.lower() == 'horizontal_velocity':
            vclev1 = np.arange(self.vlim[0], self.u00, self.vci)
            vclev2 = np.arange(self.u00 + self.vci, self.vlim[1] + self.vci, self.vci)
            
            plotDict = dict(fmt = '%2i',
                            clev = np.array(list(vclev1) + [self.u00] + list(vclev2)),
                            color = ['lime'] * len(vclev1) + ['k'] + ['r'] * len(vclev2),
                            scale = 1,
                            data = 'u') 
            return plotDict
            
        if name.lower() == 'specific_rain_water_content':
            plotDict = dict(fmt='%1.3f',
                            clev = np.arange(self.qrlim[0], self.qrlim[1], self.qrci),
                            color='skyblue',
                            scale = 1000,
                            data = 'qr') 
            return plotDict
            
        if name.lower() == 'specific_cloud_liquid_water_content':
            plotDict = dict(fmt='%1.1f',
                            clev = np.arange(self.qclim[0], self.qclim[1], self.qcci),
                            color = 'dodgerblue',
                            scale = 1000,
                            data = 'qc') 
            return plotDict
            
        if name.lower() == 'specific_humidity':
            plotDict = dict(fmt='%2i',
                            clev=np.arange(self.qvlim[0], self.qvlim[1], self.qvci),
                            color='blue',
                            scale=1000,
                            data = 'qv') 
            return plotDict
                
        raise RuntimeError("Visualizer: invalid plot name '{0}'".format(name))
        
    def plot(self, name, timestep, file = None, show = True):
        """Generate velocitry contour plot
        
        Args:
            name: Name of the plot to generate:
                - horizontal_velocity
                - rain_water
            timestep: Specify which data set to use.
            file: Save to file if not None
            show: Show plot
        """
        if timestep >= self.nt:
            raise RuntimeError("Visualizer: requested timestep '{0}' exceeds available timesteps '{1}'".format(
                  timestep, self.nt - 1))
                
        # Create figure
        fig = plt.figure(figsize=(6.5, 5.25))
        fig.subplots_adjust(left = 0.1, bottom = 0.12, right = 0.97, top = 0.96)
        
        if name == 'specific_rain_water_content':
            gs = gridspec.GridSpec(2, 1, height_ratios=[20, 20])
        else:
            gs = gridspec.GridSpec(1, 1, height_ratios=[20])
        
        # Create axis
        ax = fig.add_subplot(gs[0, 0])
        if name == 'specific_rain_water_content':
            ax2 = fig.add_subplot(gs[1, 0])
        else:
            ax2 = None
                
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
        ax.contour(self.x, self.z[timestep, :, :], self.theta, clev, colors='grey', linewidths=1)
        
        # Plot topography
        ax.plot(self.x[:, 0], self.topo[timestep, :], linewidth=2, color='k');
        
        # Plot quantity 'name'
        pd = self.__generateDictonary(name)
     
        cs = ax.contour(self.x, self.z[timestep, :, :], 
                        pd['scale'] * getattr(self, pd['data'])[timestep, :, :], 
                        pd['clev'], colors=pd['color'], linewidths=1)
                        
        ax.clabel(cs, pd['clev'], fmt=pd['fmt'], inline_spacing=2, fontsize=8)             
        
        if name == 'specific_rain_water_content':
            ax2.cla()
            ax2.set_ylabel('Acum. Rain [mm]')
            ax2.set_ylim([0, 0.1 + max(self.tot_prec[timestep, :])])
            ax2.set_xlim(self.xlim)
            cs = ax2.plot(self.x[:, 0], self.tot_prec[timestep, :], 'b-')
                          
        plt.title('Time = {0} seconds'.format(int(self.output.t()[timestep])))
        plt.tight_layout()
        
        if file:
            print("Saving {0} plot at timestep {1} to '{2}'".format(name, timestep, file))
            plt.savefig(file)
        if show:
            plt.show()

        # Cleanup    
        plt.cla()
        plt.clf()
        plt.close()
        
if __name__ == '__main__':
    pass
