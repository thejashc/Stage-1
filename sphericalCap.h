resCOMVel = -0.05;
double capRad = 5.0;
capRadSqr = pow(capRad, 2.);
pCount = 0;

// co-ordinates of center of spherical Cap
capSphXc = boxEdge[x] / 2.;
capSphYc = boxEdge[y] / 2.;
capSphZc = 40.;

// Droplet Initialization	
xind_min = 0.;
yind_min = 0.;
zind_min = wallHeight;
xind_max = boxEdge[x];
yind_max = boxEdge[y];
zind_max = boxEdge[z];


simProg << "***************************************************" << std::endl;
simProg << "Started initialization of the spherical cap of fluid " << std::endl;

simProg << "The minimum x-coord of fluid particle is: "<< xind_min << std::endl;
simProg << "The minimum y-coord of fluid particle is: "<< yind_min << std::endl;
simProg << "The minimum z-coord of fluid particle is: "<< zind_min << std::endl;

simProg << "The maximum x-coord of fluid particle is: "<< xind_max << std::endl;
simProg << "The maximum y-coord of fluid particle is: "<< yind_max << std::endl;
simProg << "The maximum z-coord of fluid particle is: "<< zind_max << std::endl;

simProg << "The radius of the spherical cap is:       "<< capRad << std::endl;

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

			// initializing particle radius, mass, position and velocity and type
			// check if ( x - xp )^2 + ( y - yp )^2 + ( z - zp )^2 <= capRadSqr
			if ( pow( xind - capSphXc , 2. ) + pow( yind - capSphYc, 2. ) + pow( zind - capSphZc , 2. ) <= capRadSqr )
				particles.push_back({0.5,1.0,{xind, yind, zind},{rand_gen_velx, rand_gen_vely, rand_gen_velz},1});
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

zind_max = wallHeight;
