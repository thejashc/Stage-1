// Initializing the cylinder array
// layer 1
double hcpEdge   = 5.0 * rcutoff;
double hcpHeight = 0.5 * sqrt(3) * hcpEdge;
double xind_cylX = 0.5 * hcpEdge;
double zind_cylZ_L1 = 0.5 * hcpEdge;
double yind_cylY = 0.5 * hcpEdge;
double zind_cylZ_L2 = 0.5 * hcpEdge + hcpHeight;

while ( xind_cylX <= boxEdge[x] ){
    zind_cylZ_L1 = 0.5 * hcpEdge;
    while ( zind_cylZ_L1 <= 0.3 * boxEdge[z] ){

        p1.X = xind_cylX;   p1.Y = 0.;            p1.Z = zind_cylZ_L1;
        p2.X = xind_cylX;   p2.Y = boxEdge[y];    p2.Z = zind_cylZ_L1;

        // std::cout << " Input p1 = " << p1 << ", Input p2 = " << p2 << std::endl;

        createCylinderArray ( p1, p2, 1.5);
        zind_cylZ_L1 += 2. * hcpHeight;

    }

    xind_cylX   += hcpEdge;
}

/*
// Parallel Fiber
xind_cylX = hcpEdge;
zind_cylZ_L2 = 0.5 * hcpEdge + hcpHeight;

while ( xind_cylX <= boxEdge[x] ){
    zind_cylZ_L2 =0.5 * hcpEdge + hcpHeight;
    while ( zind_cylZ_L2 <= 0.3 * boxEdge[z] ){

        p1.X = xind_cylX;   p1.Y = 0.;            p1.Z = zind_cylZ_L2;
        p2.X = xind_cylX;   p2.Y = boxEdge[y];    p2.Z = zind_cylZ_L2;

        createCylinderArray ( p1, p2, 1.5 );
        zind_cylZ_L2 += 2. * hcpHeight;

    }
    xind_cylX += hcpEdge;
}
*/

// Cross fiber
while ( yind_cylY <= boxEdge[y] ){
    zind_cylZ_L2 = 0.5 * hcpEdge + hcpHeight;
    while ( zind_cylZ_L2 <= 0.3 * boxEdge[z] ){

        p1.X = 0.;           p1.Y = yind_cylY;      p1.Z = zind_cylZ_L2;
        p2.X = boxEdge[x];   p2.Y = yind_cylY;      p2.Z = zind_cylZ_L2;

        createCylinderArray ( p1, p2, 1.5 );
        zind_cylZ_L2 += 2. * hcpHeight;

    }
    yind_cylY += hcpEdge;
}

//// Intitialization of the reservoir
//xind_min = 0.;
//yind_min = 0; 
//zind_min = std::max( zind_cylZ_L1, zind_cylZ_L2) - 2. * hcpHeight + 1.5 + 0.25;
//xind_max = boxEdge[x];
//yind_max = boxEdge[y]; 
//zind_max = 0.85 * boxEdge[z];
//
//pCount = 0;
//
//aCube = pow( 1. / initRho, 1./3. );
//
//simProg << "***************************************************" << std::endl;
//simProg << "Started initialization of the reservoir" << std::endl;
//
//simProg << "The minimum x-coord of fluid particle is: "<< xind_min << std::endl;
//simProg << "The minimum y-coord of fluid particle is: "<< yind_min << std::endl;
//simProg << "The minimum z-coord of fluid particle is: "<< zind_min << std::endl;
//
//simProg << "The maximum x-coord of fluid particle is: "<< xind_max << std::endl;
//simProg << "The maximum y-coord of fluid particle is: "<< yind_max << std::endl;
//simProg << "The maximum z-coord of fluid particle is: "<< zind_max << std::endl;
//
//// Particle position intialization in a crystal structure 
//xind = xind_min;
//while ( xind < xind_max){
//    yind = yind_min;
//    while( yind < yind_max){
//        zind = zind_min;
//        while( zind < zind_max){
//            // generate random velocities
//            rand_gen_velx = ((double) rand() / (RAND_MAX));
//            rand_gen_vely = ((double) rand() / (RAND_MAX));
//            rand_gen_velz = ((double) rand() / (RAND_MAX));
//        
//            #if MULTI_VISCOSITY_LIQUIDS
//                // initializing particle radius, mass, position and velocity
//                if ( zind < zind_min + 0.5 * resWdth ){
//                    particles.push_back({1.0,1.0,{xind, yind, zind},{rand_gen_velx, rand_gen_vely, rand_gen_velz}, 1});
//                    pCount++;
//                }
//                else{ 
//                    particles.push_back({1.0,1.0,{xind, yind, zind},{rand_gen_velx, rand_gen_vely, rand_gen_velz}, 2});
//                    pCount++;
//                }
//            #else
//                // initializing particle radius, mass, position and velocity
//                particles.push_back({1.0,1.0,{xind, yind, zind},{rand_gen_velx, rand_gen_vely, rand_gen_velz}, 1});
//                pCount++;
//            #endif
//
//            // update zind
//            zind += aCube * rcutoff;
//
//        }// end of zind
//        yind += aCube * rcutoff;
//    }// end of yind			
//    xind += aCube * rcutoff;
//}// end of xind
//
//simProg << "finished initialization of  " << pCount << std::endl;
//simProg << "***************************************************" << std::endl;
//

capRad   = 10.0;
capRadSqr = pow( capRad, 2.);
capSphXc = boxEdge[x] * 0.5;
capSphYc = boxEdge[y] * 0.5;
capSphZc = std::min( zind_cylZ_L1, zind_cylZ_L2) + capRad - 3.5;
pCount = 0;

// Droplet Initialization	
xind_min = 0.;
yind_min = 0.;
zind_min = std::min( zind_cylZ_L1, zind_cylZ_L2) - 3.5;
xind_max = boxEdge[x];
yind_max = boxEdge[y];
zind_max = boxEdge[z]; 

simProg << "***************************************************" << std::endl;
simProg << "Started initialization of the spherical droplet of fluid " << std::endl;

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
			if ( pow( xind - capSphXc , 2. ) + pow( yind - capSphYc, 2. ) + pow( zind - capSphZc , 2. ) <= capRadSqr ){
				particles.push_back({0.5,1.0,{xind, yind, zind},{rand_gen_velx, rand_gen_vely, rand_gen_velz},1});
				pCount++;
            }

			// update zind
			zind += aCube * rcutoff;
		}// end of zind
		yind += aCube * rcutoff;
	}// end of yind			
	xind += aCube * rcutoff;
}// end of xind
simProg << "finished initialization of  " << pCount << std::endl;
simProg << "***************************************************" << std::endl;

// Rounding boxSize[z] to the nearest multiple of 5
zind_max = std::min( zind_cylZ_L1, zind_cylZ_L2) + 2 * capRad;
resizeBoxSizeTo = floor( zind_max / 5 ) * 5 + 5;

simProg << " The max z-coord is = " << zind_max << std::endl; 
boxResize( resizeBoxSizeTo );
