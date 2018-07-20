#if PISTON
	xind_min = 0.;
	yind_min = 0.;
	zind_min = pistonStart; 
	xind_max = boxEdge[x];
	yind_max = boxEdge[y];
	zind_max = pistonEnd;

	pCount = 0;

	aCube = pow( 1. / initWallRho, 1./3. );

	xind = xind_min;

	simProg << "***************************************************" << std::endl;
	simProg << "Started initialization of piston" << std::endl;
	
	simProg << "The minimum x-coord of piston is: "<< xind_min << std::endl;
	simProg << "The minimum y-coord of piston is: "<< yind_min << std::endl;
	simProg << "The minimum z-coord of piston is: "<< zind_min << std::endl;
	
	simProg << "The maximum x-coord of piston is: "<< xind_max << std::endl;
	simProg << "The maximum y-coord of piston is: "<< yind_max << std::endl;
	simProg << "The maximum z-coord of piston is: "<< zind_max << std::endl;
	
	xind = xind_min;
	// Particle position intialization in a crystal structure 
	while ( xind < xind_max){
		yind = yind_min;
		while( yind < yind_max){
			zind = zind_min;
			while( zind < zind_max){
	
				// initializing particle radius, mass, position and velocity
				particles.push_back( { 1.0, 1.0, {xind, yind, zind}, {0., 0., 0.}, 0} );
				Asl.push_back( { asl } );	// lyophilic wall 
				pCount++;

				// update zind
				zind += aCube * rcutoff;

			} // end of zind
			yind += aCube * rcutoff;
		} // end of yind			
		xind += aCube * rcutoff;
	} // end of xind

	simProg << "finished initialization of  " << pCount << " inside piston" << std::endl;
	simProg << "***************************************************" << std::endl;
#endif

// Set max and min dimensions of boxy
xind_min = 0.;
yind_min = 0.;
zind_min = bufferLen + capLen + capWallWdth; 
xind_max = boxEdge[x];
yind_max = boxEdge[y];
zind_max = bufferLen + capLen + capWallWdth + resWdth;

pCount = 0;

aCube = pow( 1. / initRho, 1./3. );

xind = xind_min;

simProg << "***************************************************" << std::endl;
simProg << "Started initialization of the reservoir" << std::endl;

simProg << "The minimum x-coord of fluid particle is: "<< xind_min << std::endl;
simProg << "The minimum y-coord of fluid particle is: "<< yind_min << std::endl;
simProg << "The minimum z-coord of fluid particle is: "<< zind_min << std::endl;

simProg << "The maximum x-coord of fluid particle is: "<< xind_max << std::endl;
simProg << "The maximum y-coord of fluid particle is: "<< yind_max << std::endl;
simProg << "The maximum z-coord of fluid particle is: "<< zind_max << std::endl;

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
			pCount++;

			// update zind
			zind += aCube * rcutoff;

		}// end of zind
		yind += aCube * rcutoff;
	}// end of yind			
	xind += aCube * rcutoff;
}// end of xind

simProg << "finished initialization of  " << pCount << std::endl;
simProg << "***************************************************" << std::endl;

