unsigned int noOfParticles;
unsigned int dummyNumber;
char dummyString[50];
char particleType[10];

// determine number of particles to read
std::ifstream fibrePos( "./readConfig/randomFibreBundle/randomFibreConfiguration.dat", std::ifstream::in);
if ( ! fibrePos ) { simProg << "*** The fibre position file could not be opened/ does not exist *** \n Aborting !! " << std::endl; abort(); }

fibrePos >> noOfParticles;

fibrePos.close();

// read particles from the squashed fibre file and find z-coordinate center of mass
std::ifstream readConfig( "./readConfig/compressedFibre/nFibre_43_finalHeight_20_kWallNgbr_1000.xyz", std::ifstream::in);

if ( ! readConfig ) { simProg << "*** The file for the compressed fibre could not be opened/ does not exist *** \n Aborting !! " << std::endl; abort(); }

readConfig >> dummyNumber;
readConfig >> dummyString;
readConfig >> dummyString;
readConfig >> dummyString;
readConfig >> dummyString;

ngbrIdxStart = 0;
double shiftZ = 0.5* ( boxEdge[z] - 1.);
double comZ;

for ( j=1 ; j <= noOfParticles ; ++j ){

    readConfig >> particleType;
    readConfig >> xind;
    readConfig >> yind;
    readConfig >> zind;

    comZ += zind;

}

comZ /= noOfParticles;
readConfig.close();
readConfig.clear();

// read positions of the fibre into an array
readConfig.open( "./readConfig/compressedFibre/nFibre_43_finalHeight_20_kWallNgbr_1000.xyz", std::ifstream::in);

if ( ! readConfig ) { simProg << "*** The file for the compressed fibre could not be opened/ does not exist *** \n Aborting !! " << std::endl; abort(); }

readConfig >> dummyNumber;
readConfig >> dummyString;
readConfig >> dummyString;
readConfig >> dummyString;
readConfig >> dummyString;

for ( j=1 ; j <= noOfParticles ; ++j ){

    rand_gen_velx = 0.0;
    rand_gen_vely = 0.0;
    rand_gen_velz = 0.0;

    readConfig >> particleType;
    readConfig >> xind;
    readConfig >> yind;
    readConfig >> zind;

    particles.push_back( {1.0 ,1.0 , {xind , yind , zind-comZ + shiftZ} ,{rand_gen_velx, rand_gen_vely, rand_gen_velz}, 0} );

}

readConfig.close();
readConfig.clear();

ngbrIdxEnd = noOfParticles - 1;
simProg << "\n" << noOfParticles << " particles positions making-up the fibre were read" << std::endl;

simProg << "\nFibres are connected via springs"  << std::endl;
simProg << "\nParticles from index " << ngbrIdxStart << " to " << ngbrIdxEnd << " are connected via springs " << std::endl;
