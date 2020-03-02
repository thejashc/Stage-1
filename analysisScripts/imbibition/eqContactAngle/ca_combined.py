#!/usr/bin/env python

import numpy as np
import matplotlib.pyplot as plt

#wet=np.array(['-10.0','-15.0','-20.0','-25.0','-30.0','-31.0','-32.0','-33.0','-34.0','-35.0','-36.0','-37.0','-38.0','-40.0'])
wet=np.array(['-10.0','-15.0','-20.0','-25.0','-30.0','-32.0','-33.0','-34.0','-35.0','-36.0','-37.0','-38.0','-40.0'])

#tStart=100000
#tEnd=1000000
#tDiff=500
#nElem=int((tEnd-tStart)/tDiff)+1

samples=wet.shape[0]
ca=np.zeros((samples,3))

count=0
for i in wet:

    fname='./As1l1_'+str(i)+'/archive/postProcessed/CAinDegrees.dat'

    # obtain menicusVelocity
    dat=np.loadtxt(fname)

    #ca[:,count]=dat

    ca[count,0]=float(i)
    ca[count,1]=np.nanmean(dat)
    ca[count,2]=np.nanstd(dat)

    count+=1

# write the data 
np.savetxt('./plots/CA_vs_Asl.dat',ca,fmt='%7.5f')

# plot the figure
np.seterr(divide='ignore', invalid='ignore')

plt.rc('font', family='arial')
plt.rc('xtick', labelsize='x-small')
plt.rc('ytick', labelsize='x-small')
plt.rc('text', usetex=False)
plt.rc('legend',fontsize='x-small')
plt.rc('legend',handlelength=2)
plt.rc('legend',edgecolor='black')
plt.rc('axes',edgecolor='black')
plt.rc('axes',linewidth=0.5)
plt.rc('legend',edgecolor='black')

cps=0.5     # capsize
elw=0.5     # edge line width    
mew=0.5     # marker edge width
msz=0.5

figW = 3.37
figH = (3./4.)*figW

plt.rcParams['font.size'] = 10

fig,ax = plt.subplots(figsize=(figW,figH))
plt.errorbar(ca[:,0], ca[:,1], ca[:,2], color='b', marker='s', markersize=msz, capsize=cps,
        elinewidth=elw, markeredgewidth=mew, linestyle='')

plt.locator_params(axis='y', nbins=5)
plt.locator_params(axis='x', nbins=5)

ax.tick_params(direction="in", width=0.25)

# adding labels
ax.set_xlabel(r'$A_{sl}$')
ax.set_ylabel(r'$\theta_{eq}$', rotation=0)

# change layout
fig.tight_layout()

fig.savefig('./plots/ca_vs_asl.eps',format='eps')
