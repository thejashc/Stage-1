// Droplet Initialization	
cylHeight = boxEdge[z];	
cylCenterX = boxEdge[x] / 2.0;
cylCenterY = boxEdge[y] / 2.0;
cylRad = 4.0;

xind_min = 0.00;
yind_min = 0.00;
zind_min = 0.00;
xind_max = boxEdge[x];
yind_max = boxEdge[y];
zind_max = cylHeight;

zind = zind_min;
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
				particles.push_back({0.2,1.0,{xind, yind, zind},{rand_gen_velx, rand_gen_vely, rand_gen_velz}});

			}// inside cylinder
			yind += 0.63*rcutoff;
		}// yind			
		xind += 0.63*rcutoff;
	}
	zind += 0.63*rcutoff;
}// zind
