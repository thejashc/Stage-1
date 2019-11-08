double nonWettingLiquidLength = 7.5;
double wettingLiquidLength = 2.0;

///**************************** BELOW RESERVOIR **************************************/
// Set max and min dimensions of boxy

xind_min = 0.01;
yind_min = 0.01;
zind_min = bufferLen - capWallWdth - wettingLiquidLength - 0.5;
xind_max = boxEdge[x];
yind_max = boxEdge[y];
zind_max = zind_min + wettingLiquidLength;

pCount	 = 0;
aCube = pow( 1. / initRho, 1./3. );

simProg << "***************************************************" << std::endl;
simProg << "Started initialization of the wetting liquid above reservoir" << std::endl;

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

				// initializing particle radius, mass, position and velocity
				particles.push_back({1.0,1.0,{xind, yind, zind},{0., 0., 0.},5});
				pCount++;

			// update zind
			zind += aCube * rcutoff;

		} // end of zind
		yind += aCube * rcutoff;
	} // end of yind			
	xind += aCube * rcutoff;
} // end of xind

simProg << "finished initialization of  " << pCount << " particles inside wall adj. capillary" << std::endl;
simProg << "***************************************************" << std::endl;
///********************************* LIQUID IN CAPILLARY TUBE **************************/
cylCenterX		= boxEdge[x] / 2.;
cylCenterY		= boxEdge[y] / 2.;
capThick		= 2.;
pCount = 0;

xind_min = 0.;
yind_min = 0.;
zind_min = bufferLen;

xind_max = boxEdge[x];
yind_max = boxEdge[y];
zind_max = bufferLen + capLen;

zind = zind_min;
aCube = pow( 1. / initRho, 1./3. );

simProg << "***************************************************" << std::endl;
simProg << "Started initialization of wetting liquid inside capillary tube" << std::endl;

simProg << "The starting x-coordinate of the capillary tube: "<< xind_min << std::endl;
simProg << "The starting y-coordinate of the capillary tube: "<< yind_min << std::endl;
simProg << "The starting z-coordinate of the capillary tube: "<< zind_min << std::endl;

simProg << "The ending x-coordinate of the capillary tube: "<< xind_max << std::endl;
simProg << "The ending y-coordinate of the capillary tube: "<< yind_max << std::endl;
simProg << "The ending z-coordinate of the capillary tube: "<< zind_max << std::endl;

while ( zind < zind_max ){
	xind = xind_min;
	while ( xind < xind_max){
		yind = yind_min;
		while( yind < yind_max){

			// outerRadius = ( pow( xind - cylCenterX, 2.0 ) + pow( yind - cylCenterY, 2.0 ) <= pow( capRad + capThick, 2.0 ) );
			innerRadius = ( pow( xind - cylCenterX, 2.0 ) + pow( yind - cylCenterY, 2.0 ) <= pow( capRad, 2.0 ) );

			if ( innerRadius ){

				// initializing particle radius, mass, position and velocity
				// if ( xind*xind + yind*yind + zind*zind <= radSqr )
				particles.push_back( { 1.0, 1.0, {xind, yind, zind}, {0., 0., 0.}, 5} );
				pCount++;

			}// inside cylinder
			yind += aCube * rcutoff;
		}// yind			
		xind += aCube * rcutoff;
	}
	zind += aCube * rcutoff;
}// zind

simProg << "finished initialization of  " << pCount << " particles inside capillary tube" << std::endl;
simProg << "***************************************************" << std::endl;

///**************************** ABOVE RESERVOIR **************************************/
//// Set max and min dimensions of boxy
//xind_min = 0.;
//yind_min = 0.;
//// zind_min = bufferLen + capLen + capWallWdth;
//zind_min = bufferLen + capLen + capWallWdth;
//xind_max = boxEdge[x];
//yind_max = boxEdge[y];
//// zind_max = zind_min + wettingLiquidLength;
//zind_max = zind_min + wettingLiquidLength;
//
///*
//xind_min = 0.;
//yind_min = 0.;
//zind_min = resCOMZ - resWdth * 0.5;
//xind_max = boxEdge[x];
//yind_max = boxEdge[y];
//zind_max = resCOMZ + resWdth * 0.5; 
//*/
//
//pCount	 = 0;
//
//simProg << "***************************************************" << std::endl;
//simProg << "Started initialization of the wetting liquid above the reservoir" << std::endl;
//
//simProg << "The minimum x-coord of wall is: "<< xind_min << std::endl;
//simProg << "The minimum y-coord of wall is: "<< yind_min << std::endl;
//simProg << "The minimum z-coord of wall is: "<< zind_min << std::endl;
//
//simProg << "The maximum x-coord of wall is: "<< xind_max << std::endl;
//simProg << "The maximum y-coord of wall is: "<< yind_max << std::endl;
//simProg << "The maximum z-coord of wall is: "<< zind_max << std::endl;
//
//xind = xind_min;
//// Particle position intialization in a crystal structure 
//while ( xind < xind_max){
//	yind = yind_min;
//	while( yind < yind_max){
//		zind = zind_min;
//		while( zind < zind_max){
//
//				// initializing particle radius, mass, position and velocity
//				particles.push_back({0.5,1.0,{xind, yind, zind},{0., 0., 0.},1});
//				pCount++;
//
//			// update zind
//			zind += aCube * rcutoff;
//
//		} // end of zind
//		yind += aCube * rcutoff;
//	} // end of yind			
//	xind += aCube * rcutoff;
//} // end of xind
//
//simProg << "finished initialization of  " << pCount << " particles inside wall adj. capillary" << std::endl;
//simProg << "***************************************************" << std::endl;
