#if FCC_WALL
// Set max and min dimensions of planar slab
xind_min = 0.00;
xind_max = boxEdge[x];
yind_min = 0.00;
yind_max = boxEdge[y];
#if LOWER_WALL_ON 
zindLW_min = 0.00; 
zindLW_max = wallHeight;
#endif 
#if UPPER_WALL_ON 
zindUW_min = boxEdge[z] - wallHeight; 
zindUW_max = boxEdge[z]; 
#endif 

xind = xind_min;
aCube = pow( 1. / initWallRho, 1./3. );
// Particle position in lower wall
#if LOWER_WALL_ON
while ( xind < xind_max){
	yind = yind_min;
	while( yind < yind_max){
		zind = zindLW_max;
		while( zind > zindLW_min){

			// initializing particle (1) radius, 
			//			 (2) mass, 
			//			 (3) position 
			//                       (4) mid step velocity 
			//			 (5) particle type
			particles.push_back({1.0,1.0,{xind, yind, zind},{0., 0., 0.}, 0});

			// update zind
			zind -= aCube*rcutoff;
		}// end of zind
		yind += aCube*rcutoff;
	}// end of yind			
	xind += aCube*rcutoff;
}// end of xind
#endif 

xind = xind_min;
// Particle position in upper wall
#if UPPER_WALL_ON
while ( xind < xind_max){
	yind = yind_min;
	while( yind < yind_max){
		zind = zindUW_min;
		while( zind < zindUW_max){

			// initializing particle radius, mass, position and velocity
			particles.push_back({1.0,1.0,{xind, yind, zind},{0., 0., 0.}, 0});

			// update zind
			zind += aCube*rcutoff;
		}// end of zind
		yind += aCube*rcutoff;
	}// end of yind			
	xind += aCube*rcutoff;
}// end of xind
#endif 
#elif ROUGH_WALL

// rough wall
#include "restartWallconfig.h"	

#endif
