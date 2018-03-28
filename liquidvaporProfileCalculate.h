/*
   xCOM /= fluidCount;
   yCOM /= fluidCount;
   zCOM /= fluidCount;
// COM calculation of interface profile
for ( i = fluid_index[0] ; i <= fluid_index[fluidCount-1] ; ++i ) {

if ( particles[i].r.X > xCOM ){

segPlane_ind = round( ( particles[i].r.Z - segPlane_zMin ) / segPlane_zDelta );

segPlane_xCOM[segPlane_ind] += particles[i].r.X;
segPlane_zCOM[segPlane_ind] += particles[i].r.Z;

segPlane_count[segPlane_ind] += 1;
}
}

// reset variables to 0
xCOM = 0.0;
yCOM = 0.0;
zCOM = 0.0;
 */

// Calculating interface profile: BRUTE FORCE
//for ( i = fluid_index[0] ; i <= fluid_index[fluidCount-1] ; ++i ) {

nZ_indz = round( ( particles[i].r.Z - nZ_zMin ) / nZ_zbinWidth );
nZ_indx = round( ( particles[i].r.X - nZ_xMin ) / nZ_xbinWidth );

if ( ( nZ_indz < 0 ) || ( nZ_indz > nZ_zbins - 1 ) || ( nZ_indx < 0 ) || ( nZ_indx > nZ_xbins - 1 ) ){
	std::cout << " Out of bounds in nZ calculation : " << " nZ_indz: " << nZ_indz << ", nZ_indx: " << nZ_indx << ", particle position: " << particles[i].r <<  std::endl;abort();}

nZ[nZ_indz][nZ_indx] += 1;

//}
