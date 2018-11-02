// Set max and min dimensions of boxy
xind_min = 0.25;
yind_min = 0.25;
zind_min = 0.25;
xind_max = boxEdge[x];
yind_max = boxEdge[y];
zind_max = boxEdge[z];

npart = round( initRho * boxEdge[x] * boxEdge[y] * boxEdge[z] );

#if MULTI_VISCOSITY_LIQUIDS
    fluidType1_idxStart = 0;
    fluidType1_idxEnd   = ( npart / 2 ) - 1;
    fluidType2_idxStart = npart / 2;
    fluidType2_idxEnd   = npart - 1;
#endif 

/*
aCube = pow( 1. / initRho, 1./3. );

xind = xind_min;
// Particle position intialization in a crystal structure 
while ( xind < xind_max){
	yind = yind_min;
	while( yind < yind_max){
		zind = zind_min;
		while( zind < zind_max){
			// generate random velocities
			rand_gen_velx = ((double) rand() / (RAND_MAX));
			rand_gen_vely = ((double) rand() / (RAND_MAX));
			rand_gen_velz = ((double) rand() / (RAND_MAX));

			// initializing particle radius, mass, position and velocity
			particles.push_back({1.0,1.0,{xind, yind, zind},{rand_gen_velx, rand_gen_vely, rand_gen_velz}, 1});

			// update zind
			zind += aCube*rcutoff;

		}// end of zind
		yind += aCube*rcutoff;
	}// end of yind			
	xind += aCube*rcutoff;
}// end of xind
*/


#if MULTI_VISCOSITY_LIQUIDS
    simProg << "***************************************************" << std::endl;
    simProg << "Started initialization of the multi-component FCC crystal (positions and attractive powers)" << std::endl;

    simProg << "The starting x-coordinate of the crystal: "<< xind_min << std::endl;
    simProg << "The starting y-coordinate of the crystal: "<< yind_min << std::endl;
    simProg << "The starting z-coordinate of the crystal: "<< zind_min << std::endl;

    simProg << "The ending x-coordinate of the crystal: "<< xind_max << std::endl;
    simProg << "The ending y-coordinate of the crystal: "<< yind_max << std::endl;
    simProg << "The ending z-coordinate of the crystal: "<< zind_max << std::endl;

    pCount = 0;
    for ( i = fluidType1_idxStart; i <= fluidType1_idxEnd; ++i ){

        // generate random velocities
        rand_gen_velx = ((double) rand() / (RAND_MAX));
        rand_gen_vely = ((double) rand() / (RAND_MAX));
        rand_gen_velz = ((double) rand() / (RAND_MAX));

        // generate random positions
        xind = ((double) rand() / (RAND_MAX)) * ( boxEdge[x] );
        yind = ((double) rand() / (RAND_MAX)) * ( boxEdge[y] );
        zind = ((double) rand() / (RAND_MAX)) * ( 0.5 * boxEdge[z] );

        // initializing particle radius, mass, position and velocity
        particles.push_back({1.0,1.0,{xind, yind, zind},{rand_gen_velx, rand_gen_vely, rand_gen_velz}, 1});
        
        pCount++;
    }

    simProg << "finished initialization of " << pCount << " type 1 particles inside crystal lattice" << std::endl;
    simProg << "***************************************************" << std::endl;

    pCount = 0;
    for ( i = fluidType2_idxStart; i <= fluidType2_idxEnd; ++i ){

        // generate random velocities
        rand_gen_velx = ((double) rand() / (RAND_MAX));
        rand_gen_vely = ((double) rand() / (RAND_MAX));
        rand_gen_velz = ((double) rand() / (RAND_MAX));

        // generate random positions
        xind = ((double) rand() / (RAND_MAX)) * ( boxEdge[x] );
        yind = ((double) rand() / (RAND_MAX)) * ( boxEdge[y] );
        zind = 0.5 * boxEdge[z] + ((double) rand() / (RAND_MAX)) * ( 0.5 * boxEdge[z] );

        // initializing particle radius, mass, position and velocity
        particles.push_back({1.0,1.0,{xind, yind, zind},{rand_gen_velx, rand_gen_vely, rand_gen_velz}, 2});
        
        pCount++;
    }

    simProg << "finished initialization of " << pCount << " type 2 particles inside crystal lattice" << std::endl;
    simProg << "***************************************************" << std::endl;

#else
    simProg << "***************************************************" << std::endl;
    simProg << "Started initialization of the single-component FCC crystal" << std::endl;

    simProg << "The starting x-coordinate of the crystal: "<< xind_min << std::endl;
    simProg << "The starting y-coordinate of the crystal: "<< yind_min << std::endl;
    simProg << "The starting z-coordinate of the crystal: "<< zind_min << std::endl;

    simProg << "The ending x-coordinate of the crystal: "<< xind_max << std::endl;
    simProg << "The ending y-coordinate of the crystal: "<< yind_max << std::endl;
    simProg << "The ending z-coordinate of the crystal: "<< zind_max << std::endl;

    for ( i=1; i <= npart; ++i ){
        // generate random positions
        xind = ((double) rand() / (RAND_MAX)) * ( boxEdge[x] );
        yind = ((double) rand() / (RAND_MAX)) * ( boxEdge[y] );
        zind = ((double) rand() / (RAND_MAX)) * ( boxEdge[z] );

        // generate random velocities
        rand_gen_velx = ((double) rand() / (RAND_MAX));
        rand_gen_vely = ((double) rand() / (RAND_MAX));
        rand_gen_velz = ((double) rand() / (RAND_MAX));

        // initializing particle radius, mass, position and velocity
        particles.push_back({1.0,1.0,{xind, yind, zind},{rand_gen_velx, rand_gen_vely, rand_gen_velz}, 1});
    } 

simProg << "finished initialization of  " << npart << " particles inside crystal lattice" << std::endl;
simProg << "***************************************************" << std::endl;
#endif

