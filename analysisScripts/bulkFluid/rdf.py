#! /usr/bin/env python3
import numpy as np
import matplotlib.pyplot as plt 


dat=np.loadtxt('rdf.dat')

N = 1849
A = 15*15
rbinWidth=0.01
nSamples=401
rho = N / A 
kernel=dat[:,0] * 2. * np.pi * rbinWidth * nSamples * rho * N 

gr=dat[:,1]/kernel
rad=dat[:,0]

plt.plot(rad,gr)
plt.plot([0.,5.],[1.,1.],'k--')
plt.xlim([0,5])

plt.show()
