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

# KNOBS
ENERGY		    = 0
MOMENTUM	    = 0
TEMPERATURE	    = 0
VEL_PROFILE	    = 0
GR		    = 0 
VEL_DIST	    = 0
PRESSURE_TENSOR     = 0
ACF                 = 0
SACF_GK             = 1

# parameters
nFluid 		= 4374
yMin 		= 0.
yMax 		= 9.
ybinWidth 	= 0.5

startFileNum  	= 100000
endFileNum    	= 2840000
deltaFileNum  	= 1000

#------ ENERGY STATISTICS --------#
if ( ENERGY == 1 ):
	data = np.loadtxt( './data/en_data.dat' )

	# raw data
	pe      = data[:,0]
	ke      = data[:,1]
	te      = data[:,2]

	# average energy values
	peAvg 	= np.mean( pe )
	keAvg 	= np.mean( ke )
	teAvg 	= np.mean( te )

	# deviation from mean
	peDev 	= np.std(pe) / peAvg
	keDev 	= np.std(ke) / keAvg
	teDev	= np.std(te) / teAvg

	print ( 'pe fluctuation = '+str( abs( peDev * 100 ) ) )
	print ( 'ke fluctuation = '+str( abs( keDev * 100 ) ) )
	print ( 'te fluctuation = '+str( abs( teDev * 100 ) ) )

	figEnv()
	plt.title('Energy vs time')
	#plt.plot( keDev	, label= r'$KE$' )
	#plt.plot( peDev	, label= r'$PE$' )
	#plt.plot( totDev, label= r'$TE$' )
	#plt.plot( pe, 'rs', label= r'$PE$' )
	plt.plot( ke, 'b^', label= r'$KE$' )
	#plt.plot( te, 'g-', label= r'$TE$' )
	plt.xlabel( r'$t$' )
	plt.ylabel( r'$\frac{\Delta E}{\langle E \rangle}$' )
	plt.legend()
	plt.grid()
	plt.savefig( './plots/energy.eps', format='eps', dpi=1200 )
	#plt.show()

#------ MOMENTUM STATISTICS --------#
if ( MOMENTUM == 1 ) : 
	data = np.loadtxt( './data/mom_data.dat' )

	# storing values
	momX = data[:, 0]
	momY = data[:, 1]
	momZ = data[:, 2]

	figEnv()
	plt.plot( momX, label='$p_{x}$' )
	plt.plot( momY, label='$p_{y}$' )
	plt.plot( momZ, label='$p_{z}$' )
	plt.xlabel( r'$t$' )
	plt.ylabel( r'$p_{\alpha}$' )
	plt.ylim( (-1e-10,1e-10) )
	plt.grid()
	plt.legend()
	plt.savefig( './plots/momentum.eps', format='eps', dpi=1200)

#------------- TEMPERATURE AND PRESSURE -------------#
if ( TEMPERATURE == 1 ): 
	data = np.loadtxt( './data/eos_data.dat' )

	time 	= data[:,0]
	Temp	= data[:,1]
	Pressure= data[:,2]

	figEnv()
	plt.plot( Temp, label='<T>' )
	plt.xlabel( '$t$' )
	plt.ylabel( '$T$' )
	plt.grid()
	plt.legend()
	plt.savefig( './plots/temp.eps', format='eps', dpi=1200 )

##------------- DENSITY PROFILE-------------#
#filelist=[]
#
#for i in np.arange(40,80,5):
#    filelist.append("./data/rhoZ_%s.dat" %(i * 1000) )
#
# setting up matrices for averaging
#data = np.loadtxt( "./data/rhoZ_10000.dat" )
#z 		= data[:,0]
#vol		= data[:,1]
#m	 	= len(z) 
#rhoZCount 	= np.zeros(m)
#counter 	= np.zeros(m)
#
#for fname in filelist:
#	data = np.loadtxt( fname )
#	rhoZCount 	= rhoZCount + data[:,2]
#	counter 	= counter + data[:,3]
#
#figEnv()	
#rhoZ 		= ( rhoZCount ) / ( counter * vol )
#plt.plot( z, rhoZ )
#plt.title ( r'$ \rho(z)\ vs\ z $' ) 
#plt.xlabel( r'$ z $' )
#plt.ylabel( r'$ \rho $' )
#plt.grid()
#plt.savefig( './plots/rhoZ_vs_Z.eps', format='eps', dpi=1200 )

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
#plt.savefig( './plots/pTens.eps', format='eps', dpi=1800 )
#exit()

#------- VELOCITY DISTRIBUTION ------#
if ( VEL_DIST == 1 ):
	data = np.genfromtxt( './data/velDist_data.dat', skip_header=1 )

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
if ( GR == 1 ):
	data = np.genfromtxt( './data/gr_data.dat', skip_header=1 )

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

# ----------------- VELOCITY PROFILE -----------------#
if ( VEL_PROFILE == 1):

	ybins = int( ( yMax - yMin ) / ybinWidth )

	v_vs_Y = np.zeros( (ybins,3) )
	temp_vs_Y = np.zeros( (ybins,3) )
	count   = np.zeros( (ybins,3) )
	ydata   = np.arange( yMin, yMax, ybinWidth )

	# data processing
	for i in np.arange(startFileNum, endFileNum, deltaFileNum ):
		print('Processing file for timestep = %d'%i ) 	

		velData = np.genfromtxt( './data/velocity%d.dat'%i, skip_header=2 )
		posData = np.genfromtxt( './data/XYZ%d.xyz'%i     , skip_header=2 )

		#data 
		rx = posData[:,1]
		ry = posData[:,2]
		rz = posData[:,3]

		vx = velData[:,1]
		vy = velData[:,2]
		vz = velData[:,3]

		indy = np.floor( ( ry - yMin )  / ybinWidth )
		indy = indy.astype(int)
		indRem = np.where( ry >= yMax)[0]
		indy = np.delete( indy, indRem )

		missParticle = indRem.size
		if ( missParticle > 3 ):
			print( 'More than 3 particles out of box!')
			exit() 

		#if ( i == 81000 ): 
			#print( ry [ np.where( indy >= 25. )  ] )
			#print( ry [ np.where( indy < 0. )  ] )
			#print( indy [ np.where( indy < 0. )  ] )
			#exit()

		for i in np.arange(0, nFluid-missParticle, 1):	
			v_vs_Y[indy[i], 0] += vx[i] 
			v_vs_Y[indy[i], 1] += vy[i] 
			v_vs_Y[indy[i], 2] += vz[i] 

			count[indy[i], 0] += 1		
			count[indy[i], 1] += 1		
			count[indy[i], 2] += 1		

	# final processing
	tot_files = ( ( endFileNum - startFileNum ) / deltaFileNum ) + 1 
	rho = np.mean( count, axis = 1 )
	print( count.shape, rho.shape )
	v_vs_Y /= count 
	rho_vs_Y = rho / ( tot_files * ybinWidth * yMax * yMax )

	print( ydata ) 

	figEnv()
	plt.plot( ydata, v_vs_Y[:,0], 'rs-', label=r'v_{x}' )
	plt.xlabel( r'$y$' )
	plt.ylabel( r'$v_{x}$' )
	plt.grid()
	plt.legend()
	plt.savefig( './plots/vx_vs_y.eps', format='eps', dpi=1200)

	figEnv()
	plt.plot( ydata, rho_vs_Y, 'rs-', label=r'$\rho$' )
	plt.xlabel( r'$y$' )
	plt.ylabel( r'$v_{x}$' )
	plt.grid()
	plt.legend()
	plt.savefig( './plots/rho_vs_y.eps', format='eps', dpi=1200)

	# finding temperature
	for i in np.arange(startFileNum, endFileNum, deltaFileNum ):
		print('Temperatue: Processing file for timestep = %d'%i ) 	

		velData = np.genfromtxt( './data/velocity%d.dat'%i, skip_header=2 )
		posData = np.genfromtxt( './data/XYZ%d.xyz'%i     , skip_header=2 )

		#data 
		rx = posData[:,1]
		ry = posData[:,2]
		rz = posData[:,3]

		vx = velData[:,1]
		vy = velData[:,2]
		vz = velData[:,3]

		indy = np.floor( ( ry - yMin )  / ybinWidth )
		indy = indy.astype(int)
		indRem = np.where( ry >= yMax)[0]
		indy = np.delete( indy, indRem )

		missParticle = indRem.size
		if ( missParticle > 3 ):
			print( 'More than 3 particles out of box!')
			exit() 

		for i in np.arange(0, nFluid-missParticle, 1):	
			temp_vs_Y[indy[i], 0] += np.power( vx[i] - v_vs_Y[indy[i], 0], 2.) 
			temp_vs_Y[indy[i], 1] += np.power( vy[i] - v_vs_Y[indy[i], 1], 2.) 
			temp_vs_Y[indy[i], 2] += np.power( vz[i] - v_vs_Y[indy[i], 2], 2.) 

	temp_vs_Y /= count
	meanTemp = np.mean( temp_vs_Y[:] )

	print( 'mean temperature is = %f'%meanTemp )

	figEnv()
	plt.plot( ydata, temp_vs_Y[:,0], 'rs-', label=r'$k_{B}T_{x}$' )
	plt.plot( ydata, temp_vs_Y[:,1], 'ks-', label=r'$k_{B}T_{y}$' )
	plt.plot( ydata, temp_vs_Y[:,2], 'bs-', label=r'$k_{B}T_{z}$' )
	plt.xlabel( r'$y$' )
	plt.ylabel( r'$k_{B}T$' )
	plt.grid()
	plt.legend()
	plt.savefig( './plots/temp_vs_y.eps', format='eps', dpi=1200)

	# writing the velocity data
	wrtData = np.array( ( ydata, v_vs_Y[:,0] ) )
	wrtData = wrtData.transpose()
	np.savetxt( './data/velProfile.dat', wrtData, fmt='%1.7f' )

# ----------------- PRESSURE_TENSOR PROFILE -----------------#
if ( PRESSURE_TENSOR == 1):

	data = np.loadtxt('./data/pTens.dat')
	indStart = 10

	pxyC = data[:,1] + data[:,10]
	pyxC = data[:,3] + data[:,12]

	pC = ( pxyC + pyxC ) / 2.

	pxyD = data[:,19]
	pyxD = data[:,21]

	pD = ( pxyD + pyxD ) / 2.

	pxyR = data[:,28]
	pyxR = data[:,30]

	pR = ( pxyR + pyxR ) / 2.

	v_vs_Y = np.loadtxt('./data/velProfile.dat')
	Delv = v_vs_Y[-1,1] - v_vs_Y[0,1]

	yLow = ( 2*v_vs_Y[0,0]  + ybinWidth ) * 0.5			# equivalent to ( 1st + 2nd ) / 2 
	yHigh = ( yMax + v_vs_Y[-1,0] ) * 0.5				# equivalent to ( upper-limit + last-element ) / 2 

	print( yLow, yHigh )
	vGrad = Delv/ ( yHigh - yLow ) 
	vGradInput = 0.13

	pCMean = np.mean( pC[indStart:] )
	pDMean = np.mean( pD[indStart:] )
	pRMean = np.mean( pR[indStart:] )

	pCStd = np.std( pC[indStart:] )
	pDStd = np.std( pD[indStart:] )
	pRStd = np.std( pR[indStart:] )

	pxy     = pC[indStart:] + pD[indStart:] + pR[indStart:]
	pxyMean =  np.mean( pxy[:] )
	pxyStd  =  abs( np.std( pxy[:] ) )

	viscosity    = ( -1. * pxyMean ) / vGradInput
	viscosityUp  = ( -1. * ( pxyMean + pxyStd ) ) / vGradInput
	viscosityLow = ( -1. * ( pxyMean - pxyStd ) ) / vGradInput
	
	print( 'vMax=%f'%np.max( v_vs_Y[:,1] ) )	
	print( 'vMin=%f'%np.min( v_vs_Y[:,1] ) )	
	print( 'vGrad=%f'%vGrad )

	print( 'pC= '+str(pCMean)+' +/- '+str(pCStd) )	
	print( 'pD= '+str(pDMean)+' +/- '+str(pDStd) )	
	print( 'pR= '+str(pRMean)+' +/- '+str(pRStd) )	

	print( 'Lower limit Visc= '+str(viscosityLow)+', Mean Viscosity = '+str(viscosity)+', Viscosity Higher = '+str(viscosityUp) )

	figEnv()
	plt.plot( pxyC, 'r-', label=r'$p_{xy, C}$')
	plt.plot( pyxC, 'bs', label=r'$p_{yx, C}$')

	plt.plot( pxyD, 'r-', label=r'$p_{xy, D}$')
	plt.plot( pyxD, 'b^', label=r'$p_{yx, D}$')

	plt.plot( pxyR, 'r-', label=r'$p_{xy, R}$')
	plt.plot( pyxR, 'bo', label=r'$p_{yx, R}$')

	plt.xlabel( r'$t$' )
	plt.ylabel( r'$p_{\alpha\beta}$' )
	plt.title ( r'$\eta$ = %.5f'%viscosity )
	plt.grid()
	plt.legend()
	plt.savefig( './plots/pTens.eps', format='eps', dpi=1200)

# ----------------- AUTOCORRELATION PROFILE -----------------#
if ( ACF == 1):

	p64dat = np.loadtxt   ( './cbp7_li_ForLi/mycorrelationData_p_64.dat') 
	p16dat = np.loadtxt   ( './cbp7_li_ForLi/mycorrelationData_p_16.dat') 
	p8dat  = np.loadtxt   ( './cbp7_li_ForLi/mycorrelationData_p_8.dat') 
	ref    = np.genfromtxt( './cbp7_li_ForLi/matlab_autocorr.dat', delimiter=',' )
	#raw  = np.loadtxt( './cbp7_li_ForLi/durai_com.dat')

	lag64     = p64dat[:,0]
	acf64     = p64dat[:,1] / p64dat[0,1]

	lag16     = p16dat[:,0]
	acf16     = p16dat[:,1] / p16dat[0,1]

	lag8      = p8dat[:,0]
	acf8      = p8dat[:,1] / p8dat[0,1]

	reflag  = ref[:,0]
	refacf  = ref[:,1]

	#acfNumpy = numpy.correlate(raw, raw, mode='valid')
	#np.savetxt( './data/acfNumpy.dat', acfNumpy, fmt='%1.7f' )

	figEnv()
	plt.plot(    lag64,    acf64, '.' , label=r'p=64')	
	plt.plot(    lag16,    acf16, '.' , label=r'p=16')	
	plt.plot(    lag8,     acf8, '.'  , label=r'p=8' )	
	plt.plot(    reflag, refacf, 'r-' )	
	plt.xlim( (0. , 1e6 ) )

	plt.xlabel( r'$\tau$' )
	plt.ylabel( r'$CF$' )
	plt.legend()
	plt.title ( r'Correlation function -- Harmonic Oscillator + Brownian' )
	plt.grid()
	plt.savefig( './plots/corrData.eps', format='eps', dpi=1200)
# ----------------- STRESS AUTOCORRELATION PROFILE -----------------#
if ( SACF_GK == 1):

	Sxy   = np.loadtxt ( './data/correlationData.dat')
	#Sxy   = np.loadtxt ( './autocrossmatlabCompare/correlationData.dat')

	# add the zer-correlation data to Sxy
	newRow = [0]
	for i in np.arange(2, 34, 2):
		newRow.append(Sxy[0,i])
		newRow.append(Sxy[0,i])

	Sxy    = np.vstack([newRow, Sxy])
	dt     = 1e-3
	lag    = Sxy[:,0] 
	volume = 216
	kbT    = 1.0

	Ar     = np.zeros( (Sxy.shape[0], 16) );

	# find sum of correlations
	for j in np.arange(1,33,2):
		for i in np.arange(0, Sxy.shape[0], 1):
			Ar[i, int( (j-1)/2 ) ] = np.trapz( Sxy[0:i,j], Sxy[0:i, 0] * dt, dt)

	etaInf = ( volume / kbT ) * ( dt / 2. ) * Sxy[0,8]
	eta1   = ( volume / kbT ) *  np.sum( Ar, axis=1 )
	eta2   = ( volume / kbT ) * ( Ar[:,0] + Ar[:,13] + Ar[:,10] + Ar[:,3] + Ar[:,12] + Ar[:,9] - Ar[:,2] - Ar[:,15] - Ar[:,8] ) + etaInf			# Ernst
	eta3   = ( volume / kbT ) * ( Ar[:,0] + Ar[:,13] + Ar[:,3] + Ar[:,12] + Ar[:,8] )									# Espanol
	eta4   = eta3 + etaInf																	# Espanol + EtaInf

	figEnv()
	plt.semilogx(     lag,    eta1,   '.-', label=r'$\eta$ - GK = %f'%(eta1[196]), ms=2.5, mfc="none" )
	plt.semilogx(     lag,    eta2,   '^-', label=r'$\eta$ - Ernst = %f'%(eta2[196]), ms=2.5 , mfc="none")
	plt.semilogx(     lag,    eta3,   'o-', label=r'$\eta$ - Espanol = %f'%(eta3[196]), ms=2.5, mfc="none" )
	plt.semilogx(     lag,    eta4,   's-', label=r'$\eta$ - Espanol + $\eta_{\infty}$=%f'%(eta4[196]), ms=2.5, mfc="none" )

	#print( 'viscosity=%f'%(viscosity) )

	plt.xlabel( r'$\tau$ (mDPD units)' )
	#plt.ylabel( r'$\langle \sigma_{xy}(0)\sigma_{xy}(\tau) \rangle$ (normalized)' )
	plt.legend()
	#plt.title ( r'Correlation function -- stress' )
	plt.grid()
	plt.savefig( './plots/viscosity.eps', format='eps', dpi=1200)

	plt.rcParams.update({'font.size': 5.0})
	# individual elements
	lt = 1.5
	f, ((ax1, ax2 ), (ax3, ax4),  ( ax5, ax6), (ax7, ax8), (ax9, ax10), (ax11, ax12),(ax13, ax14),(ax15, ax16)) = plt.subplots(8, 2, figsize=(12, 42) )
	
	ax1.semilogx(lag, Sxy[:,1], label=r'$S_{xy}^{C1\ C1}$', lw=lt)
	ax2.semilogx(lag, Sxy[:,3], label=r'$S_{xy}^{R\ C1}$', lw=lt)
	ax3.semilogx(lag, Sxy[:,5], label=r'$S_{xy}^{R\ C1}$', lw=lt)
	ax4.semilogx(lag, Sxy[:,7], label=r'$S_{xy}^{C2\ C1}$', lw=lt)

	ax5.semilogx(lag, Sxy[:,9], label=r'$S_{xy}^{R\ R}$', lw=lt)
	ax6.semilogx(lag, Sxy[:,11], label=r'$S_{xy}^{D\ R}$', lw=lt)
	ax7.semilogx(lag, Sxy[:,13], label=r'$S_{xy}^{C2\ R}$', lw=lt)
	ax8.semilogx(lag, Sxy[:,15], label=r'$S_{xy}^{C1\ R}$', lw=lt)

	ax9.semilogx(lag, Sxy[:,17], label=r'$S_{xy}^{D\ D}$', lw=lt)
	ax10.semilogx(lag, Sxy[:,19], label=r'$S_{xy}^{C2\ D}$', lw=lt)
	ax11.semilogx(lag, Sxy[:,21], label=r'$S_{xy}^{C1\ D}$', lw=lt)
	ax12.semilogx(lag, Sxy[:,23], label=r'$S_{xy}^{R\ D}$', lw=lt)

	ax13.semilogx(lag, Sxy[:,25], label=r'$S_{xy}^{C2\ C2}$', lw=lt)
	ax14.semilogx(lag, Sxy[:,27], label=r'$S_{xy}^{C1\ C2}$', lw=lt)
	ax15.semilogx(lag, Sxy[:,29], label=r'$S_{xy}^{R\ C2}$', lw=lt)
	ax16.semilogx(lag, Sxy[:,31], label=r'$S_{xy}^{D\ C2}$', lw=lt)

	axt1 = ax1.twinx()
	axt2 = ax2.twinx()
	axt3 = ax3.twinx()
	axt4 = ax4.twinx()
	axt5 = ax5.twinx()
	axt6 = ax6.twinx()
	axt7 = ax7.twinx()
	axt8 = ax8.twinx()
	axt9 = ax9.twinx()
	axt10 = ax10.twinx()
	axt11 = ax11.twinx()
	axt12 = ax12.twinx()
	axt13 = ax13.twinx()
	axt14 = ax14.twinx()
	axt15 = ax15.twinx()
	axt16 = ax16.twinx()

	axt1.semilogx(lag, (volume / kbT) * Ar[:,0], 'r-', lw=lt)
	axt2.semilogx(lag, (volume / kbT) * Ar[:,1], 'r-', lw=lt)
	axt3.semilogx(lag, (volume / kbT) * Ar[:,2], 'r-', lw=lt)
	axt4.semilogx(lag, (volume / kbT) * Ar[:,3], 'r-', lw=lt)
	axt5.semilogx(lag, (volume / kbT) * Ar[:,4], 'r-', lw=lt)
	axt6.semilogx(lag, (volume / kbT) * Ar[:,5], 'r-', lw=lt)
	axt7.semilogx(lag, (volume / kbT) * Ar[:,6], 'r-', lw=lt)
	axt8.semilogx(lag, (volume / kbT) * Ar[:,7], 'r-', lw=lt)
	axt9.semilogx(lag, (volume / kbT) * Ar[:,8], 'r-', lw=lt)
	axt10.semilogx(lag, (volume / kbT) * Ar[:,9], 'r-', lw=lt)
	axt11.semilogx(lag, (volume / kbT) * Ar[:,10], 'r-', lw=lt)
	axt12.semilogx(lag, (volume / kbT) * Ar[:,11], 'r-', lw=lt)
	axt13.semilogx(lag, (volume / kbT) * Ar[:,12], 'r-', lw=lt)
	axt14.semilogx(lag, (volume / kbT) * Ar[:,13], 'r-', lw=lt)
	axt15.semilogx(lag, (volume / kbT) * Ar[:,14], 'r-', lw=lt)
	axt16.semilogx(lag, (volume / kbT) * Ar[:,15], 'r-', lw=lt)

	ax1.legend()
	ax2.legend()
	ax3.legend()
	ax4.legend()
	ax5.legend()
	ax6.legend()
	ax7.legend()
	ax8.legend()
	ax9.legend()
	ax10.legend()
	ax11.legend()
	ax12.legend()
	ax13.legend()
	ax14.legend()
	ax15.legend()
	ax16.legend()

	plt.savefig( './plots/individualCorr.eps', format='eps', dpi=1200)
