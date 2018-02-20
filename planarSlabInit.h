slabWidth = 5.0;
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
			particles.push_back({1.0,1.0,{xind, yind, zind},{rand_gen_velx, rand_gen_vely, rand_gen_velz}});

			// update zind
			zind += 0.63*rcutoff;

		}// end of zind
		yind += 0.63*rcutoff;
	}// end of yind			
	xind += 0.63*rcutoff;
}// end of xind
