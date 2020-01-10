// read file
bool flag=0;

int particleType;
char fname[100];
unsigned int nTrials=4;
unsigned int npart;

double xlim_min=3.5;
double ylim_min=3.5;

double dia[3];
//double dia[2];

dia[0]=3.;
dia[1]=5.;
//dia[2]=7.5;

double xGap=3.;
double yGap=3.;

double xcenters[2];
double ycenters[2];

double scaledX;
double scaledY;

/*
xcenters[0]=xGap+(dia[0]/2.);
xcenters[1]=xcenters[0]+(xGap+ dia[0]/2. + dia[1]/2.);
xcenters[2]=xcenters[1]+(xGap+ dia[1]/2. + dia[2]/2.);
*/

xcenters[0]=(xGap/2.)+(dia[0]/2.);
xcenters[1]=xcenters[0]+(xGap+ dia[0]*0.5 + dia[1]*0.5);
//xcenters[2]=xcenters[1]+(yGap+ dia[0]);

ycenters[0]=(yGap/2.)+(dia[1]/2.);
ycenters[1]=ycenters[0]+(yGap+ dia[1]);
//ycenters[2]=ycenters[1]+(yGap+ dia[1]);

/*
std::cout << xcenters[0] << ", " << xcenters[1] << ", " << xcenters[2] << std::endl;
std::cout << ycenters[0] << ", " << ycenters[1] << ", " << ycenters[2] << std::endl;
*/

sprintf(fname,"/storage/thejas/src/inputGeometry/glassyWall/combinedSlab/Lx_45_Ly_40_Lz_15/posVel15000.bin");
//sprintf(fname,"../inputGeometry/solid.bin");

std::ifstream readConfig(fname, std::ios::binary | std::ios::in ); 
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

    scaledX = xind - xlim_min;
    scaledY = yind - ylim_min;

    if ( (scaledX > 0.) && ( scaledX < boxEdge[x] ) && ( scaledY > 0.) && ( scaledY < boxEdge[y] ) && ( zind > 0.) && ( zind < 25. )  ){

        for (int i=0; i<2; ++i){
            for( int k=0; k<2; ++k){
            
               if( ( pow(scaledX - xcenters[k], 2.) + pow(scaledY-ycenters[i], 2.) ) > pow(dia[k]/2.,2.) )
                   flag=true;
               else{
                   flag=false;
                   goto break_me_here;
               }
            }
        }

break_me_here:
        if(flag){
            particles.push_back({1.0,1.0,{scaledX, scaledY, zind+bufferLen},{0., 0., 0.},0});
            pCount++;
        }
    }
}

simProg << "Number of particles in wall structure is " << pCount << "\n";

readConfig.close();
