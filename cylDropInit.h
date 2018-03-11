#if WALL_ON
cylCenterX = boxEdge[x] / 2.0;
cylCenterY = boxEdge[y] / 2.0;

pCount = 0;
xCOM = 0.0;
yCOM = 0.0;
zCOM = 0.0;
xind_min = 0.00;
yind_min = 0.00;
zind_min = wallHeight + 1.00;
xind_max = boxEdge[x];
yind_max = boxEdge[y];
zind_max = boxEdge[z] - wallHeight - 1.00;

zind = zind_min;
aCube = pow( 1. / initRho, 1./3. );
while ( zind < zind_max ){
	xind = xind_min;
	// Particle position intialization in a crystal structure 
	while ( xind < xind_max){
		yind = yind_min;
		while( yind < yind_max){

			if ( pow( xind - cylCenterX, 2.0 ) + pow( yind - cylCenterY, 2.0 ) <= pow( cylRad, 2.0 ) ){

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

			}// inside cylinder
			yind += aCube*rcutoff;
		}// yind			
		xind += aCube*rcutoff;
	}
	zind += aCube*rcutoff;
}// zind

// normalizing the center of mass
xCOM /= pCount;
yCOM /= pCount;
zCOM /= pCount;

#else
cylCenterX = boxEdge[x] / 2.0;
cylCenterY = boxEdge[y] / 2.0;

pCount = 0;
xCOM = 0.0;
yCOM = 0.0;
zCOM = 0.0;
xind_min = 0.00;
yind_min = 0.00;
zind_min = 0.00; 
xind_max = boxEdge[x];
yind_max = boxEdge[y];
zind_max = cylHeight;

zind = zind_min;
aCube = pow( 1. / initRho, 1./3. );
while ( zind < zind_max ){
	xind = xind_min;
	// Particle position intialization in a crystal structure 
	while ( xind < xind_max){
		yind = yind_min;
		while( yind < yind_max){

			if ( pow( xind - cylCenterX, 2.0 ) + pow( yind - cylCenterY, 2.0 ) <= pow( cylRad, 2.0 ) ){

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

			}// inside cylinder
			yind += aCube*rcutoff;
		}// yind			
		xind += aCube*rcutoff;
	}
	zind += aCube*rcutoff;
}// zind

// normalizing the center of mass
xCOM /= pCount;
yCOM /= pCount;
zCOM /= pCount;

#endif
