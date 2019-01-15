std::ifstream fibrePos( "./readConfig/randomFibreBundle/randomFibreConfiguration.dat", std::ifstream::in);

if ( ! fibrePos ) { simProg << "*** The fibre position file could not be opened/ does not exist *** \n Aborting !! " << std::endl; abort(); }

double shiftZ = 200.;
double comZNew;
double offset;

unsigned int noOfParticles;
double minX, minY, minZ, maxX, maxY, maxZ;
unsigned int fibId = 0;    // ID for the fibre

fibrePos >> noOfParticles;
fibrePos >> minX;
fibrePos >> maxX;
fibrePos >> minY;
fibrePos >> maxY;
fibrePos >> minZ;
fibrePos >> maxZ;

offset = -minZ + shiftZ;

ngbrIdxStart = 0;
for ( j=1 ; j <= noOfParticles ; j++ ){

    rand_gen_velx = 0.;
    rand_gen_vely = 0.;
    rand_gen_velz = 0.;

    fibrePos >> xind;
    fibrePos >> yind;
    fibrePos >> zind;

    comZNew += zind + offset;
    particles.push_back( {1.0 ,1.0 , {xind , yind , zind - minZ + shiftZ} ,{rand_gen_velx, rand_gen_vely, rand_gen_velz}, 0} ); // shift particles up by a value minZ
}

// shifted minZ and maxZ
minZ += offset;
maxZ += offset;

comZNew /= noOfParticles;
ngbrIdxEnd = noOfParticles - 1;
simProg << "\n" << noOfParticles << " particles positions making-up the fibre were read" << std::endl;
simProg << "\n" << comZNew << " is the z-coordinate center of mass" << std::endl;
simProg << "\nFibres are connected via springs"  << std::endl;
simProg << "\nParticles from index " << ngbrIdxStart << " to " << ngbrIdxEnd << " are connected via springs\n" << std::endl;

fibrePos.close();
