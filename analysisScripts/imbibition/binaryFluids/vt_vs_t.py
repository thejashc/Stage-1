#! /usr/bin/env python3

import numpy as np
import matplotlib.pyplot as plt
import sys

plt.switch_backend('agg')

tStart=int(sys.argv[1])
tEnd=int(sys.argv[2])
tDiff=200
dt=0.01
nElem=int((tEnd-tStart)/tDiff)+1
time=np.linspace(tStart,tEnd, nElem)*dt
tDiff=200

# defining INPUT and OUTPUT folder
#INPUT = sys.argv[1]
#OUTPUT = sys.argv[2]

#indStart = 1100

# read data
data = np.loadtxt('./postProcessed/interface_vs_time.dat')   # collect data : (0) actual time

# (1) imbibition time
zt = data[:,0]          # meniscus position to be fitted
zt2 = pow(zt,2)                 # square of the meniscus position

# fit meniscus with the function fitFunc
fit = np.polyfit(time, zt2, 1)   # fit a straight line of the form z^2(t) = at + b, where a = fit[0], b=fit[1]
zt2Fit = fit[0]*time + fit[1]
ztFit = np.sqrt(zt2Fit)

# derivative of fitFunc is the velocity 
vt = fit[0] / ( 2. * zt )    # velocity as a function of time

# write imbibition velocity data
tWrite = time
wrtData = np.array((tWrite, vt))
wrtData = wrtData.transpose()

np.savetxt('./postProcessed/meniscusVelocity.dat', wrtData, fmt='%5.10f')

# write imbibition position data
tWrite = time
wrtData = np.array((tWrite, zt))
wrtData = wrtData.transpose()
np.savetxt('./postProcessed/meniscusPosition.dat', wrtData, fmt='%5.10f')

# plot the relevant data to check for correctness
#fig, ax1 = plt.subplots()

#ax1.set_xlabel(r'$t$')
#ax1.set_ylabel(r'$z(t)$', color='blue')
#ax1.plot(data[:,1], data[:,2], 'b.')        # plot complete data
#ax1.plot(time, ztFit, 'k-')                 # plot fit data

#ax2 = ax1.twinx()  # instantiate a second axes that shares the same x-axis

#ax2.set_ylabel(r'$v(t)$', color='green')  # we already handled the x-label with ax1
#ax2.plot(time, vt, color='green')

#fig.savefig('./' + OUTPUT + '/vt_vs_t.eps',format='eps')
