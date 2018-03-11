#if WALL_ON
xind_min = ( boxEdge[x] / 2.)  - ( slabWidth / 2. ) ;
yind_min = 0.0;
zind_min = wallHeight + 1.00;
xind_max = ( boxEdge[x] / 2.)  + ( slabWidth / 2. ) ;
yind_max = boxEdge[y];
zind_max = boxEdge[z] - wallHeight - 1.00;

aCube = pow( 1. / initRho, 1./3. );
zind = zind_min;
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
			particles.push_back({0.5,1.0,{xind, yind, zind},{rand_gen_velx, rand_gen_vely, rand_gen_velz},1});

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

#else
// Set max and min dimensions of planar slab
xind_min = 0.25;
yind_min = 0.25;
zind_min = ( boxEdge[z] / 2. ) - ( slabWidth / 2. );
xind_max = boxEdge[x];
yind_max = boxEdge[y];
zind_max = ( boxEdge[z] / 2. ) + ( slabWidth / 2. );

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
