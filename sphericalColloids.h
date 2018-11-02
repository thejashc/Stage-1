double capRad = 1.5;
capRadSqr = pow(capRad, 2.);
pCount = 0;

// co-ordinates of center of spherical Cap
capSphXc = boxEdge[x] / 2.;
capSphYc = boxEdge[y] / 2.;
capSphZc = boxEdge[z] / 2.; 

/***************************************** SOLID INIT ********************************/
xind_min = 0.;
yind_min = 0.;
zind_min = 0.;
xind_max = boxEdge[x];
yind_max = boxEdge[y];
zind_max = boxEdge[z];

pCount = 0;

simProg << "***************************************************" << std::endl;
simProg << "Started initialization of the liquid bath" << std::endl;

simProg << "The starting x-coordinate of the liquid bath: "<< xind_min << std::endl;
simProg << "The starting y-coordinate of the liquid bath: "<< yind_min << std::endl;
simProg << "The starting z-coordinate of the liquid bath: "<< zind_min << std::endl;

simProg << "The ending x-coordinate of the liquid bath: "<< xind_max << std::endl;
simProg << "The ending y-coordinate of the liquid bath: "<< yind_max << std::endl;
simProg << "The ending z-coordinate of the liquid bath: "<< zind_max << std::endl;

xind = xind_min;
aCube = pow( 1. / initRho, 1./3. );
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
            bool outerCore =    ( pow( xind - capSphXc , 2. ) + pow( yind - capSphYc, 2. ) + pow( zind - capSphZc , 2. ) >= pow(capRad - 0.75, 2.) ) && 
                                ( pow( xind - capSphXc , 2. ) + pow( yind - capSphYc, 2. ) + pow( zind - capSphZc , 2. ) <= capRadSqr );

            bool innerCore = ( pow( xind - capSphXc , 2. ) + pow( yind - capSphYc, 2. ) + pow( zind - capSphZc , 2. ) <= pow(capRad - 0.75, 2.) );

            if ( outerCore  )
                particles.push_back({1.0,1.0,{xind, yind, zind},{rand_gen_velx, rand_gen_vely, rand_gen_velz}, 0});
            else if ( innerCore ) 
                particles.push_back({1.0,1.0,{xind, yind, zind},{rand_gen_velx, rand_gen_vely, rand_gen_velz}, 3});

            pCount++;

            // update zind
            zind += aCube * rcutoff;
        }// end of zind
        yind += aCube * rcutoff;
    }// end of yind			
    xind += aCube * rcutoff;
}// end of xind

simProg << "finished initialization of  " << pCount << " particles inside crystal lattice" << std::endl;
simProg << "***************************************************" << std::endl;
/***************************************** FLUID INIT ********************************/
xind_min = 0.;
yind_min = 0.;
zind_min = 0.;
xind_max = boxEdge[x];
yind_max = boxEdge[y];
zind_max = boxEdge[z];

pCount = 0;

simProg << "***************************************************" << std::endl;
simProg << "Started initialization of the liquid bath" << std::endl;

simProg << "The starting x-coordinate of the liquid bath: "<< xind_min << std::endl;
simProg << "The starting y-coordinate of the liquid bath: "<< yind_min << std::endl;
simProg << "The starting z-coordinate of the liquid bath: "<< zind_min << std::endl;

simProg << "The ending x-coordinate of the liquid bath: "<< xind_max << std::endl;
simProg << "The ending y-coordinate of the liquid bath: "<< yind_max << std::endl;
simProg << "The ending z-coordinate of the liquid bath: "<< zind_max << std::endl;

xind = xind_min;
aCube = pow( 1. / initRho, 1./3. );
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
            if ( ( pow( xind - capSphXc , 2. ) + pow( yind - capSphYc, 2. ) + pow( zind - capSphZc , 2. ) >= capRadSqr ) )
                particles.push_back({1.0,1.0,{xind, yind, zind},{rand_gen_velx, rand_gen_vely, rand_gen_velz}, 1});

            // particles.push_back({1.0,1.0,{xind, yind, zind},{rand_gen_velx, rand_gen_vely, rand_gen_velz}, 3});
            // particles.push_back({1.0,1.0,{xind, yind, zind},{ 0., 0., 0.}, 3});
            pCount++;

            // update zind
            zind += aCube * rcutoff;
        }// end of zind
        yind += aCube * rcutoff;
    }// end of yind			
    xind += aCube * rcutoff;
}// end of xind

simProg << "finished initialization of  " << pCount << " particles inside crystal lattice" << std::endl;
simProg << "***************************************************" << std::endl;

/*
 * ICOSPHERE GEOMETRY
xind_min = 0.;
yind_min = 0.;
zind_min = wallHeight;
xind_max = boxEdge[x];
yind_max = boxEdge[y];
zind_max = boxEdge[z];

simProg << "***************************************************" << std::endl;
simProg << "Started initialization of the spherical colloid" << std::endl;

simProg << "The minimum x-coord of colloid is: "<< xind_min << std::endl;
simProg << "The minimum y-coord of colloid is: "<< yind_min << std::endl;
simProg << "The minimum z-coord of colloid is: "<< zind_min << std::endl;

simProg << "The maximum x-coord of colloid is: "<< xind_max << std::endl;
simProg << "The maximum y-coord of colloid is: "<< yind_max << std::endl;
simProg << "The maximum z-coord of colloid is: "<< zind_max << std::endl;

simProg << "The radius of the colloid is:       "<< capRad << std::endl;

std::ifstream readConfig( "./readConfig/cluster_rad_1_5_icosphere.dat", std::ifstream::in);
//std::ifstream readConfig( "./readConfig/rad_1_5_icosphere.dat", std::ifstream::in);

if ( ! readConfig ) { simProg << "*** The restart file could not be opened/ does not exist *** \n Aborting !! " << std::endl; abort(); }

readConfig >> solidCount;

for ( j = 1 ; j <= solidCount ; ++ j ){	

    // generate random velocities
    rand_gen_velx = 0.;
    rand_gen_vely = 0.;
    rand_gen_velz = 0.;

    readConfig >> xind;
    readConfig >> yind;
    readConfig >> zind;

    if ( sqrt( pow( xind, 2. ) + pow( yind, 2. ) + pow( zind, 2. ) ) > capRad - 0.75 )
        particles.push_back( {1.0 ,1.0 , {xind + capSphXc, yind + capSphYc, zind + capSphZc} ,{rand_gen_velx, rand_gen_vely, rand_gen_velz}, 0} );
   // else
   //    particles.push_back( {1.0 ,1.0 , {xind + capSphXc, yind + capSphYc, zind + capSphZc} ,{rand_gen_velx, rand_gen_vely, rand_gen_velz}, 3} );

    //std::cout << xind << " " << yind << " " << zind << " " << rand_gen_velx << " " << rand_gen_vely << " " << rand_gen_velz << std::endl;
}

simProg << "\n  " << solidCount << " particles positions and velocities are initialized" << std::endl;

readConfig.close();
*/
