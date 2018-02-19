// Set max and min dimensions of boxy
xind_min = 0.25;
yind_min = 0.25;
zind_min = 0.25;
xind_max = boxEdge[x];
yind_max = boxEdge[y];
zind_max = boxEdge[z];

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
			particles.push_back({0.2,1.0,{xind, yind, zind},{rand_gen_velx, rand_gen_vely, rand_gen_velz}});

			// update zind
			zind += 1.00*rcutoff;

		}// end of zind
		yind += 1.00*rcutoff;
	}// end of yind			
	xind += 1.00*rcutoff;
}// end of xind
