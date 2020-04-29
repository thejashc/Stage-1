// read position and velocity data from the file posvelrestartfile.dat
/*
std::ifstream readConfig; 
if ( ! readConfig ) { simProg << "*** input file is empty" << std::endl; abort(); }
readConfig.open( "./restart/posvelrestartfile.dat", std::ios::binary | std::ios::in );
*/

// uint32_t particleType;
int particleType;
double tempRho;

double r0X;
double r0Y;
double r0Z;

std::ifstream readConfig( "./restart/posvelrestartfile.dat", std::ios::binary | std::ios::in ); 
if ( ! readConfig ) { simProg << "*** The restart file could not be opened/ does not exist *** \n Aborting !! " << std::endl; abort(); }

readConfig.read ( ( char * ) &npart,		            sizeof( unsigned int ) );
simProg << " Positions and velocities of a total of " << npart << " will be read " << std::endl;
#if WALL_ON
    readConfig.read ( ( char * ) &bckgIdxStart,		sizeof( unsigned int ) );
    readConfig.read ( ( char * ) &bckgIdxEnd,		sizeof( unsigned int ) );

    simProg << " bckgIdxStart = " << bckgIdxStart << std::endl;
    simProg << " bckgIdxEnd = " << bckgIdxEnd << std::endl;

    #if SPRING_CONNECTED_SLD
        readConfig.read ( ( char * ) &ngbrIdxStart,		sizeof( unsigned int ) );
        readConfig.read ( ( char * ) &ngbrIdxEnd,		sizeof( unsigned int ) );

        simProg << " ngbrIdxStart = " << ngbrIdxStart << std::endl;
        simProg << " ngbrIdxEnd = " << ngbrIdxEnd << std::endl;
    #endif
#endif
readConfig.read ( ( char * ) &step,		                sizeof( int ) );

simProg << " The simulation will start from step = " << step+1 << std::endl;
//readConfig.read ( ( char * ) &seedRstrt,	            sizeof( std::default_random_engine ) );
readConfig.read ( ( char * ) &gen,                      sizeof( std::mt19937 ) );
readConfig.read ( ( char * ) &normalDistribution,       sizeof( std::normal_distribution<double> ) );

//double dummyRand = randNumGen(seedRstrt);   // random number generator is for the previous time-step .. The sequence continues after this seed
//seed = seedRstrt;	

for ( j = 0 ; j < npart ; ++ j ){	

	readConfig.read ( ( char * ) &particleType,		sizeof ( particleType ) );

	readConfig.read ( ( char * ) &xind,		        sizeof ( double ) );
	readConfig.read ( ( char * ) &yind,		        sizeof ( double )  );
	readConfig.read ( ( char * ) &zind,		        sizeof ( double )  );

	readConfig.read ( ( char * ) &rand_gen_velx,	sizeof ( double ) );
	readConfig.read ( ( char * ) &rand_gen_vely,	sizeof ( double ) );
	readConfig.read ( ( char * ) &rand_gen_velz,	sizeof ( double ) );

	readConfig.read ( ( char * ) &tempRho,	        sizeof ( tempRho ) );

    #if WALL_ON
        readConfig.read ( ( char * ) &r0X,		        sizeof ( double ) );
        readConfig.read ( ( char * ) &r0Y,		        sizeof ( double )  );
        readConfig.read ( ( char * ) &r0Z,		        sizeof ( double )  );
    #endif

	particles.push_back( {1.0 ,1.0 , {xind, yind, zind} ,{rand_gen_velx, rand_gen_vely, rand_gen_velz}, particleType} );

    particles[j].dens = tempRho;

    #if WALL_ON
        if ( particles[j].type == 0 || particles[j].type == 3 ){

            particles[j].r0.X = r0X;
            particles[j].r0.Y = r0Y;
            particles[j].r0.Z = r0Z; }
    #endif



	// simProg << particleType << " " << std::setprecision(15) << xind << std::setprecision(15) << " " << yind << std::setprecision(15) << " " <<  zind << std::endl;
	// simProg << particleType << " " << std::setprecision(15) << rand_gen_velx << std::setprecision(15) << " " << rand_gen_vely << std::setprecision(15) << " " <<  rand_gen_velz << std::endl;
    // simProg << type << " " << rand_gen_velx << " " << rand_gen_vely << " " << rand_gen_velz << std::endl;
}

//simProg << "\n  " << npart << " particles positions and velocities are initialized" << std::endl;

readConfig.close();

step++;

simProg << "\n Restarting simulation from " << step << std::endl;
