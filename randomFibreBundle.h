std::ifstream readConfig( "./readConfig/randomFibreConfiguration.dat", std::ifstream::in);

if ( ! readConfig ) { simProg << "*** The restart file could not be opened/ does not exist *** \n Aborting !! " << std::endl; abort(); }

unsigned int noOfFibreParticles;
double comZ;
double comZNew;

readConfig >> noOfFibreParticles;
readConfig >> comZ;

ngbrIdxStart = 0;

for ( j=1 ; j <= noOfFibreParticles ; ++j ){

    rand_gen_velx = 0.;
    rand_gen_vely = 0.;
    rand_gen_velz = 0.;

    readConfig >> xind;
    readConfig >> yind;
    readConfig >> zind;

    comZNew += zind - comZ + 100.;
    particles.push_back( {1.0 ,1.0 , {xind , yind , zind - comZ + 100.} ,{rand_gen_velx, rand_gen_vely, rand_gen_velz}, 0} );

}

comZNew /= noOfFibreParticles;
ngbrIdxEnd = noOfFibreParticles - 1;
simProg << "\n" << noOfFibreParticles << " particles positions making-up the fibre were read" << std::endl;

simProg << "\nFibres are connected via springs"  << std::endl;
simProg << "\nParticles from index " << ngbrIdxStart << " to " << ngbrIdxEnd << " are connected via springs " << std::endl;

readConfig.close();
