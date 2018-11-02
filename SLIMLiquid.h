double nonWettingLiquidLength = 10;
double wettingLiquidLength = 22;

xind_min = 0.25;
yind_min = 0.25;
zind_min = bufferLen + pistonWidth + 3.;
xind_max = boxEdge[x];
yind_max = boxEdge[y];
zind_max = boxEdge[z];

simProg << "***************************************************" << std::endl;
simProg << "Started initialization of liquid of type 1 and type 2 " << std::endl;

simProg << "The starting x-coordinate of the crystal: "<< xind_min << std::endl;
simProg << "The starting y-coordinate of the crystal: "<< yind_min << std::endl;
simProg << "The starting z-coordinate of the crystal: "<< zind_min << std::endl;

simProg << "The ending x-coordinate of the crystal: "<< xind_max << std::endl;
simProg << "The ending y-coordinate of the crystal: "<< yind_max << std::endl;
simProg << "The ending z-coordinate of the crystal: "<< zind_max << std::endl;


aCube = pow( 1. / initRho, 1./3. );

xind = xind_min;
pCount = 0;
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

            // initializing particle radius, mass, position and velocity
            if ( ( zind > zind_min ) && zind < ( zind_min + nonWettingLiquidLength ) ){
                particles.push_back({1.0,1.0,{xind, yind, zind},{rand_gen_velx, rand_gen_vely, rand_gen_velz}, 2});
                pCount++;
            }
            else if ( ( zind > zind_min + nonWettingLiquidLength ) && zind < ( zind_min + nonWettingLiquidLength + wettingLiquidLength ) ){
                particles.push_back({1.0,1.0,{xind, yind, zind},{rand_gen_velx, rand_gen_vely, rand_gen_velz}, 1});
                pCount++;
            }

            // update zind
            zind += aCube*rcutoff;

        }// end of zind
        yind += aCube*rcutoff;
    }// end of yind			
    xind += aCube*rcutoff;
}// end of xind

simProg << "finished initialization of  " << pCount << " particles inside crystal lattice" << std::endl;
simProg << "***************************************************" << std::endl;
