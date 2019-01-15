cylCenterX		= boxEdge[x] / 2.;
cylCenterY		= boxEdge[y] / 2.;

bool wallBelowTube = 0;
bool wallAboveTube = 1;

capThick		= 2.;
if ( wallBelowTube ){
    /************************************** WALL ADJACENT RESERVOIR ********************************************/
    //  Set max and min dimensions of boxy
    xind_min = 0.01;
    yind_min = 0.01;
    zind_min = bufferLen - capWallWdth;
    xind_max = boxEdge[x];
    yind_max = boxEdge[y];
    zind_max = bufferLen;

    simProg << "***************************************************" << std::endl;
    simProg << "Started initialization of the wall-adjacent (below) capillary tube" << std::endl;

    simProg << "The minimum x-coord of wall is: "<< xind_min << std::endl;
    simProg << "The minimum y-coord of wall is: "<< yind_min << std::endl;
    simProg << "The minimum z-coord of wall is: "<< zind_min << std::endl;

    simProg << "The maximum x-coord of wall is: "<< xind_max << std::endl;
    simProg << "The maximum y-coord of wall is: "<< yind_max << std::endl;
    simProg << "The maximum z-coord of wall is: "<< zind_max << std::endl;

    xind = xind_min;
    while ( xind < xind_max){
        yind = yind_min;
        while( yind < yind_max){
            zind = zind_min;
            while( zind < zind_max){

                outerRadius = ( pow( xind - cylCenterX, 2.0 ) + pow( yind - cylCenterY, 2.0 ) <= pow( capRad + capThick, 2.0 ) );
                innerRadius = ( pow( xind - cylCenterX, 2.0 ) + pow( yind - cylCenterY, 2.0 ) <= pow( capRad, 2.0 ) );

                if ( !(outerRadius) && ( zind > zind_max - 0.5 * capWallWdth ) ){
                    // initializing particle radius, mass, position and velocity
                    particles.push_back({0.5,1.0,{xind, yind, zind},{0., 0., 0.},3});
                    pCount++;
                }
                else if( !(innerRadius) && ( zind < zind_max - 0.5 * capWallWdth ) ){ 
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
}
/******************************************* CAPILLARY TUBE ******************************************/
xind_min = 0.01;
yind_min = 0.01;
zind_min = bufferLen;

xind_max = boxEdge[x];
yind_max = boxEdge[y];
zind_max = bufferLen + capLen;

zind = zind_min;
aCube = pow( 1. / initRho, 1./3. );

simProg << "***************************************************" << std::endl;
simProg << "Started initialization of the capillary tube" << std::endl;

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

            innerRadius = ( pow( xind - cylCenterX, 2.0 ) + pow( yind - cylCenterY, 2.0 ) >= pow( capRad, 2.0 ) );
            middleInRadius = ( pow( xind - cylCenterX, 2.0 ) + pow( yind - cylCenterY, 2.0 ) <= pow( capRad + 0.5*capThick, 2.0 ) );
            middleOutRadius = ( pow( xind - cylCenterX, 2.0 ) + pow( yind - cylCenterY, 2.0 ) >= pow( capRad + 0.5*capThick, 2.0 ) );
            outerRadius = ( pow( xind - cylCenterX, 2.0 ) + pow( yind - cylCenterY, 2.0 ) <= pow( capRad + capThick, 2.0 ) );

            if ( innerRadius && middleInRadius ){

                // initializing particle radius, mass, position and velocity
                particles.push_back( { 1.0, 1.0, {xind, yind, zind}, {0., 0., 0.}, 0} );
                pCount++;

            }// inside cylinder
            else if ( middleOutRadius && outerRadius ){

                // initializing particle radius, mass, position and velocity
                particles.push_back( { 1.0, 1.0, {xind, yind, zind}, {0., 0., 0.}, 3} );
                pCount++;

            }

            yind += aCube * rcutoff;
        }// yind			
        xind += aCube * rcutoff;
    }
    zind += aCube * rcutoff;
}// zind

simProg << "finished initialization of  " << pCount << " particles inside capillary tube" << std::endl;
simProg << "***************************************************" << std::endl;

if ( wallAboveTube ){
    /************************************** WALL ADJACENT RESERVOIR ********************************************/
    // Set max and min dimensions of boxy
    xind_min = 0.01;
    yind_min = 0.01;
    zind_min = bufferLen + capLen;
    xind_max = boxEdge[x];
    yind_max = boxEdge[y];
    zind_max = zind_min + capWallWdth;

    simProg << "***************************************************" << std::endl;
    simProg << "Started initialization of the wall-adjacent (above) capillary tube" << std::endl;

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

                outerRadius = ( pow( xind - cylCenterX, 2.0 ) + pow( yind - cylCenterY, 2.0 ) <= pow( capRad + capThick, 2.0 ) );
                innerRadius = ( pow( xind - cylCenterX, 2.0 ) + pow( yind - cylCenterY, 2.0 ) <= pow( capRad, 2.0 ) );

                if ( !(outerRadius) && ( zind < zind_min + 0.6 * capWallWdth ) ){
                    // initializing particle radius, mass, position and velocity
                    particles.push_back({0.5,1.0,{xind, yind, zind},{0., 0., 0.},3});
                    pCount++;
                }
                else if ( !(innerRadius) && ( zind > zind_min + 0.6 * capWallWdth ) ){
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
}
