// Droplet Initialization	
xind_min = 0.;
yind_min = 0.;
zind_min = 0.;
xind_max = boxEdge[x];
yind_max = boxEdge[y];
zind_max = boxEdge[z];

double radSqr = sphDropRad * sphDropRad;

aCube = pow( 1. / initRho, 1./3. );

xind = xind_min;
// Particle position intialization in a crystal structure 
while ( xind < xind_max){
	yind = yind_min;
	while( yind < yind_max){
		zind = zind_min;
		while( zind < zind_max){
			// generate random velocities
			rand_gen_velx = 0.;
			rand_gen_vely = 0.;
			rand_gen_velz = 0.;

			// initializing particle radius, mass, position and velocity and type
			if ( pow( xind - 0.5 * boxEdge[x], 2.) + 
                 pow( yind - 0.5 * boxEdge[y], 2.) + 
                 pow( zind - 0.5 * boxEdge[z], 2.) <= radSqr )

                particles.push_back({1.0,1.0,{xind, yind, zind},{0., 0., 0.},1});

			// update zind
			zind += aCube*rcutoff;
		}// end of zind
		yind += aCube*rcutoff;
	}// end of yind			
	xind += aCube*rcutoff;
}// end of xind
