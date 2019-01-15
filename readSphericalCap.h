// read particles from the squashed fibre file and find z-coordinate center of mass
readConfig.open( "./readConfig/sphericalCap/sphericalCap_rad_7.5.xyz", std::ifstream::in);

if ( ! readConfig ) { simProg << "*** The file for the spherical cap could not be opened/ does not exist *** \n Aborting !! " << std::endl; abort(); }

readConfig >> noOfParticles;
readConfig >> dummyString;
readConfig >> dummyString;
readConfig >> dummyString;
readConfig >> dummyString;

ngbrIdxStart = 0;
shiftZ = 0.5* ( boxEdge[z] + 33.);

comZ = 0.;
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
readConfig.open( "./readConfig/sphericalCap/sphericalCap_rad_7.5.xyz", std::ifstream::in);

if ( ! readConfig ) { simProg << "*** The file for the spherical cap could not be opened/ does not exist *** \n Aborting !! " << std::endl; abort(); }

readConfig >> noOfParticles;
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

    particles.push_back( {1.0 ,1.0 , {xind , yind , zind-comZ + shiftZ} ,{rand_gen_velx, rand_gen_vely, rand_gen_velz}, 1} );

}

readConfig.close();
readConfig.clear();
ngbrIdxEnd = noOfParticles - 1;
simProg << "\n" << noOfParticles << " particles positions making-up the spherical cap were read" << std::endl;
