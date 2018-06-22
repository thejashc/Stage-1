#if WALL_ON

	pCount = 0;
	#if BODY_FORCE
		xind_min = 0.0;
		xind_max = boxEdge[x];
	#else
		if ( slabWidth > boxEdge[x] ){
			simProg << "** The width of the slab intialized is greater than simulation box **" << std::endl;
			simProg << "** Aborting ! ** " << std::endl;abort();}  
	
		xind_min = ( boxEdge[x] / 2.)  - ( slabWidth / 2. ) ;
		xind_max = ( boxEdge[x] / 2.)  + ( slabWidth / 2. ) ;
	#endif

	yind_min = 0.0;
	yind_max = boxEdge[y];
	zind_min = wallHeight + 1.;
	zind_max = boxEdge[z] - wallHeight - 1.;

	aCube = pow( 1. / initRho, 1./3. );
	zind = zind_min;

	simProg << "***************************************************" << std::endl;
	simProg << "Started initialization of the planar slab of fluid " << std::endl;

	simProg << "The minimum x-coord of fluid particle is: "<< xind_min << std::endl;
	simProg << "The minimum y-coord of fluid particle is: "<< yind_min << std::endl;
	simProg << "The minimum z-coord of fluid particle is: "<< zind_min << std::endl;
	
	simProg << "The maximum x-coord of fluid particle is: "<< xind_max << std::endl;
	simProg << "The maximum y-coord of fluid particle is: "<< yind_max << std::endl;
	simProg << "The maximum z-coord of fluid particle is: "<< zind_max << std::endl;

	while ( zind < zind_max ){
		xind = xind_min;
		// Particle position intialization in a crystal structure 
		while ( xind < xind_max){
			yind = yind_min;
			while( yind < yind_max){

				// generate random velocities
				rand_gen_velx = ((double) rand() / (RAND_MAX));
				rand_gen_vely = ((double) rand() / (RAND_MAX));
				rand_gen_velz = ((double) rand() / (RAND_MAX));

				// initializing particle radius, mass, position and velocity
				// if ( xind*xind + yind*yind + zind*zind <= radSqr )
				particles.push_back({0.5,1.0,{xind, yind, zind},{0., 0., 0.},1});

				// calculating the center of mass of cylinder
				xCOM += xind;
				yCOM += yind;
				zCOM += zind;
				pCount += 1;

				yind += aCube*rcutoff;
			}// yind			
			xind += aCube*rcutoff;
		}
		zind += aCube*rcutoff;
	}// zind
	simProg << "finished initialization of  " << pCount << std::endl;
	simProg << "***************************************************" << std::endl;

#else
	// Set max and min dimensions of planar slab
	xind_min = 0.25;
	yind_min = 0.25;
	zind_min = ( boxEdge[z] / 2. ) - ( slabWidth / 2. );
	xind_max = boxEdge[x];
	yind_max = boxEdge[y];
	zind_max = ( boxEdge[z] / 2. ) + ( slabWidth / 2. );

	xind = xind_min;
	aCube = pow( 1. / initRho, 1./3. );
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
				particles.push_back({1.0,1.0,{xind, yind, zind},{rand_gen_velx, rand_gen_vely, rand_gen_velz},1});

				// calculating the center of mass of cylinder
				xCOM += xind;
				yCOM += yind;
				zCOM += zind;
				pCount += 1;

				// update zind
				zind += aCube*rcutoff;

			}// end of zind
			yind += aCube*rcutoff;
		}// end of yind			
		xind += aCube*rcutoff;
	}// end of xind
	simProg << "finished initializing the planar slab" << std::endl;

	// normalizing the center of mass
	xCOM /= pCount;
	yCOM /= pCount;
	zCOM /= pCount;

	// recenter the particle positions to place at exact COM
	for( i = 0 ; i < pCount ; ++i ){

		particles[i].r.X += - xCOM + ( boxEdge[x] / 2.0 );
		particles[i].r.Y += - yCOM + ( boxEdge[y] / 2.0 );
		particles[i].r.Z += - zCOM + ( boxEdge[z] / 2.0 );
	}
#endif
