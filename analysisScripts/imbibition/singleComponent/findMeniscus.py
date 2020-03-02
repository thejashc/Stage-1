#!/usr/bin/env python

import numpy as np
import matplotlib.pyplot as plt
from scipy.optimize import curve_fit
from scipy.optimize import least_squares
import sys

def Tanh(z, n0, z0, w):
    return 0.5 * n0 * ( 1. - np.tanh( ( z - z0 ) / w ) )

def cyl(p, x, z):
    return ( pow(x-p[0], 2.) + pow(z-p[1], 2.) - p[2])

capLen=115.
tStart=int(sys.argv[1])
tEnd=int(sys.argv[2])
tFileStart=int(sys.argv[3])
tDiff=100
dt=0.01
nElem=int((tEnd-tStart)/tDiff)+1
time=np.linspace(tStart,tEnd, nElem)
tDiff=100

# obtain radii and other parameters
capRad=5.
offSet=1.
radStart=0.
radEnd=capRad+offSet
dr=0.5
r = np.arange( radStart + 0.5 * dr, radEnd + 0.5*dr, dr)     # obtain r

zStart=0
zEnd=capLen
dz=1.0
zBins= int( (zEnd - zStart) / dz )

z = np.arange( zStart + 0.5 * dz, zEnd + 0.5*dz, dz ) # obtain z

dA=2.*np.pi*r*dr    # area of a circular element
dV=dA*dz            # differential volume element

zBins=z.shape[0]    # number of zbins
rBins=r.shape[0]    # number of rbins

rInt = np.zeros((rBins,1))
zInt = np.zeros((rBins,1))

#print( dr, dz, rBins, zBins, rho.shape )
nElem=int((tEnd-tStart)/tDiff)+1

meniscusPosition=np.zeros((nElem,rBins))

timeCount=int((tStart-tFileStart)/tDiff)
counter=0
for time in np.arange(tStart, tEnd+tDiff, tDiff):

    dat=np.loadtxt('./postProcessed/density/density_vs_time_'+str(timeCount)+'.dat') 

    N_rz=dat     # N(r,z) at a function of time
    rho=N_rz/dV         # number density = Number / volume

    count=0         # reset count to 0
    for i in np.arange(0, rBins, 1):

        popt, pcov = curve_fit(Tanh, z, rho[:,i], bounds=([5,0.,0.], [7., capLen, 1.5]),  maxfev=1000)

        rInt[count] = r[i]          # Note: I do not reser rInt, zInt because they are reset during this operation
        zInt[count] = popt[1]
        
        count += 1

    meniscusPosition[counter,:]=zInt.ravel()
    timeCount += 1
    counter += 1

np.savetxt('./postProcessed/interface_vs_time.dat',meniscusPosition)
