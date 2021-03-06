sqEdge 		= ( M_PI / 2. ) * capRad;

sqXmin		= 0.5 * boxEdge[x] - ( 0.5 * sqEdge + capWallWdth );
sqYmin		= 0.5 * boxEdge[y] - ( 0.5 * sqEdge + capWallWdth );
sqXmax 		= 0.5 * boxEdge[x] + ( 0.5 * sqEdge + capWallWdth );
sqYmax 		= 0.5 * boxEdge[y] + ( 0.5 * sqEdge + capWallWdth );

sqInnerEdgeXmin	= sqXmin + capWallWdth;
sqInnerEdgeXmax	= sqXmax - capWallWdth;
sqInnerEdgeYmin	= sqYmin + capWallWdth;
sqInnerEdgeYmax	= sqYmax - capWallWdth;

//std::cout << sqInnerEdgeXmin << std::endl;
//std::cout << sqInnerEdgeXmax << std::endl;
//std::cout << sqInnerEdgeYmin << std::endl;
//std::cout << sqInnerEdgeYmax << std::endl;

capThick	= 2.;
pCount 		= 0;

xind_min 	= 0.01;
yind_min 	= 0.01;
zind_min 	= bufferLen;

xind_max	= boxEdge[x];
yind_max	= boxEdge[y];
zind_max 	= bufferLen + capLen;

zind 		= zind_min;
aCube 		= pow( 1. / initWallRho, 1./3. );

simProg << "***************************************************" << std::endl;
simProg << "Started initialization of the square capillary tube" << std::endl;

simProg << "The starting x-coordinate of the square capillary tube: "<< xind_min << std::endl;
simProg << "The starting y-coordinate of the square capillary tube: "<< yind_min << std::endl;
simProg << "The starting z-coordinate of the square capillary tube: "<< zind_min << std::endl;

simProg << "The ending x-coordinate of the square capillary tube: "<< xind_max << std::endl;
simProg << "The ending y-coordinate of the square capillary tube: "<< yind_max << std::endl;
simProg << "The ending z-coordinate of the square capillary tube: "<< zind_max << std::endl;

while ( zind < zind_max ){
	xind = xind_min;
	while ( xind < xind_max){
		yind = yind_min;
		while( yind < yind_max){

			particleInSquareSmall = ( xind >= sqXmin + capWallWdth ) && ( xind <= sqXmax - capWallWdth ) && ( yind >= sqYmin + capWallWdth ) && ( yind <= sqYmax - capWallWdth );
			particleInSquareLarge = ( xind >= sqXmin ) && ( xind <= sqXmax ) && ( yind >= sqYmin ) && ( yind <= sqYmax );

			if ( !(particleInSquareSmall) && particleInSquareLarge ){

				// initializing particle radius, mass, position and velocity
				// if ( xind*xind + yind*yind + zind*zind <= radSqr )
				particles.push_back( { 1.0, 1.0, {xind, yind, zind}, {0., 0., 0.}, 0} );
				pCount++;

			}// inside cylinder
			yind += aCube * rcutoff;
		}// yind			
		xind += aCube * rcutoff;
	}
	zind += aCube * rcutoff;
}// zind

simProg << "finished initialization of  " << pCount << " particles inside square capillary tube" << std::endl;
simProg << "***************************************************" << std::endl;

// Set max and min dimensions of boxy
xind_min = 0.01;
yind_min = 0.01;
zind_min = bufferLen + capLen;
xind_max = boxEdge[x];
yind_max = boxEdge[y];
zind_max = zind_min + capWallWdth;

pCount	 = 0;

simProg << "***************************************************" << std::endl;
simProg << "Started initialization of the wall adjacent to reservoir" << std::endl;

simProg << "The minimum x-coord of wall is: "<< xind_min << std::endl;
simProg << "The minimum y-coord of wall is: "<< yind_min << std::endl;
simProg << "The minimum z-coord of wall is: "<< zind_min << std::endl;

simProg << "The maximum x-coord of wall is: "<< xind_max << std::endl;
simProg << "The maximum y-coord of wall is: "<< yind_max << std::endl;
simProg << "The maximum z-coord of wall is: "<< zind_max << std::endl;

xind = xind_min;
// Particle position intialization in a crystal structure 
while ( xind < xind_max){
	yind = yind_min;
	while( yind < yind_max){
		zind = zind_min;
		while( zind < zind_max){

			particleInSquareSmall = ( xind >= sqXmin + capWallWdth ) && ( xind <= sqXmax - capWallWdth ) && ( yind >= sqYmin + capWallWdth ) && ( yind <= sqYmax - capWallWdth );

			if ( !(particleInSquareSmall) ){
				// initializing particle radius, mass, position and velocity
				particles.push_back({0.5,1.0,{xind, yind, zind},{0., 0., 0.},0});
				pCount++;
			}

			// update zind
			zind += aCube * rcutoff;

		} // end of zind
		yind += aCube * rcutoff;
	} // end of yind			
	xind += aCube * rcutoff;
} // end of xind

simProg << "finished initialization of  " << pCount << " particles inside wall adj. capillary" << std::endl;
simProg << "***************************************************" << std::endl;
