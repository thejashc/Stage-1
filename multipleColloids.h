double colloidPos[NColloids][3];
double colloidRad = 1.5;
double colloidRadSqr = pow(colloidRad, 2.);
double zcutOff = boxEdge[z];

char dummyString[100];
char type;
char fname[100];

unsigned int NColloidsFile;
unsigned int flag;

pCount = 0;
ngbrIdxStart = 0;

if ( NColloids != 0 ){
    simProg << "***************************************************" << std::endl;
    simProg << "Started initialization of " << NColloids << " colloids and the reservoir" << std::endl;

    // Reading colloid center of mass information
    sprintf(fname, "./readConfig/multipleColloids/Lx_20_Ly_20_Lz_%d/colloid_N_%d.dat", int(resWdth), int( NColloids ) );
    std::ifstream readColloidPos( fname, std::ifstream::in);
    if ( ! readColloidPos ) { simProg << "*** The colloid position file does not exist *** \n Aborting !! " << std::endl; abort(); }

    readColloidPos >> NColloidsFile;

    if ( NColloidsFile != NColloids ){

        simProg << "Inconsistent colloid number" << std::endl;
        exit(0);
    }

    for ( i=0; i<NColloids ; ++i ){
        readColloidPos >> colloidPos[i][0];
        readColloidPos >> colloidPos[i][1];
        readColloidPos >> colloidPos[i][2];
    }

    readColloidPos.close();

    // Reading reservoir information 
    sprintf(fname, "./readConfig/multipleColloids/Lx_20_Ly_20_Lz_%d/reservoir_Lx_20_Ly_20_Lz_%d_2.dat", int(resWdth), int( resWdth ) );
    std::ifstream readReservoirPos( fname, std::ifstream::in);
    if ( ! readReservoirPos ) { simProg << "*** The reservoir file for the colloidal suspension does not exist *** \n Aborting !! " << std::endl; abort(); }

    readReservoirPos >> npart;
    readReservoirPos >> dummyString;
    readReservoirPos >> dummyString;
    readReservoirPos >> dummyString;
    readReservoirPos >> dummyString;

    for ( j=1 ; j <= npart ; ++j ){	

        // generate random velocities
        rand_gen_velx = 0.;
        rand_gen_vely = 0.;
        rand_gen_velz = 0.;

        readReservoirPos >> type;
        readReservoirPos >> xind;
        readReservoirPos >> yind;
        readReservoirPos >> zind;

        flag = 1;
        i = 0;
        while ( flag == 1 && i < NColloids && zind < zcutOff ){

            Rij.X = xind - colloidPos[i][0];
            Rij.Y = yind - colloidPos[i][1];
            Rij.Z = zind - colloidPos[i][2];

            // nearest image distance
            Rij.X = Rij.X - boxEdge[x] * round( Rij.X / boxEdge[x] );
            Rij.Y = Rij.Y - boxEdge[y] * round( Rij.Y / boxEdge[y] );
            Rij.Z = Rij.Z - boxEdge[z] * round( Rij.Z / boxEdge[z] );

            r2 = Rij.getLengthSquared();

            if ( r2 < colloidRadSqr  ){
                particles.push_back({1.0,1.0,{xind, yind, zind + resCOMZ},{rand_gen_velx, rand_gen_vely, resCOMVel}, 0});
                pCount++;
                flag = 0;
            }

            i++;

        }

        // if particle is attached to colloid then it is a fluid
        if ( flag == 1 && zind < zcutOff){
            particles.push_back({1.0,1.0,{xind, yind, zind + resCOMZ},{rand_gen_velx, rand_gen_vely, resCOMVel}, 1});
            pCount++;
        }
    }

    readReservoirPos.close();
}
else{

    // Reading reservoir information 
    sprintf(fname, "./readConfig/multipleColloids/Lx_20_Ly_20_Lz_%d/reservoir_Lx_20_Ly_20_Lz_%d_2.dat", int(resWdth), int( resWdth ) );
    std::ifstream readReservoirPos( fname, std::ifstream::in);
    if ( ! readReservoirPos ) { simProg << "*** The reservoir file for the colloidal suspension does not exist *** \n Aborting !! " << std::endl; abort(); }

    readReservoirPos >> npart;
    readReservoirPos >> dummyString;
    readReservoirPos >> dummyString;
    readReservoirPos >> dummyString;
    readReservoirPos >> dummyString;

    for ( j=1 ; j <= npart ; ++j ){	

        // generate random velocities
        rand_gen_velx = 0.;
        rand_gen_vely = 0.;
        rand_gen_velz = 0.;

        readReservoirPos >> type;
        readReservoirPos >> xind;
        readReservoirPos >> yind;
        readReservoirPos >> zind;

        particles.push_back({1.0,1.0,{xind, yind, zind + resCOMZ},{rand_gen_velx, rand_gen_vely, resCOMVel}, 1});
        pCount++;
    }

    readReservoirPos.close();
}

// domain for connected springs
ngbrIdxEnd   = pCount - 1;

simProg << " The solid particles between the indices " << ngbrIdxStart << " and " << ngbrIdxEnd << " are neighbour connected springs " << std::endl;
