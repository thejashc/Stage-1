#!/usr/bin/env python

import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.axes_grid1 import make_axes_locatable
import matplotlib.patches as patches
import sys

np.seterr(divide='ignore', invalid='ignore')


figW = 3.37
figH = (3./4.)*figW

plt.rcParams['font.size'] = 10

# obtain radii and other parameters
capRad=5.
offSet=1.
radStart=0.
radEnd=capRad+offSet
rBinWidth=0.5
rad = np.arange( radStart + 0.5 * rBinWidth, radEnd + 0.5*rBinWidth, rBinWidth)

radBins = rad.shape[0]

zStart =    -10
zEnd   =   15.
zBinWidth = 1.0
zBins     = int( (zEnd - zStart) / zBinWidth )

z = np.arange( zStart + 0.5 * zBinWidth, zEnd + 0.5*zBinWidth, zBinWidth )
vol = 2 * np.pi * rad * rBinWidth * zBinWidth

#print(vol)
#print(radBins)

# obtain quiver plots
vz = np.zeros((zBins, radBins))
vr = np.zeros((zBins, radBins))
rho = np.zeros((zBins, radBins))

tStart=int(sys.argv[1])
tEnd=int(sys.argv[2])
tDiff=100
nElem=int((tEnd-tStart)/tDiff)+1

nStart = 0
nEnd = nElem-1
nPerTrial = nEnd -nStart + 1
Trials=1

# obtain menicusVelocity
veldat=np.loadtxt('./postProcessed/meniscusVelocity.dat')
meniscusVelocity=veldat[:,1]

#print(meniscusVelocity)

# obtain data from the velocity profiles
for j in np.arange(nStart,nEnd,1):

    velz = np.loadtxt('./postProcessed/velocityAnalysis/vz_' + str(j) +'.dat')
    velr = np.loadtxt('./postProcessed/velocityAnalysis/vr_' + str(j) +'.dat')
    N = np.loadtxt('./postProcessed/velocityAnalysis/rho_' + str(j) +'.dat')

    vz += (velz/meniscusVelocity[j])
    vr += (velr/meniscusVelocity[j])
    rho += N
# obtain averages 
vz /= rho
vr /= rho

normFactor = ( nPerTrial * vol* Trials )

rhoMap = rho/ normFactor

#################### DENSITY AT DIFFERENT LOCATIONS ####
fig,ax = plt.subplots(figsize=(figW,figH))

plt.plot(rad, rhoMap[15,:], 'g.', label=r'$z=15$')
plt.plot(rad, rhoMap[18,:], 'g.', label=r'$z=18$')
plt.plot(rad, rhoMap[23,:], 'g.', label=r'$z=23$')

# adding labels
ax.set_xlabel(r'$r$')
ax.set_ylabel(r'$\rho(r)$')

# legend
plt.legend()

# limit figure limits
ax.set_xlim([0.,6.0])

# change layout
fig.tight_layout()

# saving figure
fig.savefig('./plots/rho_vs_r.eps',format='eps')

#################### DENSITY MAPS ######################
# drawing the contour map
X, Y = np.meshgrid(rad, z)

# Create figure and axes
fig,ax = plt.subplots(figsize=(figW,figH))
divider = make_axes_locatable(ax)
cax = divider.append_axes('right', size='5%', pad=0.05)

h = ax.contourf(X, Y, rhoMap, 10, cmap = 'Blues')
fig.colorbar(h, cax=cax)

# Create a Rectangle patch
rect = patches.Rectangle((5,-10),1,40,linewidth=1,edgecolor='r',facecolor='r')

# Add the patch to the Axes
ax.add_patch(rect)

# add text
s = 'capillary wall'
ax.text(5.025, 11.0, s, fontsize=12, fontweight='bold', rotation=90, color='white')

# adding labels
ax.set_xlabel(r'$r$')
ax.set_ylabel(r'$z$')

# limit figure limits
ax.set_xlim([0.,5.5])
ax.set_ylim([-10.,15.])

# set xticks
ax.set_xticks([0.,1.,2.,3.,4.,5.])

# change layout
fig.tight_layout()

# saving figure
fig.savefig('./plots/rho.eps',format='eps')

#################### QUIVER PLOTS ######################
zcutOff = 0
rcutOff = -1

vrNew = vr
vzNew = vz

# eliminate spurious velocities
idx = (abs(vrNew) > 4.) | ( abs(vzNew) > 4.)

vrNew[idx] = 0.
vzNew[idx] = 0.

# setting up the grid
X, Y = np.meshgrid(rad[:rcutOff], z[zcutOff:])

# Create figure and axes
fig,ax = plt.subplots(figsize=(figW,figH))

ax.quiver(X, Y, vrNew[zcutOff:,:rcutOff], vzNew[zcutOff:,:rcutOff], color='blue')

ax.set_ylabel(r'$z$')
ax.set_xlabel(r'$r$')

ax.set_ylim([-10.,15.])

# Create a Rectangle patch
rect = patches.Rectangle((5,-10),1,40,linewidth=1,edgecolor='r',facecolor='r')

# Add the patch to the Axes
ax.add_patch(rect)

# add text
s = 'capillary wall'
ax.text(5.1, 11.0, s, fontsize=12., fontweight='bold', rotation=90, color='white')

# set xticks
ax.set_xticks([0.,1.,2.,3.,4.,5.])

# change layout
fig.tight_layout()

# saving figure
fig.savefig('./plots/vel.eps',format='eps')

#################### VELOCITY PROFILES ######################
zcutOff = 12
rcutOff = -1
capRad = 5

xPlot = rad[:rcutOff]
avvz = np.mean(vz[zcutOff:,:rcutOff],0)
vTh = -2.0*( 1- pow(xPlot/capRad, 2.))

# Create figure and axes
fig,ax = plt.subplots(figsize=(figW,figH))

ax.plot(xPlot[:-1], avvz[:-1],'go-')
ax.plot(xPlot, vTh, 'k-',label='Theory')

ax.plot([0.,5.],[0.,0.], 'r--')
#plt.plot([5.,5.],[-2.,0.25], 'r--')

# Create a Rectangle patch
rect = patches.Rectangle((5,-2.5),1,4,linewidth=1,edgecolor='r',facecolor='r')

# Add the patch to the Axes
ax.add_patch(rect)

# add text
s = 'capillary wall'
ax.text(5.1, -0.25, s, fontsize=12., fontweight='bold', rotation=90, color='white')

# set limits to the figure
ax.set_xlim([0.,5.5])

# legend
fig.legend(loc='center left', bbox_to_anchor=(0.2, 0.65))

# change layout
fig.tight_layout()

# saving figure
fig.savefig('./plots/MC_wetting_vzr.eps',format='eps')
