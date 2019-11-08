// read position and velocity data from the file fibresConfig_posVel.bin
/*
std::ifstream readConfig; 
if ( ! readConfig ) { simProg << "*** input file is empty" << std::endl; abort(); }
readConfig.open( "./restart/posvelrestartfile.dat", std::ios::binary | std::ios::in );
*/

// uint32_t particleType;
int particleType;

ngbrIdxStart=0;

std::ifstream readConfig( "/storage/thejas/src/inputGeometry/paper/fibresConfig_posVel.bin", std::ios::binary | std::ios::in ); 
if ( ! readConfig ) { simProg << "*** The paper configuration file could not be opened/ does not exist *** \n Aborting !! " << std::endl; abort(); }

readConfig.read ( ( char * ) &npart,		            sizeof( unsigned int ) );
simProg << " Positions and velocities of a total of " << npart << " will be read " << std::endl;

for ( j = 0 ; j < npart ; ++ j ){	

	readConfig.read ( ( char * ) &particleType,		sizeof ( particleType ) );

	readConfig.read ( ( char * ) &xind,		        sizeof ( double ) );
	readConfig.read ( ( char * ) &yind,		        sizeof ( double )  );
	readConfig.read ( ( char * ) &zind,		        sizeof ( double )  );

	readConfig.read ( ( char * ) &rand_gen_velx,	sizeof ( double ) );
	readConfig.read ( ( char * ) &rand_gen_vely,	sizeof ( double ) );
	readConfig.read ( ( char * ) &rand_gen_velz,	sizeof ( double ) );

	particles.push_back( {1.0 ,1.0 , {xind, yind, zind} ,{rand_gen_velx, rand_gen_vely, rand_gen_velz}, particleType} );

    //std::cout << particleType << ", " << xind << ", " << yind << ", " << zind << ", " << rand_gen_velx << ", " << rand_gen_vely << ", " << rand_gen_velz << std::endl;

}

ngbrIdxEnd = npart-1;

readConfig.close();
