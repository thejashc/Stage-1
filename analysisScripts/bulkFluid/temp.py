#! /usr/bin/env python3
import numpy as np
import matplotlib.pyplot as plt 

dat=np.loadtxt('temperature.dat')

N = 1849

av_kx=dat[:,0]/N
av_ky=dat[:,1]/N

t=0.5*(av_kx + av_ky )

print(np.mean(t))

plt.plot(t)

plt.show()
