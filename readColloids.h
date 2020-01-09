double xLowerCutOff=0.5*(origLx - boxEdge[x]);            // original Lx=35, Ly=30, Lz=120
double xUpperCutOff=0.5*(origLx + boxEdge[x]);            

double yLowerCutOff=0.;
double yUpperCutOff=boxEdge[y];

double zLowerCutOff=0.;
double zUpperCutOff=50.;

double colloidRad = 1.5;
double colloidRadSqr = pow(colloidRad, 2.);

double zOffset = bufferLen + capLen + capWallWdth + 40.;

unsigned int flag;

// read centers of the colloid from file
simProg << "***************************************************" << std::endl;
simProg << "Started initialization of the spherical colloids" << std::endl;
simProg << "Initializing " << NColloids << " colloids" << std::endl;

unsigned int colloidIdx=0;
double colloidPos[NColloids][3];

double xPos, yPos, zPos;
unsigned int dummyNum;

sprintf(fname, "./readConfig/solids/HARD_SPHERES/colloid_N_%d.dat", NColloids);
std::ifstream readColloidPos( fname, std::ifstream::in );

if ( ! readColloidPos ) { simProg << "*** The colloids position file does not exist *** \n Aborting !! " << std::endl; abort(); }

readColloidPos >> dummyNum;
if( dummyNum != NColloids ){
    simProg << " file containing colloid position is not available *** \n Aborting" << std::endl; 
    abort();
}

for (i=0; i<NColloids; ++i){

    readColloidPos >> xPos;
    readColloidPos >> yPos;
    readColloidPos >> zPos;

    colloidPos[colloidIdx][0] = xPos;
    colloidPos[colloidIdx][1] = yPos;
    colloidPos[colloidIdx][2] = zPos;

    colloidIdx++;
}

readColloidPos.close();

sprintf(fname,"./inputGeometry/glassyWall/combinedSlab/Lx_35_Ly_30_Lz_30/posVel6500.bin");

//sprintf(fname,"../inputGeometry/solid.bin");

readConfig.open(fname, std::ios::binary | std::ios::in ); 
//readConfig.open(fname, std::ios::binary | std::ios::in ); 
if ( ! readConfig ) { simProg << "*** The file could not be opened/ does not exist *** \n Aborting !! " << std::endl; abort(); }

readConfig.read ( ( char * ) &npart, sizeof (unsigned int) );
simProg << " reservoir containing " << npart << " particles the glassy capillary cylinder  being read \n" << std::endl;

for ( j = 0 ; j < npart ; ++ j ){	

    readConfig.read ( ( char * ) &particleType,		sizeof (unsigned int ) );

    readConfig.read ( ( char * ) &xind,		        sizeof ( double ) );
    readConfig.read ( ( char * ) &yind,		        sizeof ( double )  );
    readConfig.read ( ( char * ) &zind,		        sizeof ( double )  );

    readConfig.read ( ( char * ) &rand_gen_velx,	sizeof ( double ) );
    readConfig.read ( ( char * ) &rand_gen_vely,	sizeof ( double ) );
    readConfig.read ( ( char * ) &rand_gen_velz,	sizeof ( double ) );

    flag = 1;
    i = 0;
    while ( flag == 1 && i < NColloids 
            && xind >= xLowerCutOff && xind <= xUpperCutOff 
            && yind >= yLowerCutOff && yind <= yUpperCutOff 
            && zind >= zLowerCutOff && zind <= zUpperCutOff ){

        Rij.X = xind - colloidPos[i][0];
        Rij.Y = yind - colloidPos[i][1];
        Rij.Z = zind - colloidPos[i][2];

        // nearest image distance -- periodic boundaries only in the x and y directions
        Rij.X = Rij.X - boxEdge[x] * round( Rij.X / boxEdge[x] );
        Rij.Y = Rij.Y - boxEdge[y] * round( Rij.Y / boxEdge[y] );
        //Rij.Z = Rij.Z - boxEdge[z] * round( Rij.Z / boxEdge[z] );

        r2 = Rij.getLengthSquared();

        if ( r2 < colloidRadSqr  ){
            particles.push_back({1.0,1.0,{xind, yind, zind + zOffset},{rand_gen_velx, rand_gen_vely, rand_gen_velz}, 4});
            flag = 0;
        }

        i++;
    }

    // only if particle is attached to colloid then it is a solid, else it is a fluid particle
    if ( flag == 1 
            && xind >= xLowerCutOff && xind <= xUpperCutOff
            && yind >= yLowerCutOff && yind <= yUpperCutOff
            && zind >= zLowerCutOff && zind <= zUpperCutOff ){
        particles.push_back({1.0,1.0,{xind, yind, zind + zOffset},{rand_gen_velx, rand_gen_vely, rand_gen_velz}, 1});
    }

    /*
       std::cout << type << "\t"   << xind << "\t" << std::setprecision(15)  
       << yind << "\t"  << std::setprecision(15)     
       << zind << "\t" << std::setprecision(15)   
       << rand_gen_velx << "\t" << std::setprecision(15)  
       << rand_gen_vely << "\t" << std::setprecision(15)  
       << rand_gen_velz << "\n";
       exit(0);
       */
}

readConfig.close();
