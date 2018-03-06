// Droplet Initialization
cylHeight =  boxEdge[z] / 2.0; // cylinder retraction
// cylHeight =  boxEdge[z]; -- surface tension measurements
cylCenterX = boxEdge[x] / 2.0;
cylCenterY = boxEdge[y] / 2.0;
cylRad = 6.0;

pCount = 0;
xCOM = 0.0;
yCOM = 0.0;
zCOM = 0.0;
xind_min = 0.00;
yind_min = 0.00;
zind_min = wallHeight;
xind_max = boxEdge[x];
yind_max = boxEdge[y];
zind_max = boxEdge[z] - wallHeight;

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
				particles.push_back({0.5,1.0,{xind, yind, zind},{rand_gen_velx, rand_gen_vely, rand_gen_velz},1});

				// calculating the center of mass of cylinder
				xCOM += xind;
				yCOM += yind;
				zCOM += zind;
				pCount += 1;

			}// inside cylinder
			yind += 0.63*rcutoff;
		}// yind			
		xind += 0.63*rcutoff;
	}
	zind += 0.63*rcutoff;
}// zind

// normalizing the center of mass
xCOM /= pCount;
yCOM /= pCount;
zCOM /= pCount;
