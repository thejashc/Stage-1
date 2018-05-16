pIdeal[0][0] += rho * ( vx2 / npart ) ;		// equipartition theorem
pIdeal[1][1] += rho * ( vy2 / npart ) ;
pIdeal[2][2] += rho * ( vz2 / npart ) ;

// add a pIdeal temp here as well

// Non-Ideal contribution to pressure -- Conservative forces	
pNonIdeal[0][0] 		+= pNonIdeal_temp[0][0];		// Sxx
pNonIdeal[0][1] 		+= pNonIdeal_temp[0][1];		// Sxy
pNonIdeal[0][2] 		+= pNonIdeal_temp[0][2];		// Sxz

pNonIdeal[1][0] 		+= pNonIdeal_temp[1][0];		// Syx
pNonIdeal[1][1] 		+= pNonIdeal_temp[1][1];		// Syy
pNonIdeal[1][2] 		+= pNonIdeal_temp[1][2];		// Syz

pNonIdeal[2][0] 		+= pNonIdeal_temp[2][0];		// Szx
pNonIdeal[2][1] 		+= pNonIdeal_temp[2][1];		// Szy
pNonIdeal[2][2] 		+= pNonIdeal_temp[2][2];		// Szz

#if RANDOM_DISSIPATIVE
// Non-Ideal contribution to pressure -- Dissipative forces
pDissipative[0][0] 		+= pDissipative_temp[0][0];		// Sxx
pDissipative[0][1] 		+= pDissipative_temp[0][1];             // Sxy
pDissipative[0][2] 		+= pDissipative_temp[0][2];             // Sxz
                                                                              
pDissipative[1][0]		+= pDissipative_temp[1][0];             // Syx
pDissipative[1][1] 		+= pDissipative_temp[1][1];             // Syy
pDissipative[1][2] 		+= pDissipative_temp[1][2];             // Syz
                                                                              
pDissipative[2][0] 		+= pDissipative_temp[2][0];             // Szx
pDissipative[2][1] 		+= pDissipative_temp[2][1];             // Szy
pDissipative[2][2] 		+= pDissipative_temp[2][2];             // Szz

// Non-Ideal contribution to pressure -- Random forces
pRandom[0][0] 			+= pRandom_temp[0][0];			// Sxx
pRandom[0][1] 			+= pRandom_temp[0][1];                  // Sxy
pRandom[0][2] 			+= pRandom_temp[0][2];                  // Sxz
                                                                              
pRandom[1][0] 			+= pRandom_temp[1][0];                  // Syx
pRandom[1][1] 			+= pRandom_temp[1][1];                  // Syy
pRandom[1][2] 			+= pRandom_temp[1][2];                  // Syz
                                                                              
pRandom[2][0] 			+= pRandom_temp[2][0];                  // Szx
pRandom[2][1] 			+= pRandom_temp[2][1];                  // Szy
pRandom[2][2] 			+= pRandom_temp[2][2];	                // Szz
#endif	
