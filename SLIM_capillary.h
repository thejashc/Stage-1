xind_min = 0.;
yind_min = 0.;
zind_min = wallHeight;
xind_max = boxEdge[x];
yind_max = boxEdge[y];
zind_max = boxEdge[z];

simProg << "***************************************************" << std::endl;
simProg << "Started initialization of particle positions and momenta from file" << std::endl;

/*
   simProg << "The minimum x-coord of colloid is: "<< xind_min << std::endl;
   simProg << "The minimum y-coord of colloid is: "<< yind_min << std::endl;
   simProg << "The minimum z-coord of colloid is: "<< zind_min << std::endl;

   simProg << "The maximum x-coord of colloid is: "<< xind_max << std::endl;
   simProg << "The maximum y-coord of colloid is: "<< yind_max << std::endl;
   simProg << "The maximum z-coord of colloid is: "<< zind_max << std::endl;
*/

//std::ifstream readConfig( "./readConfig/SLIM_position.dat", std::ifstream::in);
//std::ifstream readVel( "./readConfig/SLIM_momenta.dat", std::ifstream::in);
std::ifstream readConfig( "./restart/capImb/XYZ129000.xyz", std::ifstream::in);
std::ifstream readVel( "./restart/capImb/velocity129000.dat", std::ifstream::in);

if ( ! readConfig ) { simProg << "*** The restart file could not be opened/ does not exist *** \n Aborting !! " << std::endl; abort(); }
if ( ! readVel ) { simProg << "*** The restart file could not be opened/ does not exist *** \n Aborting !! " << std::endl; abort(); }

char dummyString[80];
char dummyChar;
char type;

readConfig >> npart;
readConfig >> dummyString;
readConfig >> dummyString;
readConfig >> dummyString;
readConfig >> dummyString;

readVel >> npart;
readVel >> dummyString;
readVel >> dummyString;
readVel >> dummyString;

for ( j = 1 ; j <= npart ; ++ j ){	

    // generate random velocities
    readVel >> dummyChar;
    readVel >> rand_gen_velx;
    readVel >> rand_gen_vely;
    readVel >> rand_gen_velz;

    rand_gen_velx = 0.;
    rand_gen_vely = 0.;
    rand_gen_velz = 0.;

    readConfig >> type;
    readConfig >> xind;
    readConfig >> yind;
    readConfig >> zind;

    if ( type == 'O' )
        particles.push_back( {1.0 ,1.0 , {xind , yind , zind } ,{rand_gen_velx, rand_gen_vely, rand_gen_velz}, 0} );
    else if ( type == 'H' )
        particles.push_back( {1.0 ,1.0 , {xind , yind , zind } ,{rand_gen_velx, rand_gen_vely, rand_gen_velz}, 1} );
    else if ( type == 'C' )
        particles.push_back( {1.0 ,1.0 , {xind , yind , zind } ,{rand_gen_velx, rand_gen_vely, rand_gen_velz}, 2} );
    else if ( type == 'B' )
        particles.push_back( {1.0 ,1.0 , {xind , yind , zind } ,{rand_gen_velx, rand_gen_vely, rand_gen_velz}, 3} );

    // simProg << type << "\t" << xind << "\t" << yind << "\t" << zind << "\t" << rand_gen_velx << "\t" << rand_gen_vely << "\t" << rand_gen_velz << std::endl; 

}

simProg << "\n  " << npart << " particles positions and velocities were read" << std::endl;

readConfig.close();
readVel.close();
