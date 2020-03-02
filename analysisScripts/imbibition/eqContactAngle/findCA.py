#!/usr/bin/env python

import matplotlib.pyplot as plt
import numpy as np
from scipy.optimize import curve_fit
from scipy.optimize import least_squares
import sys

def cyl(p, x, z):
    return (pow(x - p[0] , 2.) + pow(z - p[1], 2.)  - pow( p[2], 2.))

xdat=np.loadtxt('./postProcessed/xInterface.dat')
zdat=np.loadtxt('./postProcessed/zInterface.dat')

nElem=zdat.shape[1]

xC=np.zeros((nElem,1))
zC=np.zeros((nElem,1))
rad=np.zeros((nElem,1))

zInt=np.zeros((nElem,1))

xcutoff=3
xWall=8.0

# fitting tanh function to each z 
timeCount=0
for time in np.arange(0,nElem,1):

    xcyl=xdat[xcutoff:-xcutoff]
    zcyl=zdat[xcutoff:-xcutoff, time]
    p0 = np.array([0., 15.0, 8.0])

    fits=least_squares(cyl, p0, args=(xcyl,zcyl))

    xC[time]=fits.x[0]
    zC[time]=fits.x[1]
    rad[time]=fits.x[2]

    c1=plt.Circle((xC[time],zC[time]),rad[time],color='r',fill=False)

    # visualization
    #fig,ax=plt.subplots()
    #plt.plot(xcyl,zcyl,'s-')
    #ax.add_artist(c1)
    #plt.axis('equal')
    #plt.show()
    #exit()

    timeCount+=1

# find intersection points of circle with wall
zInt=zC + np.sqrt( pow(rad,2.) - pow(xWall - xC, 2.) )

#print( zInt-zC )
#print( xWall-xC )

# find the angle
tanTheta=(zInt-zC)/(xWall-xC)
theta=np.arctan(tanTheta)

#print(theta)

CA=np.pi - theta
CAinDegrees=CA*(180./np.pi)

# write the data
np.savetxt('./postProcessed/CAinDegrees.dat',CAinDegrees,fmt='%7.5f')
