pIdeal[0][0] += rho * ( vx2 / npart ) ;		// equipartition theorem
pIdeal[1][1] += rho * ( vy2 / npart ) ;
pIdeal[2][2] += rho * ( vz2 / npart ) ;

// add a pIdeal temp here as well
// Non-Ideal contribution to pressure -- Conservative forces	
pNonIdeal[0][0] 		+= pNonIdeal_temp[0][0];		// xx
pNonIdeal[0][1] 		+= pNonIdeal_temp[0][1];		// xy
pNonIdeal[0][2] 		+= pNonIdeal_temp[0][2];		// xz
pNonIdeal[1][0] 		+= pNonIdeal_temp[1][0];		// yx
pNonIdeal[1][1] 		+= pNonIdeal_temp[1][1];		// yy
pNonIdeal[1][2] 		+= pNonIdeal_temp[1][2];		// yz
pNonIdeal[2][0] 		+= pNonIdeal_temp[2][0];		// zx
pNonIdeal[2][1] 		+= pNonIdeal_temp[2][1];		// zy
pNonIdeal[2][2] 		+= pNonIdeal_temp[2][2];		// zz

// Non-Ideal contribution to pressure -- Conservative forces	
pNonIdealKin[0][0] 		+= pNonIdealKin_temp[0][0];		// xx
pNonIdealKin[0][1] 		+= pNonIdealKin_temp[0][1];		// xy
pNonIdealKin[0][2] 		+= pNonIdealKin_temp[0][2];		// xz
pNonIdealKin[1][0] 		+= pNonIdealKin_temp[1][0];		// yx
pNonIdealKin[1][1] 		+= pNonIdealKin_temp[1][1];		// yy
pNonIdealKin[1][2] 		+= pNonIdealKin_temp[1][2];		// yz
pNonIdealKin[2][0] 		+= pNonIdealKin_temp[2][0];		// zx
pNonIdealKin[2][1] 		+= pNonIdealKin_temp[2][1];		// zy
pNonIdealKin[2][2] 		+= pNonIdealKin_temp[2][2];		// zz

#if RANDOM_DISSIPATIVE
// Non-Ideal contribution to pressure -- Dissipative forces
pDissipative[0][0] 		+= pDissipative_temp[0][0];		// xx
pDissipative[0][1] 		+= pDissipative_temp[0][1];             // xy
pDissipative[0][2] 		+= pDissipative_temp[0][2];             // xz
pDissipative[1][0]		+= pDissipative_temp[1][0];             // yx
pDissipative[1][1] 		+= pDissipative_temp[1][1];             // yy
pDissipative[1][2] 		+= pDissipative_temp[1][2];             // yz
pDissipative[2][0] 		+= pDissipative_temp[2][0];             // zx
pDissipative[2][1] 		+= pDissipative_temp[2][1];             // zy
pDissipative[2][2] 		+= pDissipative_temp[2][2];             // zz

// Non-Ideal contribution to pressure -- Random forces
pRandom[0][0] 			+= pRandom_temp[0][0];			// xx
pRandom[0][1] 			+= pRandom_temp[0][1];                  // xy
pRandom[0][2] 			+= pRandom_temp[0][2];                  // xz
pRandom[1][0] 			+= pRandom_temp[1][0];                  // yx
pRandom[1][1] 			+= pRandom_temp[1][1];                  // yy
pRandom[1][2] 			+= pRandom_temp[1][2];                  // yz
pRandom[2][0] 			+= pRandom_temp[2][0];                  // zx
pRandom[2][1] 			+= pRandom_temp[2][1];                  // zy
pRandom[2][2] 			+= pRandom_temp[2][2];	                // zz
#endif	
