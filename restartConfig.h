// read position and velocity data from the file posvelrestartfile.dat
/*
std::ifstream readConfig; 
if ( ! readConfig ) { simProg << "*** input file is empty" << std::endl; abort(); }
readConfig.open( "./restart/posvelrestartfile.dat", std::ios::binary | std::ios::in );
*/

std::ifstream readConfig( "./restart/posvelrestartfile.dat", std::ios::binary | std::ios::in ); 
if ( ! readConfig ) { simProg << "*** The restart file could not be opened/ does not exist *** \n Aborting !! " << std::endl; abort(); }

readConfig.read ( ( char * ) &npart,		sizeof( int ) );
readConfig.read ( ( char * ) &step,		sizeof( int ) );
readConfig.read ( ( char * ) &seedRstrt,	sizeof( std::default_random_engine ) );

seed = seedRstrt;	// setting the seed for the uniform random number generation

for ( j = 1 ; j <= npart ; ++ j ){	

	readConfig.read ( ( char * ) &xind,		sizeof ( double ) );
	readConfig.read ( ( char * ) &yind,		sizeof ( double )  );
	readConfig.read ( ( char * ) &zind,		sizeof ( double )  );
	readConfig.read ( ( char * ) &rand_gen_velx,	sizeof ( double ) );
	readConfig.read ( ( char * ) &rand_gen_vely,	sizeof ( double ) );
	readConfig.read ( ( char * ) &rand_gen_velz,	sizeof ( double ) );

	particles.push_back( {1.0 ,1.0 , {xind, yind, zind} ,{rand_gen_velx, rand_gen_vely, rand_gen_velz}, 1} );
	// simProg << xind << " " << yind << " " << zind << " " << rand_gen_velx << " " << rand_gen_vely << " " << rand_gen_velz << std::endl;
}

simProg << "\n  " << npart << " particles positions and velocities are initialized" << std::endl;

readConfig.close();

step++; // the simulation input should be used time-step of step + 1

simProg << "\n Restarting simulation from " << step << ". The seed of the random number generator is  " << seed << std::endl;
