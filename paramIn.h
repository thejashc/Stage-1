// set global parameters
boxEdge[x] = 10.0;
boxEdge[y] = 10.0;
boxEdge[z] = 10.0;

// Half box size
boxHalve[x] = boxEdge[x] / 2.0;
boxHalve[y] = boxEdge[y] / 2.0;
boxHalve[z] = boxEdge[z] / 2.0;

// Reciprocal box size
boxRecip[x] = 1.0 / boxEdge[x];
boxRecip[y] = 1.0 / boxEdge[y];
boxRecip[z] = 1.0 / boxEdge[z];

kBT = 1.0;			// DPD fluid temperature 
Aij = -40.0;			// DPD Warren conservative force -- attractive parameter
Bij = +40.0;			// DPD Warren conservative force -- repulsive parameter
#if RANDOM_DISSIPATIVE
sigma = 3.0;			// DPD random force parameter
gamma = pow( sigma,2.0 )/( 2.0 * kBT ); // DPD dissipative force parameter
#endif
rcutoff = 1.0;			// cut-off distance for pairwise attraction
rd_cutoff = 0.75;			// cut-off distance for pairwise repulsion
fifteen_by_twopi_by_rd = 15.0/( 2.0 * M_PI * pow( rd_cutoff,3.0) );	// 15/(2*PI*rd^3) used in Lucy weight function
fifteen_by_twopi_by_rc = 15.0/( 2.0 * M_PI * pow( rcutoff,3.0 ) );	// 15/(2*PI*rc^3) used in Lucy weight function
rc2 = pow( rcutoff , 2);	// square of cut-off distance
rd2 = pow( rd_cutoff,2);	// square of cut-off distance
dim = 3;				// 3D system

dt = 0.001;
step = 1;
stepMax = 2e6;

// post-processing
// g(r) calculation
gR_radMin = 0.0;			// minimum radius for g(r) 
gR_radDelta = 0.01;			// thickness of a shell
gR_radMax = boxEdge[x] / 2.0;		// maximum radius for g(r)
gR_nElem  = round( ( gR_radMax - gR_radMin )/ gR_radDelta); // number of elements

gR_tStart = 5e4;
gR_tDelta = 100;
gR_tEnd = stepMax;
gR_tSamples = round(( gR_tEnd - gR_tStart )/(gR_tDelta)) - 1;

// velocity distribution
velHist_velMin = -25.0;			// minimum velocity 
velHist_velDelta = 0.005;		// resolution of DeltaVel
velHist_velMax = 25.0;			// maximum velocity
velHist_bins  = round( ( velHist_velMax - velHist_velMin)/velHist_velDelta); // number of elements

velHist_tStart = 5e4;
velHist_tDelta = 100;
velHist_tEnd = stepMax;

// density distribution along z
#if PLANAR_SLAB
rhoZ_Zmin 	= 0.00;		// minimum Z
rhoZ_Zdelta 	= 0.05;		// resolution of Delta Z 
rhoZ_Zmax 	= boxEdge[z];		// maximum Z
rhoZ_bins  	= round( ( rhoZ_Zmax - rhoZ_Zmin ) / rhoZ_Zdelta ); // number of elements
#elif CYLINDER_DROPLET
rhor_rmin 	= 0.00;
rhor_rdelta 	= 0.05;
rhor_rmax 	= boxEdge[x];
rhor_bins  	= round( ( rhor_rmax - rhor_rmin ) / rhor_rdelta ); // number of elements

#endif
