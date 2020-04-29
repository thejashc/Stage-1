unsigned int flag;

double lim1=0.5*(capLen - ligamentLen);
double lim2=0.5*(capLen + ligamentLen);

bool even=true;
unsigned int count=0;

unsigned int ratio=int(100./prcntCompose);

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

    innerRadius = ( pow( scaledX, 2.0 ) + pow( scaledY, 2.0 ) <= ri2 );

    // cylinder
    //if ( innerRadius && (zind < capLen) ){
    if ( innerRadius && (zind > lim1) && (zind < lim2)){
        
        if( (count % ratio) == 0 ){
            particles.push_back({1.0,1.0,{scaledX+cylCenterX, scaledY+cylCenterY, zind},{0., 0., 0.},1});
            even=false;
        }
        else{
            particles.push_back({1.0,1.0,{scaledX+cylCenterX, scaledY+cylCenterY, zind},{0., 0., 0.},2});
            even=true;
        }

        count++;
    }
}


readConfig.close();
