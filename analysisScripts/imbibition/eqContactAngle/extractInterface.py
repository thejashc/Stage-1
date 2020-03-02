#!/usr/bin/env python

import matplotlib.pyplot as plt
import numpy as np
from scipy.optimize import curve_fit
from scipy.optimize import least_squares
import sys

def Tanh(z, n0, z0, w):
    return 0.5 * n0 * ( 1. - np.tanh( ( z - z0 ) / w ) )

def cyl(p, x, z):
    return ( pow(x-p[0], 2.) + pow(z-p[1], 2.) - p[2])

dat=np.loadtxt('./postProcessed/density/density_vs_time_1.dat')

dx=0.5
dz=0.5
dy=10.     # entire length in the y-direction

dvol=dx*dz*dy

xmin=-21.+dx*0.5
xmax=+21.-dx*0.5

zmin=-40.+dz*0.5
zmax=+40.-dz*0.5

x=np.linspace(xmin,xmax,dat.shape[1])
z=np.linspace(zmin,zmax,dat.shape[0])

[xGrid,zGrid]=np.meshgrid(x,z)

tStart=int(sys.argv[1])
tEnd=int(sys.argv[2])
tDiff=int(sys.argv[3])

nElem=int((tEnd-tStart)/tDiff)+1

# fitting tanh function to each z 
zInterface=np.zeros((34,nElem))

timeCount=0
for time in np.arange(0,nElem,1):

    dat=np.loadtxt('./postProcessed/density/density_vs_time_'+str(time)+'.dat')
    
    counter=0
    for i in np.arange(25,59,1):
        popt, pcov = curve_fit(Tanh, z[70:], dat[70:,i]/dvol, bounds=([1.,0.,0.], [10., 40., 1.5]),  maxfev=2000)
    
        zInterface[counter,timeCount]=popt[1]
    
        counter+=1
   
    timeCount+=1
    #zInterface[...]=0

# write the data
np.savetxt('./postProcessed/xInterface.dat',x[25:59],fmt='%7.5f')
np.savetxt('./postProcessed/zInterface.dat',zInterface,fmt='%7.5f')

#plt.contourf(xGrid, zGrid, dat/dvol)
#plt.plot(x[20:60], zInterface[:,timeCount],'ws')
#
#plt.plot([xmin,xmax],[0.,0.],'k--')
#plt.plot([0.,0.],[zmin,zmax],'k--')
#
#plt.plot([-8.,-8.],[zmin,zmax],'w--')
#plt.plot([8.,8.],[zmin,zmax],'w--')
#
#plt.xlim([-8., 8.])
#plt.ylim([4, 15.])
#
#plt.colorbar()
