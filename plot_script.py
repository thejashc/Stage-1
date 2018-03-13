#! /usr/bin/env python3

import matplotlib.pyplot as plt
import numpy as np
import pylab as pl
import math as math

# global definition of pi
PI 		= np.pi
plt.rcParams.update({'font.size': 12})

def figEnv():
    return plt.figure(num=None, figsize=(8. , 6.), dpi=80, facecolor='w', edgecolor='k')

#------- INTERFACIAL PROFILE & CONTACT ANGLE -------#
filelist=[]

for i in np.arange(50,200,5):
    filelist.append("./data/segPlane_%s.dat" %(i * 1000) )

refFile = np.loadtxt('./data/segPlane_50000.dat')
ind = np.arange(10,111,1)
xRef = refFile[ind, 0]
zRef = refFile[ind, 1]

#xRef = xRef[~np.isnan(xRef)]
#zRef = zRef[~np.isnan(zRef)]

dim = xRef.size
xCOM = np.zeros(dim)
zCOM = np.zeros(dim)
counter = 0

for fname in filelist:
	data = np.loadtxt(fname)
	
	x= data[ind, 0] * 5000
	z= data[ind, 1] * 5000
	x= x[~np.isnan(x)]
	z= z[~np.isnan(z)]

	xCOM = xCOM + x
	zCOM = zCOM + z
	counter = counter + 1

xCOM = xCOM/counter
zCOM = zCOM/counter

# fitting fourth-order polynomial
xp = np.linspace(31.0, 31.4, 100 )
p4 = np.poly1d( np.polyfit( xCOM[0:10],zCOM[0:10], 4) )
p4d = np.polyder( p4 ) 

plt.figure()
plt.plot( xCOM, zCOM, '.-', xp, p4(xp), '.' )
plt.grid()
plt.xlabel( r'$x$' )
plt.ylabel( r'$z$' )
plt.title( 'Interface profile' )
plt.show()

CA = 180. -  np.arctan( p4d(xp ) )*( 180. / PI ) 
plt.figure()
plt.plot( xp, CA, '.' )
plt.grid()
plt.xlabel( r'$x$' )
plt.ylabel( r'$\theta$' )
plt.show()

##------ ENERGY STATISTICS --------#
#
#data = np.loadtxt( './data/en_data.dat' )
#
## average energy values
#peAvg 	= np.mean( data[:,0] )
#keAvg 	= np.mean( data[:,1] )
#totAvg 	= np.mean( data[:,2] )
#
## deviation from mean
#peDev 	= ( data[:,0] - peAvg  ) / peAvg
#keDev 	= ( data[:,1] - keAvg  ) / keAvg
#totDev	= ( data[:,2] - totAvg ) / totAvg
#
#figEnv()
#plt.title('Energy vs time')
#plt.plot( keDev	, label= r'$KE$' )
#plt.plot( peDev	, label= r'$PE$' )
#plt.plot( totDev, label= r'$TE$' )
#plt.xlabel( r'$t$' )
#plt.ylabel( r'$\frac{\Delta E}{\langle E \rangle}$' )
#plt.legend()
#plt.grid()
#plt.savefig( './plots/energy.svg', format='svg', dpi=1200 )
##plt.show()
#
##------ MOMENTUM STATISTICS --------#
#data = np.loadtxt( './data/mom_data.dat' )
#
## storing values
#momX = data[:, 0]
#momY = data[:, 1]
#momZ = data[:, 2]
#
#figEnv()
#plt.plot( momX, label='$p_{x}$' )
#plt.plot( momY, label='$p_{y}$' )
#plt.plot( momZ, label='$p_{z}$' )
#plt.xlabel( r'$t$' )
#plt.ylabel( r'$p_{\alpha}$' )
#plt.ylim( (-1e-10,1e-10) )
#plt.grid()
#plt.legend()
#plt.savefig( './plots/momentum.svg', format='svg', dpi=1200)
#
##------------- TEMPERATURE AND PRESSURE -------------#
#data = np.loadtxt( './data/eos_data.dat' )
#
#time 	= data[:,0]
#Temp	= data[:,1]
#Pressure= data[:,2]
#
#figEnv()
#plt.plot( Temp, label='<T>' )
#plt.xlabel( '$t$' )
#plt.ylabel( '$T$' )
#plt.grid()
#plt.legend()
#plt.savefig( './plots/temp.svg', format='svg', dpi=1200 )
#
##------------- DENSITY PROFILE-------------#
#filelist=[]
#
#for i in range(10,34):
#    filelist.append("./data/rhoZ_%s.dat" %(i * 10000) )
#
## setting up matrices for averaging
#data = np.loadtxt( "./data/rhoZ_10000.dat" )
#z 		= data[:,0]
#vol		= data[:,1]
#m	 	= len(z) 
#rhoZCount 	= np.zeros(m)
#counter 	= np.zeros(m)
#
#figEnv()
#for fname in filelist:
#	data = np.loadtxt( fname )
#	rhoZCount 	= rhoZCount + data[:,2]
#	counter 	= counter + data[:,3]
#	
#rhoZ 		= ( rhoZCount ) / ( counter * vol )
#plt.plot( z, rhoZ )
#plt.title ( r'$ \rho(z)\ vs\ z $' ) 
#plt.xlabel( r'$ z $' )
#plt.ylabel( r'$ \rho $' )
#plt.grid()
#plt.savefig( './plots/rhoZ_vs_Z.svg', format='svg', dpi=1200 )
#
##------------- PRESSURE TENSOR --------------#
#data = np.genfromtxt( './data/pTens.dat', delimiter=',' )
#
#pxx = data[:,0] + data[:,9]
#pyy = data[:,4] + data[:,13]
#pzz = data[:,8] + data[:,17]
##
#pxxAvg = np.mean( pxx[10:34] )
#pyyAvg = np.mean( pyy[10:34] )
#pzzAvg = np.mean( pzz[10:34] )
#gamma   = ( 10.0 / 4.0 )*( 2. * pzzAvg - pxxAvg - pyyAvg )
##
#figEnv()
#plt.plot ( pxx, label= r'$\langle p_{xx} \rangle = %.5f$' %pxxAvg )
#plt.plot ( pyy, label= r'$\langle p_{yy} \rangle = %.5f$' %pyyAvg )
#plt.plot ( pzz, label= r'$\langle p_{zz} \rangle = %.5f$' %pzzAvg )
#
#plt.title( r'$\gamma =%.5f$'%gamma )
##plt.annotate('del P =%.5f'%delP, xy=(100,2.35), xytext=( 100, 2.35))
#plt.xlabel(r'$t$')
#plt.ylabel(r'$P_{\alpha \beta}$')
#plt.grid()
#plt.legend( loc = 4)
#plt.savefig( './plots/pTens.svg', format='svg', dpi=1800 )
#exit()
#
##------- VELOCITY DISTRIBUTION ------#
#data = np.loadtxt( './data/velDist_data.dat' )
#
## probability distributions
#velBin 	 	= data[:,0]
#kbT		= data[0,1]
#velHistX 	= data[:,2]
#velHistY 	= data[:,4]
#velHistZ 	= data[:,6]
#normAreaX 	= data[:,3]
#normAreaY 	= data[:,5]
#normAreaZ 	= data[:,7]
#
## Maxwell-Boltzmann distribution
#m		= 1.0
#v2		= np.power( velBin, 2.0)
#MB		= np.sqrt( m/(2.0*PI*kbT) ) * np.exp(-(0.5*m*v2) /kbT )
#
#pdfX = velHistX / normAreaX
#pdfY = velHistY / normAreaY
#pdfZ = velHistZ / normAreaZ
#
#figEnv()
#plt.plot( velBin, pdfX, c='r', label='$v_{x}$' )
#plt.plot( velBin, pdfY, c='g', label='$v_{y}$' )
#plt.plot( velBin, pdfZ, c='b', label='$v_{z}$' )
#plt.plot( velBin, MB  , c='k', label='MB, T = %.2f' % kbT ) 
#plt.xlim( (-5. , 5. ) )
#plt.xlabel( '$v$' )
#plt.ylabel( 'PDF( $v$ )' )
#plt.grid()
#plt.legend()
#plt.savefig( './plots/velDist.svg', format='svg', dpi=1200)
#
##----------- RADIAL DISTRIBUTION FUNCTION --------#
#data = np.loadtxt( './data/gr_data.dat' )
#
## data
#ri	= data[:,0]
#ro	= data[:,1]
#rad	= data[:,2]
#grCount	= data[:,3]
#npart	= data[:,4]
#samples	= data[:,5]
#rho	= data[:,6]
#
## g(r) calculation
#shellVol	= (4./3.)* PI * ( np.power( ro, 3.) - np.power( ri, 3. ) )
#nHomo		= shellVol * rho
#gr		= grCount / ( npart * samples * nHomo ) 
#
#figEnv()
#plt.plot( rad, gr, c='r', label='g(r)' )
#plt.xlabel( 'r' )
#plt.ylabel( 'g(r)' )
#plt.grid()
#plt.legend()
#plt.savefig( './plots/gr.svg', format='svg', dpi=1200)
