// Droplet Initialization	
dropBox = boxEdge[x]/2.0;	// boxEdge[y] or boxEdge[z] is also fine
xind_min = 1.0*( dropBox / 2.0 ) + 0.25;
yind_min = 1.0*( dropBox / 2.0 ) + 0.25;
zind_min = 1.0*( dropBox / 2.0 ) + 0.25;
xind_max = 3.0*( dropBox / 2.0 );
yind_max = 3.0*( dropBox / 2.0 );
zind_max = 3.0*( dropBox / 2.0 );

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
			// if ( xind*xind + yind*yind + zind*zind <= radSqr )
			particles.push_back({0.2,1.0,{xind, yind, zind},{rand_gen_velx, rand_gen_vely, rand_gen_velz}});

			// update zind
			zind += 0.63*rcutoff;
		}// end of zind
		yind += 0.63*rcutoff;
	}// end of yind			
	xind += 0.63*rcutoff;
}// end of xind
