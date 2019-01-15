double pistonWidth = 2.0;
double shiftZPiston = 40.0;
// pistonStart = resCOMZ + resWdth * 0.5 + 3.0*rcutoff; 
// pistonEnd   = pistonStart + pistonWidth;
// pistonStart = bufferLen + capLen + capWallWdth + wettingLiquidLength + nonWettingLiquidLength + 3.5 * rcutoff;
//
// UPPER PISTON
bckgIdxStart = ngbrIdxEnd + 1;
pistonStart = maxZ + shiftZPiston;
pistonEnd   = pistonStart + pistonWidth;
upperPistonIdxStart = bckgIdxStart;

xind_min = 0.01;
yind_min = 0.01;
zind_min = pistonStart; 
xind_max = boxEdge[x];
yind_max = boxEdge[y];
zind_max = pistonEnd;

pCount = 0;

aCube = pow( 1. / initRho, 1./3. );

xind = xind_min;

simProg << "***************************************************" << std::endl;
simProg << "Started initialization of upper piston" << std::endl;

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
                particles.push_back( { 1.0, 1.0, {xind, yind, zind}, {0., 0., 0.}, 3} );
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

simProg << "End of initialization " << std::endl;
pistonParticles = pCount;

simProg << "finished initialization of  " << pistonParticles << " inside piston" << std::endl;
simProg << "***************************************************" << std::endl;

// LOWER PISTON 
pistonStart = minZ - shiftZPiston;
pistonEnd   = pistonStart + pistonWidth;
lowerPistonIdxStart = upperPistonIdxEnd+1;

xind_min = 0.01;
yind_min = 0.01;
zind_min = pistonStart; 
xind_max = boxEdge[x];
yind_max = boxEdge[y];
zind_max = pistonEnd;

pCount = 0;

aCube = pow( 1. / initRho, 1./3. );

xind = xind_min;

simProg << "***************************************************" << std::endl;
simProg << "Started initialization of lower piston" << std::endl;

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
                particles.push_back( { 1.0, 1.0, {xind, yind, zind}, {0., 0., 0.}, 0} );
                pCount++;
            }
            else {
                particles.push_back( { 1.0, 1.0, {xind, yind, zind}, {0., 0., 0.}, 3} );
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

simProg << "\nPistons are connected to their backgrounds"  << std::endl;
simProg << "\nParticles from index " << bckgIdxStart << " to " << bckgIdxEnd << " are connected to their backgrounds" << std::endl;

simProg << " Indices of upper piston are from: " << upperPistonIdxStart << " to " << upperPistonIdxEnd << std::endl;
simProg << " Indices of lower piston are from: " << lowerPistonIdxStart << " to " << lowerPistonIdxEnd << std::endl;
