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
vzA = np.zeros((zBins, radBins))
vrA = np.zeros((zBins, radBins))
rhoA = np.zeros((zBins, radBins))

vzB = np.zeros((zBins, radBins))
vrB = np.zeros((zBins, radBins))
rhoB = np.zeros((zBins, radBins))

tStart=int(sys.argv[1])
tEnd=int(sys.argv[2])
tDiff=200
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

    velz = np.loadtxt('./postProcessed/velocityAnalysis/vzA_' + str(j) +'.dat')
    velr = np.loadtxt('./postProcessed/velocityAnalysis/vrA_' + str(j) +'.dat')
    N = np.loadtxt('./postProcessed/velocityAnalysis/rhoA_' + str(j) +'.dat')

    vzA += (velz/meniscusVelocity[j])
    vrA += (velr/meniscusVelocity[j])
    rhoA += N

    velz = np.loadtxt('./postProcessed/velocityAnalysis/vzB_' + str(j) +'.dat')
    velr = np.loadtxt('./postProcessed/velocityAnalysis/vrB_' + str(j) +'.dat')
    N = np.loadtxt('./postProcessed/velocityAnalysis/rhoB_' + str(j) +'.dat')

    vzB += (velz/meniscusVelocity[j])
    vrB += (velr/meniscusVelocity[j])
    rhoB += N

# obtain averages 
vzA /= rhoA
vrA /= rhoA

vzB /= rhoB
vrB /= rhoB

normFactor = ( nPerTrial * vol* Trials )

rhoAMap = rhoA/ normFactor
rhoBMap = rhoB/ normFactor

#################### DENSITY AT DIFFERENT LOCATIONS ####
fig,ax = plt.subplots(figsize=(figW,figH))

plt.plot(rad, rhoAMap[15,:], 'b.', label=r'$z=15$')
plt.plot(rad, rhoAMap[18,:], 'b.', label=r'$z=18$')
plt.plot(rad, rhoAMap[23,:], 'b.', label=r'$z=23$')

plt.plot(rad, rhoBMap[15,:], 'r.', label=r'$z=15$')
plt.plot(rad, rhoBMap[18,:], 'r.', label=r'$z=18$')
plt.plot(rad, rhoBMap[23,:], 'r.', label=r'$z=23$')

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
################## type A #####################
# drawing the contour map
X, Y = np.meshgrid(rad, z)

# Create figure and axes
fig,ax = plt.subplots(figsize=(figW,figH))
divider = make_axes_locatable(ax)
cax = divider.append_axes('right', size='5%', pad=0.05)

h = ax.contourf(X, Y, rhoAMap, 10, cmap = 'Blues')
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
fig.savefig('./plots/MC_wetting_rhoA.eps',format='eps')

################## type B #####################
# drawing the contour map
X, Y = np.meshgrid(rad, z)

# Create figure and axes
fig,ax = plt.subplots(figsize=(figW,figH))
divider = make_axes_locatable(ax)
cax = divider.append_axes('right', size='5%', pad=0.05)

h = ax.contourf(X, Y, rhoBMap, 10, cmap = 'Reds')
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
fig.savefig('./plots/MC_wetting_rhoB.eps',format='eps')
#################### QUIVER PLOTS ######################
################## type A #####################
zcutOff = 0
rcutOff = -1

vrNew = vrA
vzNew = vzA

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
fig.savefig('./plots/MC_wetting_velA.eps',format='eps')

################## type B #####################
vrNew = vrB
vzNew = vzB

# eliminate spurious velocities
idx = (abs(vrNew) > 4.) | ( abs(vzNew) > 4.)

vrNew[idx] = 0.
vzNew[idx] = 0.

# setting up the grid
X, Y = np.meshgrid(rad[:rcutOff], z[zcutOff:])

# Create figure and axes
fig,ax = plt.subplots(figsize=(figW,figH))

ax.quiver(X, Y, vrNew[zcutOff:,:rcutOff], vzNew[zcutOff:,:rcutOff], color='red')

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
fig.savefig('./plots/MC_wetting_velB.eps',format='eps')
#################### VELOCITY PROFILES ######################
zcutOff = 12
rcutOff = -1
capRad = 5

xPlot = rad[:rcutOff]
avvzA = np.mean(vzA[zcutOff:,:rcutOff],0)
avvzB = np.mean(vzB[zcutOff:,:rcutOff],0)
vTh = -2.0*( 1- pow(xPlot/capRad, 2.))

# Create figure and axes
fig,ax = plt.subplots(figsize=(figW,figH))

ax.plot(xPlot[:-1], avvzA[:-1],'bo-')
ax.plot(xPlot[:-1], avvzB[:-1],'ro-')

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
