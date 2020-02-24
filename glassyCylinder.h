// read file
int particleType;
char fname[200];
unsigned int nTrials=4;
unsigned int npart;

unsigned int pCountCyl=0;
unsigned int pCountWall=0;

double cylCenterX = 0.5*boxEdge[x];
double cylCenterY = 0.5*boxEdge[y];

double origCx=0.5*origLx;
double origCy=0.5*origLy;

double thickness=2.;
double ri=capRad;
double ro=ri+thickness;
double rm=ro-0.5;

double ri2=pow(ri,2.);
double ro2=pow(ro,2.);
double rm2=pow(rm,2.);

double scaledX;
double scaledY;

double wallHeight =2.;
double cylVol = 3.14159 * (ro2 - ri2) * capLen;
double wallVol = (boxEdge[x]*boxEdge[y] - 3.14159 * ri2)*wallHeight;

std::ifstream readConfig(readSolidFrom, std::ios::binary | std::ios::in ); 

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

    scaledX = xind - origCx;
    scaledY = yind - origCy;

    innerRadius     = ( pow( scaledX, 2.0 ) + pow( scaledY, 2.0 ) >= ri2 );
    middleInRadius  = ( pow( scaledX, 2.0 ) + pow( scaledY, 2.0 ) <= rm2 );
    middleOutRadius = ( pow( scaledX, 2.0 ) + pow( scaledY, 2.0 ) > rm2 );
    outerRadius     = ( pow( scaledX, 2.0 ) + pow( scaledY, 2.0 ) <= ro2 );

    // cylinder
    if ( innerRadius && middleInRadius && (zind < capLen) ){
        particles.push_back({1.0,1.0,{scaledX+cylCenterX, scaledY+cylCenterY, zind+bufferLen},{0., 0., 0.},0});
        pCount++;
        pCountCyl++;
    }
    else if (middleOutRadius && outerRadius && (zind < capLen) ){
        particles.push_back({1.0,1.0,{scaledX+cylCenterY, scaledY+cylCenterY, zind+bufferLen},{0., 0., 0.},3});
        pCount++;
        pCountCyl++;
    }
    //wall
    else if ( innerRadius &&
            (scaledX > -0.5*boxEdge[x]) && (scaledX < 0.5*boxEdge[x]) &&
            (scaledY > -0.5*boxEdge[y]) && (scaledY < 0.5*boxEdge[y]) &&            
            (zind>capLen+0.5) && (zind<capLen+wallHeight) ){
        particles.push_back({1.0,1.0,{scaledX+cylCenterY, scaledY+cylCenterY, zind+bufferLen},{0., 0., 0.},0});
        pCount++;
        pCountWall++;
    }
    else if ( innerRadius && middleInRadius && (zind>capLen) && (zind<capLen+0.5) ){
        particles.push_back({1.0,1.0,{scaledX+cylCenterY, scaledY+cylCenterY, zind+bufferLen},{0., 0., 0.},0});
        pCount++;
        pCountWall++;
    }
    else if ( middleOutRadius && 
            (scaledX > -0.5*boxEdge[x]) && (scaledX < 0.5*boxEdge[x]) &&
            (scaledY > -0.5*boxEdge[y]) && (scaledY < 0.5*boxEdge[y]) &&  
            (zind>capLen) && (zind<capLen+0.5) ){
        particles.push_back({1.0,1.0,{scaledX+cylCenterY, scaledY+cylCenterY, zind+bufferLen},{0., 0., 0.},3});
        pCount++;
        pCountWall++;
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

simProg << "Number of particles in cylinder with inner radius = " << ri << ", outer radius = " << ro << " and length = " << capLen << " is " << pCountCyl << "\n";
simProg << "Number density of particles in cylinder is =" << pCountCyl/cylVol << "\n\n"; 

simProg << "Number of particles in wall is " << pCountWall << "\n";
simProg << "Number density of particles in wall is =" << pCountWall/wallVol << "\n\n"; 

readConfig.close();
