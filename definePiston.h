#if SLIM && PISTON
// double bufferLen   = 10.;
double pistonWidth = 2.0;

xind_min = 0.;
yind_min = 0.;
zind_min = bufferLen; 
xind_max = boxEdge[x];
yind_max = boxEdge[y];
zind_max = bufferLen + pistonWidth;

pCount = 0;

aCube = pow( 1. / initRho, 1./3. );

xind = xind_min;

simProg << "***************************************************" << std::endl;
simProg << "Started initialization of piston" << std::endl;

simProg << "The minimum x-coord of piston is: "<< xind_min << std::endl;
simProg << "The minimum y-coord of piston is: "<< yind_min << std::endl;
simProg << "The minimum z-coord of piston is: "<< zind_min << std::endl;

simProg << "The maximum x-coord of piston is: "<< xind_max << std::endl;
simProg << "The maximum y-coord of piston is: "<< yind_max << std::endl;
simProg << "The maximum z-coord of piston is: "<< zind_max << std::endl;

xind = xind_min;
pistonStartIndex = 0;
// Particle position intialization in a crystal structure 
while ( xind < xind_max){
    yind = yind_min;
    while( yind < yind_max){
        zind = zind_min;
        while( zind < zind_max){

            // initializing particle radius, mass, position and velocity
            if ( zind < zind_min + 1.0 ){
                particles.push_back( { 1.0, 1.0, {xind, yind, zind}, {0., 0., 0.}, 0} );
                pCount++;
            }
            else {
                particles.push_back( { 1.0, 1.0, {xind, yind, zind}, {0., 0., 0.}, 0} );
                pCount++;
            }

            // update zind
            zind += aCube * rcutoff;

        } // end of zind
        yind += aCube * rcutoff;
    } // end of yind			
    xind += aCube * rcutoff;
} // end of xind

pistonEndIndex = pCount;
simProg << "finished initialization of  " << pistonEndIndex << " inside piston" << std::endl;
simProg << "Piston Start Index = " << pistonStartIndex << " and Piston End Index = " << pistonEndIndex;
simProg << "***************************************************" << std::endl;

#elif PISTON && !(SLIM)

    double nonWettingLiquidLength = 7.5;
    double wettingLiquidLength = 2.0;
    double pistonWidth = 2.0;
    pistonStart = resCOMZ + resWdth * 0.5 + 3.0*rcutoff; 
    // pistonStart = bufferLen + capLen + capWallWdth + wettingLiquidLength + nonWettingLiquidLength + 3.5 * rcutoff;
    pistonEnd   = pistonStart + pistonWidth;

	xind_min = 0.;
	yind_min = 0.;
	zind_min = pistonStart; 
	xind_max = boxEdge[x];
	yind_max = boxEdge[y];
	zind_max = pistonEnd;

	pCount = 0;

	aCube = pow( 1. / initRho, 1./3. );

	xind = xind_min;

	simProg << "***************************************************" << std::endl;
	simProg << "Started initialization of piston" << std::endl;
	
	simProg << "The minimum x-coord of piston is: "<< xind_min << std::endl;
	simProg << "The minimum y-coord of piston is: "<< yind_min << std::endl;
	simProg << "The minimum z-coord of piston is: "<< zind_min << std::endl;
	
	simProg << "The maximum x-coord of piston is: "<< xind_max << std::endl;
	simProg << "The maximum y-coord of piston is: "<< yind_max << std::endl;
	simProg << "The maximum z-coord of piston is: "<< zind_max << std::endl;
	
	xind = xind_min;
	// Particle position intialization in a crystal structure 
	while ( xind < xind_max){
		yind = yind_min;
		while( yind < yind_max){
			zind = zind_min;
			while( zind < zind_max){
	
                // initializing particle radius, mass, position and velocity
                if ( zind > zind_min + 1.0 ){
                    particles.push_back( { 1.0, 1.0, {xind, yind, zind + 20.}, {0., 0., 0.}, 3} );
                    pCount++;
                }
                else {
                    particles.push_back( { 1.0, 1.0, {xind, yind, zind + 20.}, {0., 0., 0.}, 0} );
                    pCount++;
                }

				// update zind
				zind += aCube * rcutoff;

			} // end of zind
			yind += aCube * rcutoff;
		} // end of yind			
		xind += aCube * rcutoff;
	} // end of xind

	pistonParticles = pCount;
	simProg << "finished initialization of  " << pistonParticles << " inside piston" << std::endl;
	simProg << "***************************************************" << std::endl;
#endif
