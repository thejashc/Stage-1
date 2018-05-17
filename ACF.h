/*
for ( n_vars_counter=0 ; n_vars_counter<n_vars ; ++n_vars_counter ){
	// std::cout << "step= " << step  << std::endl;
	recursive_addCorr(xind, n_vars_counter, 1);		// send data to the type-i correlator at level=0
	// std::cout << "n_vars_counter= " << n_vars_counter << " done" << std::endl; 
	// std::cout << std::endl;

}
*/

// calculate components for stress-autocorrelation : 1 instead of pTensCounter
// Sxx  = rho * ( vx2 / npart ) + ( pNonIdeal_temp[0][0] + pDissipative_temp[0][0] + pRandom_temp[0][0] ) / ( 1. * volume );
// Syy  = rho * ( vy2 / npart ) + ( pNonIdeal_temp[1][1] + pDissipative_temp[1][1] + pRandom_temp[1][1] ) / ( 1. * volume );
// Szz  = rho * ( vz2 / npart ) + ( pNonIdeal_temp[2][2] + pDissipative_temp[2][2] + pRandom_temp[2][2] ) / ( 1. * volume );

SxyC   =  pNonIdeal_temp[0][1]     / ( volume );		// conservative force contribution to Sxy
SxyR   =  pRandom_temp[0][1]       / ( volume );		// random force contribution to Sxy
SxyD   =  pDissipative_temp[0][1]  / ( volume );		// dissipative force contribution to Sxy
SxyVxy =  vxvy / volume;					// sum of v_{i,x} * v_{i,y} / volume

// Syz  = ( pNonIdeal_temp[1][2] + pDissipative_temp[1][2] + pRandom_temp[1][2] ) / ( 1. * volume );
// Szx  = ( pNonIdeal_temp[2][0] + pDissipative_temp[2][0] + pRandom_temp[2][0] ) / ( 1. * volume );

// Nxy  = Sxx - Syy;
// Nyz  = Syy - Szz;
// Nzx  = Szz - Sxx;

// std::cout << "Sxx= " << Sxx << ", Sxy= " << Sxy << ", Sxz= " << Szx << ", Syy=" << Syy << ", Syz= " << Syz << ", Szz= " << Szz << ", Nxy= " << Nxy << ", Nyz= " << Nyz << ", Nzx= " << Nzx << std::endl;
// std::cout << SxyC << "\t" << SxyR << "\t" << SxyD << std::endl;


//		Sxy	=	| Sxy_CC	Sxy_CR		Sxy_CD	|	Sxy matrix with all stress components
//				| Sxy_RC	Sxy_RR		Sxy_RD	|
//				| Sxy_DC	Sxy_DR		Sxy_DD	|			

// call the auto-correlation function
recursive_addCorr( SxyC,   0, 1 );
recursive_addCorr( SxyR,   1, 1 );
recursive_addCorr( SxyD,   2, 1 );
//recursive_addCorr( Sxyvxy, 3, 1 );

//recursive_addCorr( Syz, 1, 1 );
//recursive_addCorr( Szx, 2, 1 );
//recursive_addCorr( Nxy, 3, 1 );
//recursive_addCorr( Nyz, 4, 1 );
//recursive_addCorr( Nzx, 5, 1 );

// cyclic order - C->R->D->C

normalizeCorr[0] += SxyC * SxyC;
normalizeCorr[1] += SxyC * SxyR;
normalizeCorr[2] += SxyC * SxyD;

normalizeCorr[3] += SxyR * SxyR;
normalizeCorr[4] += SxyR * SxyD;
normalizeCorr[5] += SxyR * SxyC;

normalizeCorr[6] += SxyD * SxyD;
normalizeCorr[7] += SxyD * SxyC;
normalizeCorr[8] += SxyD * SxyR;

//normalizeCorr[9] += SxyVxy * SxyVxy;

//normalizeCorr[1] += Syz * Syz;
//normalizeCorr[2] += Szx * Szx;
//normalizeCorr[3] += Nxy * Nxy;
//normalizeCorr[4] += Nyz * Nyz;
//normalizeCorr[5] += Nzx * Nzx;

normalizeCorr_count += 1;

//std::cout << Sxy << std::endl;

pTensCounter = 0;	// set counter back to 0
