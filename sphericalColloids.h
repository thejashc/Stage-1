// properties of reservoir
unsigned int resWdth = 20;
unsigned int resLen = 20;
unsigned int resHeight = 30;

double colloidRad = 1.5;
double colloidRadSqr = pow(colloidRad, 2.);

// cutoff lengths for reading
double xLowerCutOff = 0.;
double xUpperCutOff = boxEdge[x];

double yLowerCutOff = 0.;
double yUpperCutOff = boxEdge[y];

double zLowerCutOff = 0.;
//double zUpperCutOff = (3. / 10.) * resHeight;
double zUpperCutOff = boxEdge[z];

//double zUpperCutOff = (3. / 5.) * resHeight;
//double zLowerCutOff = 10.;
//double zUpperCutOff = (2. / 3.) * resHeight;

//double xOffset = 0.5*( boxEdge[x] - resWdth );
double xOffset = 0.0;
//double zOffset = 3.5;
double zOffset = 0.0;

char dummyString[100];
char type;
char fname[100];

unsigned int flag;

pCount = 0;
double xPos, yPos, zPos;
double spacing = 3.50 * colloidRad * rcutoff;
double Zspacing = 6.00 * colloidRad * rcutoff;

double xColloidMin = 5.5;
double yColloidMin = 5.5;
double zColloidMin = 5.5;
//double zColloidMin = 0.5 * boxEdge[z];

//double xColloidMax = (1. * resWdth) - xColloidMin + 1;
//double yColloidMax = (1. * resLen) - yColloidMin + 1;
double xColloidMax = boxEdge[x];
double yColloidMax = boxEdge[y];
//double zColloidMax = zUpperCutOff - zColloidMin + 1;
//double zColloidMax = zUpperCutOff;
double zColloidMax = boxEdge[z];


// determining the total number of colloids
NColloids = 0;
xPos = xColloidMin;
while ( xPos <= xColloidMax ){
    yPos = yColloidMin;
    while( yPos <= yColloidMax ){
        zPos = zColloidMin;
        while( zPos <= zColloidMax ){

            NColloids += 1;

            //std::cout << zPos << ", " << yPos << ", " << xPos << std::endl;
            zPos += Zspacing;
        }
        yPos += spacing;
    }
    xPos += spacing;
}

// initializing colloid centres
simProg << "***************************************************" << std::endl;
simProg << "Started initialization of the spherical colloids" << std::endl;
simProg << "Initializing " << NColloids << " colloids" << std::endl;

ngbrIdxStart = particles.size();
unsigned int colloidIdx=0;
double colloidPos[NColloids][3];

colloidIdx = 0;
xPos = xColloidMin;
while ( xPos <= xColloidMax ){
    yPos = yColloidMin;
    while( yPos <= yColloidMax ){
        zPos = zColloidMin;
        while( zPos <= zColloidMax ){

            colloidPos[colloidIdx][0] = xPos;
            colloidPos[colloidIdx][1] = yPos;
            colloidPos[colloidIdx][2] = zPos;

            zPos += Zspacing;
            colloidIdx++;
        }
        yPos += spacing;
    }
    xPos += spacing;
}

// Reading reservoir information 
//sprintf(fname, "./readConfig/multipleColloids/Lx_20_Ly_20_Lz_%d/reservoir_Lx_20_Ly_20_Lz_%d_2.dat", int(resWdth), int( resWdth ) );
sprintf(fname, "./readConfig/solids/HARD_SPHERES/Lx_%d_Ly_%d_Lz_%d/reservoir_Lx_%d_Ly_%d_Lz_%d.dat", int(resLen), int(resWdth), int(resHeight), int(resLen), int(resWdth), int(resHeight));
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

    // simProg << "starting initialization of particle " << j << " with positions " << xind << ", " << yind << ", " << zind << std::endl;

    flag = 1;
    i = 0;
    while ( flag == 1 && i < NColloids && xind >= xLowerCutOff && xind <= xUpperCutOff 
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
            particles.push_back({1.0,1.0,{xind + xOffset, yind, zind + zOffset},{rand_gen_velx, rand_gen_vely, rand_gen_velz}, 0});
            pCount++;
            flag = 0;
        }

        i++;
    }

    // if particle is attached to colloid then it is a solid
    if ( flag == 1 && xind >= xLowerCutOff && xind <= xUpperCutOff
                   && yind >= yLowerCutOff && yind <= yUpperCutOff
                   && zind >= zLowerCutOff && zind <= zUpperCutOff ){
        particles.push_back({1.0,1.0,{xind + xOffset, yind, zind + zOffset},{rand_gen_velx, rand_gen_vely, rand_gen_velz}, 1});
        pCount++;
    }
}

readReservoirPos.close();

// domain for connected springs
ngbrIdxEnd   = ngbrIdxStart + pCount - 1;

simProg << " The solid particles from index between " << ngbrIdxStart << " and " << ngbrIdxEnd << " are neighbour connected springs " << std::endl;
simProg << "***************************************************" << std::endl;
//exit(0);
/*
 * ICOSPHERE GEOMETRY
 xind_min = 0.;
 yind_min = 0.;
 zind_min = wallHeight;
 xind_max = boxEdge[x];
 yind_max = boxEdge[y];
 zind_max = boxEdge[z];

 simProg << "***************************************************" << std::endl;
 simProg << "Started initialization of the spherical colloid" << std::endl;

 simProg << "The minimum x-coord of colloid is: "<< xind_min << std::endl;
 simProg << "The minimum y-coord of colloid is: "<< yind_min << std::endl;
 simProg << "The minimum z-coord of colloid is: "<< zind_min << std::endl;

 simProg << "The maximum x-coord of colloid is: "<< xind_max << std::endl;
 simProg << "The maximum y-coord of colloid is: "<< yind_max << std::endl;
 simProg << "The maximum z-coord of colloid is: "<< zind_max << std::endl;

 simProg << "The radius of the colloid is:       "<< capRad << std::endl;

 std::ifstream readConfig( "./readConfig/cluster_rad_1_5_icosphere.dat", std::ifstream::in);
//std::ifstream readConfig( "./readConfig/rad_1_5_icosphere.dat", std::ifstream::in);

if ( ! readConfig ) { simProg << "*** The restart file could not be opened/ does not exist *** \n Aborting !! " << std::endl; abort(); }

readConfig >> solidCount;

for ( j = 1 ; j <= solidCount ; ++ j ){	

// generate random velocities
rand_gen_velx = 0.;
rand_gen_vely = 0.;
rand_gen_velz = 0.;

readConfig >> xind;
readConfig >> yind;
readConfig >> zind;

if ( sqrt( pow( xind, 2. ) + pow( yind, 2. ) + pow( zind, 2. ) ) > capRad - 0.75 )
particles.push_back( {1.0 ,1.0 , {xind + capSphXc, yind + capSphYc, zind + capSphZc} ,{rand_gen_velx, rand_gen_vely, rand_gen_velz}, 0} );
// else
//    particles.push_back( {1.0 ,1.0 , {xind + capSphXc, yind + capSphYc, zind + capSphZc} ,{rand_gen_velx, rand_gen_vely, rand_gen_velz}, 3} );

//std::cout << xind << " " << yind << " " << zind << " " << rand_gen_velx << " " << rand_gen_vely << " " << rand_gen_velz << std::endl;
}

simProg << "\n  " << solidCount << " particles positions and velocities are initialized" << std::endl;

readConfig.close();
*/
