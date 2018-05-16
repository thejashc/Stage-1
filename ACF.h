/*
for ( n_vars_counter=0 ; n_vars_counter<n_vars ; ++n_vars_counter ){
	// std::cout << "step= " << step  << std::endl;
	recursive_addCorr(xind, n_vars_counter, 1);		// send data to the type-i correlator at level=0
	// std::cout << "n_vars_counter= " << n_vars_counter << " done" << std::endl; 
	// std::cout << std::endl;

}
*/

// calculate components for stress-autocorrelation : 1 instead of pTensCounter
Sxx  = rho * ( vx2 / npart ) + ( pNonIdeal_temp[0][0] + pDissipative_temp[0][0] + pRandom_temp[0][0] ) / ( 1. * volume );
Syy  = rho * ( vy2 / npart ) + ( pNonIdeal_temp[1][1] + pDissipative_temp[1][1] + pRandom_temp[1][1] ) / ( 1. * volume );
Szz  = rho * ( vz2 / npart ) + ( pNonIdeal_temp[2][2] + pDissipative_temp[2][2] + pRandom_temp[2][2] ) / ( 1. * volume );

Sxy  = ( pNonIdeal_temp[0][1] + pDissipative_temp[0][1] + pRandom_temp[0][1] ) / ( 1. * volume );
Syz  = ( pNonIdeal_temp[1][2] + pDissipative_temp[1][2] + pRandom_temp[1][2] ) / ( 1. * volume );
Szx  = ( pNonIdeal_temp[2][0] + pDissipative_temp[2][0] + pRandom_temp[2][0] ) / ( 1. * volume );

Nxy  = Sxx - Syy;
Nyz  = Syy - Szz;
Nzx  = Szz - Sxx;

//if ( step % 20000 == 0 ){
//	std::cout << "Sxx= " << Sxx << ", Sxy= " << Sxy << ", Sxz= " << Szx << ", Syy=" << Syy << ", Syz= " << Syz << ", Szz= " << Szz << ", Nxy= " << Nxy << ", Nyz= " << Nyz << ", Nzx= " << Nzx << std::endl;
//}

// call the auto-correlation function
recursive_addCorr( Sxy, 0, 0, 1 );
//recursive_addCorr( Syz, 1, 1 );
//recursive_addCorr( Szx, 2, 1 );
//recursive_addCorr( Nxy, 3, 1 );
//recursive_addCorr( Nyz, 4, 1 );
//recursive_addCorr( Nzx, 5, 1 );

normalizeCorr[0] += Sxy * Sxy;
//normalizeCorr[1] += Syz * Syz;
//normalizeCorr[2] += Szx * Szx;
//normalizeCorr[3] += Nxy * Nxy;
//normalizeCorr[4] += Nyz * Nyz;
//normalizeCorr[5] += Nzx * Nzx;

normalizeCorr_count += 1;

//std::cout << Sxy << std::endl;

pTensCounter = 0;	// set counter back to 0
