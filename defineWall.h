// Set max and min dimensions of planar slab
xind_min = 0.01;	// avoid boundary edge -- non-moving case
xind_max = boxEdge[x];
yind_min = 0.01;	// avoid boundary edge -- non-moving case
yind_max = boxEdge[y];

aCube = pow( 1. / initRho, 1./3. );
#if LOWER_WALL_ON 
    zindLW_max = wallHeight;
    zindLW_min = wallHeight - 0.01; 
#endif 
#if UPPER_WALL_ON 
    zindUW_min = boxEdge[z] - wallHeight; 
    zindUW_max = boxEdge[z]; 
#endif 

xind = xind_min;

// Particle position in lower wall
#if LOWER_WALL_ON
    lwp = 0;
    simProg << "***************************************************" << std::endl;
    simProg << "Started initialization of the FCC wall " << std::endl;

    simProg << "The minimum x-coord of lower wall is: "<< xind_min << std::endl;
    simProg << "The minimum y-coord of lower wall is: "<< yind_min << std::endl;
    simProg << "The minimum z-coord of lower wall is: "<< zindLW_min << std::endl;

    simProg << "The maximum x-coord of lower wall is: "<< xind_max << std::endl;
    simProg << "The maximum y-coord of lower wall is: "<< yind_max << std::endl;
    simProg << "The maximum z-coord of lower wall is: "<< zindLW_max << std::endl;
    while ( xind < xind_max){
        yind = yind_min;
        while( yind < yind_max){
            zind = zindLW_max;
            while( zind > zindLW_min){

                // initializing particle (1) radius, 
                //			 (2) mass, 
                //			 (3) position 
                //           (4) mid step velocity 
                //			 (5) particle type
                particles.push_back( { 1.0, 1.0, {xind, yind, zind}, {0., 0., 0.}, 0} );
                lwp++;

                // update zind
                zind -= aCube*rcutoff;
            }// end of zind
            yind += aCube*rcutoff;
        }// end of yind			
        xind += aCube*rcutoff;
    }// end of xind

/*
    particles.push_back( { 1.0, 1.0, {7.5, 7.5, 6.00}, {0., 0., 1.0}, 0} );
    particles.push_back( { 1.0, 1.0, {7.5, 7.5, 6.25}, {0., 0., 0.0}, 0} );
    particles.push_back( { 1.0, 1.0, {7.5, 7.5, 6.50}, {0., 0., 0.0}, 0} );
    particles.push_back( { 1.0, 1.0, {7.5, 7.5, 6.75}, {0., 0., 0.0}, 0} );
    particles.push_back( { 1.0, 1.0, {7.5, 7.5, 7.00}, {0., 0., 0.0}, 0} );
    particles.push_back( { 1.0, 1.0, {7.5, 7.5, 7.25}, {0., 0., 0.0}, 0} );
    particles.push_back( { 1.0, 1.0, {7.5, 7.5, 7.50}, {0., 0., 0.0}, 0} );
    particles.push_back( { 1.0, 1.0, {7.5, 7.5, 7.75}, {0., 0., 0.0}, 0} );
    particles.push_back( { 1.0, 1.0, {7.5, 7.5, 8.00}, {0., 0., 0.0}, 0} );
    particles.push_back( { 1.0, 1.0, {7.5, 7.5, 8.25}, {0., 0., 0.0}, 0} );
    particles.push_back( { 1.0, 1.0, {7.5, 7.5, 8.50}, {0., 0., 0.0}, 0} );
    particles.push_back( { 1.0, 1.0, {7.5, 7.5, 8.75}, {0., 0., 0.0}, 0} );
    particles.push_back( { 1.0, 1.0, {7.5, 7.5, 9.00}, {0., 0., -1.0}, 0} );
*/
#endif 

xind = xind_min;

// Particle position in upper wall
#if UPPER_WALL_ON
    simProg << "finished initialization of  " << lwp << " lower wall particles" << std::endl;
    simProg << "***************************************************" << std::endl;

    simProg << "The minimum x-coord of upper wall is: "<< xind_min << std::endl;
    simProg << "The minimum y-coord of upper wall is: "<< yind_min << std::endl;
    simProg << "The minimum z-coord of upper wall is: "<< zindUW_min << std::endl;

    simProg << "The maximum x-coord of upper wall is: "<< xind_max << std::endl;
    simProg << "The maximum y-coord of upper wall is: "<< yind_max << std::endl;
    simProg << "The maximum z-coord of upper wall is: "<< zindUW_max << std::endl;
    uwp = 0;
    while ( xind < xind_max){
        yind = yind_min;
        while( yind < yind_max){
            zind = zindUW_min;
            while( zind < zindUW_max){

                // initializing particle radius, mass, position and velocity
                particles.push_back({1.0,1.0,{xind, yind, zind},{0., 0., 0.}, 0});
                uwp++;

                // update zind
                zind += aCube*rcutoff;
            }// end of zind
            yind += aCube*rcutoff;
        }// end of yind			
        xind += aCube*rcutoff;
    }// end of xind

    simProg << "finished initialization of  " << uwp << " lower wall particles" << std::endl;
    simProg << "***************************************************" << std::endl;
#endif 
