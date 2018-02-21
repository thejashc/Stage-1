pIdeal[0][0] += rho * ( vx2 / npart ) ;		// equipartition theorem
pIdeal[1][1] += rho * ( vy2 / npart ) ;
pIdeal[2][2] += rho * ( vz2 / npart ) ;

/*
pTensor[0][0] += pIdeal[0][0] + ( 1. / volume ) * pNonIdeal[0][0];	// pxx
pTensor[0][1] += pIdeal[0][1] + ( 1. / volume ) * pNonIdeal[0][1];	// pxy
pTensor[0][2] += pIdeal[0][2] + ( 1. / volume ) * pNonIdeal[0][2];	// pxz

pTensor[1][0] += pIdeal[1][0] + ( 1. / volume ) * pNonIdeal[1][0];	// pyx
pTensor[1][1] += pIdeal[1][1] + ( 1. / volume ) * pNonIdeal[1][1];	// pyy
pTensor[1][2] += pIdeal[1][2] + ( 1. / volume ) * pNonIdeal[1][2];	// pyz

pTensor[2][0] += pIdeal[2][0] + ( 1. / volume ) * pNonIdeal[2][0];	// pzx
pTensor[2][1] += pIdeal[2][1] + ( 1. / volume ) * pNonIdeal[2][1];	// pzy
pTensor[2][2] += pIdeal[2][2] + ( 1. / volume ) * pNonIdeal[2][2];	// pzz
*/
