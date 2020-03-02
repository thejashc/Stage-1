#!/usr/bin/env python

import numpy as np
import matplotlib.pyplot as plt
import sys

# read data
data = np.loadtxt('./' + INPUT + '/z_vs_t.dat')   # collect data : (0) actual time

indStart=0

# (1) imbibition time
zt = data[indStart:,2]          # meniscus position to be fitted
time = data[indStart:,1]        # corresponding imbibition time
zt2 = pow(zt,2)                 # square of the meniscus position

plt.plot(time, zt2)
plt.xlabel(r'$t$')
plt.ylabel(r'$z^2(t)$')

plt.savefig('./plots/z_vs_t.eps',format='eps')
