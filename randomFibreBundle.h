std::ifstream readConfig( "./readConfig/randomFibreBundle.dat", std::ifstream::in);

if ( ! readConfig ) { simProg << "*** The restart file could not be opened/ does not exist *** \n Aborting !! " << std::endl; abort(); }

unsigned int noOfFibreParticles;

readConfig >> noOfFibreParticles;

for ( j=1 ; j <= noOfFibreParticles ; ++j ){

    rand_gen_velx = 0.;
    rand_gen_vely = 0.;
    rand_gen_velz = 0.;

    readConfig >> xind;
    readConfig >> yind;
    readConfig >> zind;

    particles.push_back( {1.0 ,1.0 , {xind , yind , zind} ,{rand_gen_velx, rand_gen_vely, rand_gen_velz}, 0} );

}

simProg << "\n  " << noOfFibreParticles << " particles positions making-up the fibre were read" << std::endl;

readConfig.close();
