// read position and velocity data from the file fibresConfig_posVel.bin
/*
std::ifstream readConfig; 
if ( ! readConfig ) { simProg << "*** input file is empty" << std::endl; abort(); }
readConfig.open( "./restart/posvelrestartfile.dat", std::ios::binary | std::ios::in );
*/

// uint32_t particleType;
int particleType;
char fname[200];

sprintf(fname,"/storage/thejas/Year3/paperImbibition/Step2_makePaperFromFibre/finalThickness_20/data/posVel130000.bin");
std::ifstream readConfig(fname, std::ios::binary | std::ios::in ); 
if ( ! readConfig ) { simProg << "*** The file"<< fname <<" configuration file could not be opened/ does not exist *** \n Aborting !! " << std::endl; abort(); }

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

    rand_gen_velx=0.;
    rand_gen_vely=0.;
    rand_gen_velz=0.;

	particles.push_back( {1.0 ,1.0 , {xind, yind, zind} ,{rand_gen_velx, rand_gen_vely, rand_gen_velz}, particleType} );
    pCount++;

    //std::cout << particleType << ", " << xind << ", " << yind << ", " << zind << ", " << rand_gen_velx << ", " << rand_gen_vely << ", " << rand_gen_velz << std::endl;

}

readConfig.close();
