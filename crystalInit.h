// read position of a fcc cylinder
char posfname[100];

/*
readConfig >> dummyString;
readConfig >> dummyString;
readConfig >> dummyString;
readConfig >> dummyString;
*/

/***************************** Single Component / Multi component fluid fcc crystal *************************************/
//sprintf(posfname, "/storage/thejas/src/readConfig/fluids/CRYSTAL/fccCrystalMC_boxSize_%d.dat", int(boxEdge[x]) );
//sprintf(posfname, "/storage/thejas/src/readConfig/fluids/CRYSTAL/fccCrystalMC_boxSize_%d_3Layer.dat", int(boxEdge[x]) );
sprintf(posfname, "./readConfig/fluids/CRYSTAL/fccCrystalMC_boxSize_%d_3Layer.dat", int(boxEdge[x]) );

std::ifstream readConfig( posfname, std::ifstream::in);

if ( ! readConfig ) { simProg << "*** The file " << posfname << " could not be opened/ does not exist *** \n Aborting !! " << std::endl; abort(); }

char dummyString[80];
char dummyChar;
char type;
int flag =0;
int pCount =0;
double offSet = 0.000001;
double zoffSet = boxEdge[z] * 0.5;

readConfig >> npart;

for ( j = 1 ; j <= npart ; ++ j ){	

    // generate random velocities
    rand_gen_velx = 0.;
    rand_gen_vely = 0.;
    rand_gen_velz = 0.;

    readConfig >> type;
    readConfig >> xind;
    readConfig >> yind;
    readConfig >> zind;

    if ( type == 'H' ){
        particles.push_back( {1.0 ,1.0 , {xind + offSet, yind + offSet, zind+zoffSet+offSet} ,{rand_gen_velx, rand_gen_vely, 0.}, 1} );
        //particles.push_back( {1.0 ,1.0 , {xind + offSet, yind + offSet, zind + zOffset} ,{rand_gen_velx, rand_gen_vely, 0.}, 1} );
    }
    else if ( type == 'C' ){
        particles.push_back( {1.0 ,1.0 , {xind + offSet, yind + offSet, zind+zoffSet+offSet} ,{rand_gen_velx, rand_gen_vely, 0.}, 2} );
        //particles.push_back( {1.0 ,1.0 , {xind + offSet, yind + offSet, zind + zOffset} ,{rand_gen_velx, rand_gen_vely, 0.}, 2} );
    }

    pCount++;
    // simProg << type << "\t" << xind << "\t" << yind << "\t" << zind << "\t" << rand_gen_velx << "\t" << rand_gen_vely << "\t" << rand_gen_velz << std::endl; 
}

simProg << "\n  " << npart << " particles making the fcc lattice were read" << std::endl;

readConfig.close();
