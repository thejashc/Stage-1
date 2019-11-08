nPartPerStructure = 0;
// Lower positional indices of particles
xind_min = 0.01;	// avoid boundary edge -- non-moving case
xind_max = boxEdge[x];

// Upper positional indices of particles
yind_min = 0.01;	// avoid boundary edge -- non-moving case
yind_max = boxEdge[y];

aCube = pow( 1. / initRho, 1./2. );

xind = xind_min;
// Particle position in upper wall
while ( xind < xind_max){
    yind = yind_min;
    while( yind < yind_max){

        particles.push_back( {1.0, 1.0, {xind, yind, pistonZStart}, {0., 0., 0.}, 4} );

        // increment no. of particles
        nPartPerStructure++;

        yind += aCube*rcutoff;
    }// end of yind			
    xind += aCube*rcutoff;
}// end of xind

pCount += nPartPerStructure;

simProg << "finished initialization of  " << nPartPerStructure << " particles in piston" << std::endl;
simProg << "***************************************************" << std::endl;
