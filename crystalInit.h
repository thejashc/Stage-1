// Set max and min dimensions of boxy
xind_min = 0.25;
yind_min = 0.25;
zind_min = 0.25;
xind_max = boxEdge[x];
yind_max = boxEdge[y];
zind_max = boxEdge[z];

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

npart = round( initRho * boxEdge[x] * boxEdge[y] * boxEdge[z] );

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
