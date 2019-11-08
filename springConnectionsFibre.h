double posBefSquash[npart][3];      // position of the particle before squashing

// read particle positions before squashing
readConfig.open( "/storage/thejas/src/inputGeometry/paper/fibresConfig_posVel.bin", std::ios::binary | std::ios::in ); 
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

    posBefSquash[j][0]=xind;
    posBefSquash[j][1]=yind;
    posBefSquash[j][2]=zind;
}

simProg << "The position of all particles before squashing has been read" << std::endl;
readConfig.close();

std::ifstream partPerFibre( "/storage/thejas/src/inputGeometry/paper/particlesPerFibre.dat", std::ifstream::in);
if ( ! partPerFibre ) { simProg << "*** The parts per fibre file could not be opened/ does not exist *** \n Aborting !! " << std::endl; abort(); }

unsigned int bondsPerFibre;
unsigned int noOfFibres;
unsigned int startIdx, endIdx;

// read all the startingIndices of fibres
partPerFibre >> noOfFibres;
unsigned int fibreStartId[noOfFibres];

for ( i=0; i< noOfFibres; ++i )
    partPerFibre >> fibreStartId[i];

startIdx = 0;
endIdx = fibreStartId[0] - 1;

simProg << "A total of " << noOfFibres << " fibres will be read " << std::endl;

// attach particles within a fibre
for (unsigned int m=1; m <= noOfFibres; m++ ){

    bondsPerFibre = springNetworkFixed(startIdx, endIdx, posBefSquash);

    simProg << "Fibre No.: " << m 
            << ", startidx : " << startIdx 
            << ", endIdx: " << endIdx 
            << ", noOfParticles: " << endIdx - (startIdx - 1)
            << ", bonds: " << bondsPerFibre << std::endl;

    startIdx = endIdx + 1;
    endIdx = endIdx + fibreStartId[m];
}

partPerFibre.close();
