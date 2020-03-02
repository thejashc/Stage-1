#!/usr/bin/env python

import numpy as np

gammaDot=0.01
dat=np.loadtxt('./data/pTens.dat')

indStart=40
kinVisc=np.loadtxt('./postProcessed/kin_visc_xy.dat')

pxyC=np.mean(dat[indStart:,10])
pxyD=np.mean(dat[indStart:,28])
pxyR=np.mean(dat[indStart:,37])
kinViscxy=np.mean(kinVisc)

pxy=-(pxyC+pxyR+pxyD+kinViscxy)
visc=pxy/gammaDot

print(visc)
