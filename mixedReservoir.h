// Set max and min dimensions of boxy
//xind_min = sqInnerEdgeXmin;
//yind_min = sqInnerEdgeYmin;
//zind_min = bufferLen + capLen - 10; 
//xind_max = sqInnerEdgeXmax;
//yind_max = sqInnerEdgeYmax;
//zind_max = bufferLen + capLen;

xind_min = 0.;
yind_min = 0.;
zind_min = resCOMZ - resWdth * 0.5;
xind_max = boxEdge[x];
yind_max = boxEdge[y];
zind_max = resCOMZ + resWdth * 0.5; 

pCount = 0;

aCube      = pow( 1. / initRho, 1./3. );
fluidCount = initRho * ( boxEdge[x] * boxEdge[y] * ( zind_max - zind_min ) );

simProg << "***************************************************" << std::endl;
simProg << "Started initialization of the reservoir" << std::endl;

simProg << "The minimum x-coord of fluid particle is: "<< xind_min << std::endl;
simProg << "The minimum y-coord of fluid particle is: "<< yind_min << std::endl;
simProg << "The minimum z-coord of fluid particle is: "<< zind_min << std::endl;

simProg << "The maximum x-coord of fluid particle is: "<< xind_max << std::endl;
simProg << "The maximum y-coord of fluid particle is: "<< yind_max << std::endl;
simProg << "The maximum z-coord of fluid particle is: "<< zind_max << std::endl;

// Particle position intialization in a crystal structure 
for ( i=1 ; i<= fluidCount/2 ; ++i ){

            // generate random positions
            xind = ((double) rand() / (RAND_MAX)) * ( boxEdge[x] );
            yind = ((double) rand() / (RAND_MAX)) * ( boxEdge[y] );
            // zind = zind_min + ((double) rand() / (RAND_MAX)) * ( zind_max - zind_min );
            zind = ( resCOMZ - resWdth * 0.5 ) + ((double) rand() / (RAND_MAX)) * resWdth;

			// generate random velocities
			rand_gen_velx = ((double) rand() / (RAND_MAX));
			rand_gen_vely = ((double) rand() / (RAND_MAX));
			rand_gen_velz = ((double) rand() / (RAND_MAX));
        
            #if MULTI_VISCOSITY_LIQUIDS
                // initializing particle radius, mass, position and velocity
                    particles.push_back({1.0,1.0,{xind, yind, zind},{rand_gen_velx, rand_gen_vely, rand_gen_velz}, 1});
                    pCount++;
            #else
                // initializing particle radius, mass, position and velocity
                particles.push_back({1.0,1.0,{xind, yind, zind},{rand_gen_velx, rand_gen_vely, rand_gen_velz}, 1});
                pCount++;
            #endif
}// end of xind

for ( i=fluidCount/2 + 1; i<= fluidCount ; ++i ){

            // generate random positions
            xind = ((double) rand() / (RAND_MAX)) * ( boxEdge[x] );
            yind = ((double) rand() / (RAND_MAX)) * ( boxEdge[y] );
            // zind = zind_min + ((double) rand() / (RAND_MAX)) * ( zind_max - zind_min );
            zind = ( resCOMZ - resWdth * 0.5 ) + ((double) rand() / (RAND_MAX)) * resWdth;

			// generate random velocities
			rand_gen_velx = ((double) rand() / (RAND_MAX));
			rand_gen_vely = ((double) rand() / (RAND_MAX));
			rand_gen_velz = ((double) rand() / (RAND_MAX));
        
            #if MULTI_VISCOSITY_LIQUIDS
                // initializing particle radius, mass, position and velocity
                    particles.push_back({1.0,1.0,{xind, yind, zind},{rand_gen_velx, rand_gen_vely, rand_gen_velz}, 2});
                    pCount++;
            #else
                // initializing particle radius, mass, position and velocity
                particles.push_back({1.0,1.0,{xind, yind, zind},{rand_gen_velx, rand_gen_vely, rand_gen_velz}, 1});
                pCount++;
            #endif

}// end of xind

simProg << "finished initialization of  " << pCount << std::endl;
simProg << "***************************************************" << std::endl;
