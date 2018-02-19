#! /usr/bin/python

import matplotlib.pyplot as plt
import numpy as np
import pylab as pl

def figEnv():
    return plt.figure(num=None, figsize=(8, 6), dpi=80, facecolor='w', edgecolor='k')

#------ ENERGY STATISTICS --------#

data = np.loadtxt( './data/en_data.dat' )

# average energy values
peAvg 	= np.mean( data[:,0] )
keAvg 	= np.mean( data[:,1] )
totAvg 	= np.mean( data[:,2] )

# deviation from mean
peDev 	= ( data[:,0] - peAvg  ) / peAvg
keDev 	= ( data[:,1] - keAvg  ) / keAvg
totDev	= ( data[:,2] - totAvg ) / totAvg

figEnv()
plt.plot( keDev	, c='r', label='Kinetic Energy' )
plt.plot( peDev	, c='b', label='Potential Energy' )
plt.plot( totDev, c='g', label='Total Energy' )
plt.xlabel( 'Time' )
plt.ylabel( 'Energy' )
plt.legend()
plt.grid()
plt.savefig( './plots/energy.eps', format='eps', dpi=1200)
#plt.show()


#------ MOMENTUM STATISTICS --------#
data = np.loadtxt( './data/mom_data.dat' )

# storing values
momX = data[:, 0]
momY = data[:, 1]
momZ = data[:, 2]

figEnv()
plt.plot( momX	, c='r', label='$p_{x}$' )
plt.plot( momY	, c='b', label='$p_{y}$' )
plt.plot( momZ	, c='g', label='$p_{z}$' )
plt.xlabel( 'Time' )
plt.ylabel( 'Momentum' )
plt.ylim( (-1e-10,1e-10) )
plt.grid()
plt.legend()
plt.savefig( './plots/momentum.eps', format='eps', dpi=1200)

#------- VELOCITY DISTRIBUTION ------#
data = np.loadtxt( './data/velDist_data.dat' )

# probability distributions
velBin 	 	= data[:,0]
kbT		= data[0,1]
velHistX 	= data[:,2]
velHistY 	= data[:,4]
velHistZ 	= data[:,6]
normAreaX 	= data[:,3]
normAreaY 	= data[:,5]
normAreaZ 	= data[:,7]

# Maxwell-Boltzmann distribution
m		= 1.0
v2		= np.power( velBin, 2.0)
PI 		= np.pi
MB		= np.sqrt( m/(2.0*PI*kbT) ) * np.exp(-(0.5*m*v2) /kbT )

pdfX = velHistX / normAreaX
pdfY = velHistY / normAreaY
pdfZ = velHistZ / normAreaZ

figEnv()
plt.plot( velBin, pdfX, c='r', label='$v_{x}$' )
plt.plot( velBin, pdfY, c='g', label='$v_{y}$' )
plt.plot( velBin, pdfZ, c='b', label='$v_{z}$' )
plt.plot( velBin, MB  , c='k', label='MB, T = %.2f' % kbT ) 
plt.xlim( (-5. , 5. ) )
plt.xlabel( '$v$' )
plt.ylabel( 'PDF( $v$ )' )
plt.grid()
plt.legend()
plt.savefig( './plots/velDist.eps', format='eps', dpi=1200)

#----------- RADIAL DISTRIBUTION FUNCTION --------#
data = np.loadtxt( './data/gr_data.dat' )

# data
ri	= data[:,0]
ro	= data[:,1]
rad	= data[:,2]
grCount	= data[:,3]
npart	= data[:,4]
samples	= data[:,5]
rho	= data[:,6]

# g(r) calculation
shellVol	= (4./3.)* PI * ( np.power( ro, 3.) - np.power( ri, 3. ) )
nHomo		= shellVol * rho
gr		= grCount / ( npart * samples * nHomo ) 

figEnv()
plt.plot( rad, gr, c='r', label='g(r)' )
plt.xlabel( 'r' )
plt.ylabel( 'g(r)' )
plt.grid()
plt.legend()
plt.savefig( './plots/gr.eps', format='eps', dpi=1200)

#------------- TEMPERATURE AND PRESSURE -------------#
data = np.loadtxt( './data/eos_data.dat' )

time 	= data[:,0]
Temp	= data[:,1]
Pressure= data[:,2]

figEnv()
plt.plot( Temp, c='r', label='<T>' )
plt.xlabel( 'time' )
plt.ylabel( 'T' )
plt.grid()
plt.legend()
plt.savefig( './plots/temp.eps', format='eps', dpi=1200 )

figEnv()
plt.plot( Pressure, c='b', label='<P>' )
plt.xlabel( 'time' )
plt.ylabel( 'P' )
plt.grid()
plt.legend()
plt.savefig( './plots/pressure.eps', format='eps', dpi=1200 )
