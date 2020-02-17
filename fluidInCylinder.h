double colloidRad = 1.5;
double colloidRadSqr = pow(colloidRad, 2.);

unsigned int flag;

// read centers of the colloid from file
simProg << "***************************************************" << std::endl;
simProg << "Started initialization of the spherical colloids" << std::endl;
simProg << "Initializing " << NColloids << " colloids" << std::endl;

unsigned int colloidIdx=0;
double colloidPos[NColloids][3];

double xPos, yPos, zPos;
unsigned int dummyNum;

std::ifstream readColloidPos(readColloidsFrom);

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

// read file
readConfig.open(readFluidFrom, std::ios::binary | std::ios::in ); 

if ( ! readConfig ) { simProg << "*** The file could not be opened/ does not exist *** \n Aborting !! " << std::endl; abort(); }

readConfig.read ( ( char * ) &npart, sizeof (unsigned int) );
simProg << " reservoir containing " << npart << " particles the glassy capillary cylinder  being read for fluid\n" << std::endl;

for ( j = 0 ; j < npart ; ++ j ){	

    readConfig.read ( ( char * ) &particleType,		sizeof (unsigned int ) );

    readConfig.read ( ( char * ) &xind,		        sizeof ( double ) );
    readConfig.read ( ( char * ) &yind,		        sizeof ( double )  );
    readConfig.read ( ( char * ) &zind,		        sizeof ( double )  );

    readConfig.read ( ( char * ) &rand_gen_velx,	sizeof ( double ) );
    readConfig.read ( ( char * ) &rand_gen_vely,	sizeof ( double ) );
    readConfig.read ( ( char * ) &rand_gen_velz,	sizeof ( double ) );

    scaledX = xind - origCx;
    scaledY = yind - origCy;

    innerRadius     = ( pow( scaledX, 2.0 ) + pow( scaledY, 2.0 ) <= ri2 );

    // cylinder
    if ( innerRadius && (zind < capLen) ){

        flag = 1;
        i = 0;
        while ( (flag == 1) && i < NColloids ){

            Rij.X = xind - origCx;
            Rij.Y = yind - origCy;
            Rij.Z = zind - boxEdge[z]*0.5;

            // nearest image distance -- periodic boundaries only in the x and y directions
            //Rij.X = Rij.X - boxEdge[x] * round( Rij.X / boxEdge[x] );
            //Rij.Y = Rij.Y - boxEdge[y] * round( Rij.Y / boxEdge[y] );
            //Rij.Z = Rij.Z - boxEdge[z] * round( Rij.Z / boxEdge[z] );

            r2 = Rij.getLengthSquared();

            //std::cout << r2 << ", " << colloidRadSqr << std::endl;

            if ( r2 < colloidRadSqr ){
                particles.push_back({1.0,1.0,{Rij.X + cylCenterX, Rij.Y + cylCenterY, zind},{0., 0., 0.}, 4});
                flag = 0;
            }

            i++;
        }

        if( flag == 1 )
            particles.push_back({1.0,1.0,{scaledX+cylCenterX, scaledY+cylCenterY, zind},{0., 0., 0.},1});

    }
}

readConfig.close();
