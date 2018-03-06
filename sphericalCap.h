capRad = 6.0;
capRadSqr = pow(capRad, 2.);

// co-ordinates of center of spherical Cap
capSphXc = boxEdge[x] / 2.;
capSphYc = boxEdge[y] / 2.;
capSphZc = wallHeight;

// Droplet Initialization	
xind_min = 0.;
yind_min = 0.;
zind_min = wallHeight;
xind_max = boxEdge[x];
yind_max = boxEdge[y];
zind_max = wallHeight + capRad;

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

			// initializing particle radius, mass, position and velocity and type
			// check if ( x - xp )^2 + ( y - yp )^2 + ( z - zp )^2 <= capRadSqr
			if ( pow( xind - capSphXc , 2. ) + pow( yind - capSphYc, 2. ) + pow( zind - capSphZc , 2. ) <= capRadSqr )
				particles.push_back({0.5,1.0,{xind, yind, zind},{rand_gen_velx, rand_gen_vely, rand_gen_velz},1});

			// update zind
			zind += 0.63*rcutoff;
		}// end of zind
		yind += 0.63*rcutoff;
	}// end of yind			
	xind += 0.63*rcutoff;
}// end of xind
