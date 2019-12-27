bckgIdxStart=0;

// read file
unsigned int particleType;
unsigned int npart;

bool partOfPillar;
bool assign=false;

char fname[100];

double dummy1;
double dummy2;

double xDist;
double yDist;

double scaleFactor=5.;
double scaleFactor2=pow(scaleFactor,2.);

unsigned int pCountCyl=0;
unsigned int pCountFluid=0;

double porosity=0.9;
double ATot=20.*20.;
unsigned int NPillars=1;
double pillarRad2=( ATot / (NPillars * 3.14159) ) * ( 1. - porosity ) * scaleFactor2;

std::ifstream readConfig;

double pillarCenters[NPillars][2];

simProg << "The pillar radius for a porosity of " << porosity << " is = " << sqrt(pillarRad2) << std::endl;

sprintf(fname,"./inputGeometry/randomPillarAssembly_porosity_0_6_check.dat");

simProg << " ********************************************************" << std::endl;
simProg << " Reading the centers of the cylinders for a porosity of " << porosity << std::endl;

readConfig.open(fname, std::ios::in ); 
if ( ! readConfig ) { simProg << "*** The file could not be opened/ does not exist *** \n Aborting !! " << std::endl; abort(); }

for ( i=0; i<NPillars; ++i ){

    readConfig >> dummy1;
    readConfig >> dummy2;

    pillarCenters[i][0] = dummy1 * scaleFactor;
    pillarCenters[i][1] = dummy2 * scaleFactor;

    //simProg << pillarCenters[i][0] << "\t" << pillarCenters[i][1] << std::endl;
}
readConfig.close();
simProg << " Reading the centers of the cylinders for a porosity of " << porosity << std::endl;
simProg << " ********************************************************" << std::endl;

sprintf(fname,"./inputGeometry/2D_Fluid/data/posVel100000.bin");
simProg << " Reading the equilibriated fluid particles " << porosity << std::endl;

readConfig.open( fname, std::ios_base::in);
if ( ! readConfig ) { simProg << "*** The file could not be opened/ does not exist *** \n Aborting !! " << std::endl; abort(); }

readConfig.read ( ( char * ) &npart, sizeof (unsigned int) );
simProg << "2D planar fluid containing " << npart << " particles  is being read\n" << std::endl;

if( readConfig.is_open() ){

    for ( j = 0 ; j < npart ; ++ j ){	

        readConfig.read ( ( char * ) &particleType,		sizeof (unsigned int ) );

        readConfig.read ( ( char * ) &xind,		        sizeof ( double ) );
        readConfig.read ( ( char * ) &yind,		        sizeof ( double )  );
        readConfig.read ( ( char * ) &zind,		        sizeof ( double )  );

        readConfig.read ( ( char * ) &rand_gen_velx,	sizeof ( double ) );
        readConfig.read ( ( char * ) &rand_gen_vely,	sizeof ( double ) );
        readConfig.read ( ( char * ) &rand_gen_velz,	sizeof ( double ) );

        for( i=0 ; i < NPillars; ++i ){

            xDist = xind - pillarCenters[i][0];
            yDist = yind - pillarCenters[i][1];

            xDist -= boxEdge[x] * round( xDist / boxEdge[x] ); 
            yDist -= boxEdge[y] * round( yDist / boxEdge[y] ); 

            partOfPillar= ( pow( xDist, 2.) + pow( yDist, 2.) < pillarRad2 );

            if ( partOfPillar ){
                particles.push_back({1.0,1.0,{xind, yind, zind},{0., 0., 0.},0});
                pCountCyl++;
            }
        }

        /*
           simProg << particleType << "\t"   << xind << "\t" << std::setprecision(15)  
           << yind << "\t"  << std::setprecision(15)     
           << zind << "\t" << std::setprecision(15)   
           << rand_gen_velx << "\t" << std::setprecision(15)  
           << rand_gen_vely << "\t" << std::setprecision(15)  
           << rand_gen_velz << std::endl;
           */
    }

}
else{
    std::cout << "Error opening file" << std::endl;
    exit(0);
}

bckgIdxEnd=pCountCyl-1;

readConfig.close();
simProg << "Number of particles in pillars with radius = " << sqrt(pillarRad2) << " is " << pCountCyl << std::endl;

/*****************************************************************************************************************/
/************************************* READING FLUID *************************************************************/
/*****************************************************************************************************************/
pCountFluid=0;

std::ifstream readFluidConfig(fname, std::ios::binary | std::ios::in );  

if ( ! readFluidConfig ) { simProg << "*** The file could not be opened/ does not exist *** \n Aborting !! " << std::endl; abort(); }

readFluidConfig.read ( ( char * ) &npart, sizeof (unsigned int) );
simProg << "2D planar fluid containing " << npart << " particles  is being read\n" << std::endl;

if( readFluidConfig.is_open() ){

    for ( j = 0 ; j < npart ; ++ j ){	

        readFluidConfig.read ( ( char * ) &particleType,		sizeof (unsigned int ) );

        readFluidConfig.read ( ( char * ) &xind,		        sizeof ( double ) );
        readFluidConfig.read ( ( char * ) &yind,		        sizeof ( double )  );
        readFluidConfig.read ( ( char * ) &zind,		        sizeof ( double )  );

        readFluidConfig.read ( ( char * ) &rand_gen_velx,	sizeof ( double ) );
        readFluidConfig.read ( ( char * ) &rand_gen_vely,	sizeof ( double ) );
        readFluidConfig.read ( ( char * ) &rand_gen_velz,	sizeof ( double ) );

        for( i=0 ; i < NPillars; ++i ){

            xDist = xind - pillarCenters[i][0];
            yDist = yind - pillarCenters[i][1];

            xDist -= boxEdge[x] * round( xDist / boxEdge[x] ); 
            yDist -= boxEdge[y] * round( yDist / boxEdge[y] ); 

            partOfPillar= ( pow( xDist, 2.) + pow( yDist, 2.) < pillarRad2 );

            if ( !partOfPillar )
                assign=true;
            else{
                assign=false;
                break;
            }
        }

        if ( assign ){
            particles.push_back({1.0,1.0,{xind, yind, zind},{0., 0., 0.},1});
            pCountFluid++;
        }
    }
}
else{
    std::cout << "Error opening file" << std::endl;
    exit(0);
}

readFluidConfig.close();

simProg << "Number of particles in the fluid surrounding pillars is " << pCountFluid << std::endl;
