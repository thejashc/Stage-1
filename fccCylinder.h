// read position of a fcc cylinder
char posfname[100];

/*
readConfig >> dummyString;
readConfig >> dummyString;
readConfig >> dummyString;
readConfig >> dummyString;
*/

/***************************** CYLINDER *************************************/
//sprintf(posfname, "./readConfig/solids/FCC_CYLINDER/cylinder.dat");
sprintf(posfname, "./readConfig/solids/FCC_CYLINDER/cylinder_capRad_15.dat");
//sprintf(posfname, "/storage/thejas/src/inputGeometry/fcc_cylinder.dat");

std::ifstream readConfig( posfname, std::ifstream::in);

if ( ! readConfig ) { simProg << "*** The file " << posfname << " could not be opened/ does not exist *** \n Aborting !! " << std::endl; abort(); }

char dummyString[80];
char dummyChar;
char type;
int flag =0;
int pCount =0;
double offSet=0.0000001;

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

    if ( type == 'O' ){

        if ( flag == 0 ){
            bckgIdxStart = pCount;
            flag = 1;
        }

        particles.push_back( {1.0 ,1.0 , {xind , yind , zind + bufferLen} ,{rand_gen_velx, rand_gen_vely, rand_gen_velz}, 0} );
    }
    else if ( type == 'B' ){

        if ( flag == 0 ){
            bckgIdxStart = pCount;
            flag = 1;
        }

        particles.push_back( {1.0 ,1.0 , {xind , yind , zind + bufferLen} ,{rand_gen_velx, rand_gen_vely, rand_gen_velz}, 3} );
    }

    pCount++;
    // simProg << type << "\t" << xind << "\t" << yind << "\t" << zind << "\t" << rand_gen_velx << "\t" << rand_gen_vely << "\t" << rand_gen_velz << std::endl; 
}

simProg << "\n  " << npart << " particles making the fcc lattice were read" << std::endl;

readConfig.close();
/***************************** WALL ABOVE CYLINDER *************************************/
//sprintf(posfname, "./readConfig/solids/FCC_CYLINDER/wall.dat");
sprintf(posfname, "./readConfig/solids/FCC_CYLINDER/wall_capRad_15.dat");
//sprintf(posfname, "/storage/thejas/src/inputGeometry/fcc_wall.dat");

//std::ifstream readConfig( posfname, std::ifstream::in);
readConfig.open(posfname, std::ifstream::in);

if ( ! readConfig ) { simProg << "*** The file " << posfname << " could not be opened/ does not exist *** \n Aborting !! " << std::endl; abort(); }

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

    if ( type == 'O' ){

        particles.push_back( {1.0 ,1.0 , {xind+offSet, yind+offSet, zind + bufferLen} ,{rand_gen_velx, rand_gen_vely, rand_gen_velz}, 0} );
    }
    else if ( type == 'B' ){

        particles.push_back( {1.0 ,1.0 , {xind+offSet, yind+offSet, zind + bufferLen} ,{rand_gen_velx, rand_gen_vely, rand_gen_velz}, 3} );
    }

    pCount++;
    // simProg << type << "\t" << xind << "\t" << yind << "\t" << zind << "\t" << rand_gen_velx << "\t" << rand_gen_vely << "\t" << rand_gen_velz << std::endl; 
}

bckgIdxEnd = pCount -1;
simProg << "\n  " << npart << " particles making the fcc wall above the cylinder  were read" << std::endl;

readConfig.close();
