double cy=boxEdge[y]*0.5;
double cz=boxEdge[z]*0.5;
double fibRad=5.0;
double fibThick=1.5;
double bufferLen=25.;
double fibLen=50.;

bool READ_FIBRE=1;
bool CONSTRUCT_FIBRE=0;

if ( CONSTRUCT_FIBRE ){
    ngbrIdxStart=0;

    //  Set max and min dimensions of boxy
    xind_min = bufferLen;
    yind_min = 0.01;
    zind_min = 0.01;
    xind_max = bufferLen+fibLen;
    yind_max = boxEdge[y];
    zind_max = boxEdge[z];

    simProg << "***************************************************" << std::endl;
    simProg << "Started initialization of a single paper fibre" << std::endl;

    simProg << "The minimum x-coord is: "<< xind_min << std::endl;
    simProg << "The minimum y-coord is: "<< yind_min << std::endl;
    simProg << "The minimum z-coord is: "<< zind_min << std::endl;

    simProg << "The maximum x-coord is: "<< xind_max << std::endl;
    simProg << "The maximum y-coord is: "<< yind_max << std::endl;
    simProg << "The maximum z-coord is: "<< zind_max << std::endl;

    xind = xind_min;
    while ( xind < xind_max){
        yind = yind_min;
        while( yind < yind_max){
            zind = zind_min;
            while( zind < zind_max){

                bool outerRadius = (pow(yind - cy,2.0) + pow(zind - cz,2.0) <= pow(fibRad, 2.0));          // all particles within the outermost radii
                bool innerRadius = (pow(yind - cy,2.0) + pow(zind - cz,2.0) >= pow(fibRad-fibThick, 2.0));          // all particles within the outermost radii

                if ( xind >= xind_min && xind <= xind_max && outerRadius && innerRadius ){
                    particles.push_back({1.0,1.0,{xind, yind, zind},{0., 0., 0.},0});
                    pCount++;}

                // update zind
                zind += aCube * rcutoff;
            } // end of zind
            yind += aCube * rcutoff;
        } // end of yind			
        xind += aCube * rcutoff;
    } // end of xind

    simProg << "finished initialization of  " << pCount << " particles inside wall adj. capillary" << std::endl;
    simProg << "***************************************************" << std::endl;

    ngbrIdxEnd=pCount-1;
}
else if ( READ_FIBRE ){
    ngbrIdxStart=0;
    pCount=0;

    unsigned int particleType;
    double xOffset=2.5*rcutoff;

    simProg << "Reading a squashed fibre" << std::endl;

    std::ifstream readConfig( "readConfig/solids/COMPRESSED_FIBRE/singleFibre_fibLen_25_fibRad_5_fibThick_1_5.bin", std::ios::binary | std::ios::in ); 
    if ( ! readConfig ) { simProg << "*** The compressed fibre file could not be opened/ does not exist *** \n Aborting !! " << std::endl; abort(); }

    readConfig.read ( ( char * ) &npart,		        sizeof( unsigned int ) );
    for ( j = 0 ; j < npart ; ++ j ){	

        readConfig.read ( ( char * ) &particleType,		sizeof ( particleType ) );

        readConfig.read ( ( char * ) &xind,		        sizeof ( double ) );
        readConfig.read ( ( char * ) &yind,		        sizeof ( double )  );
        readConfig.read ( ( char * ) &zind,		        sizeof ( double )  );

        readConfig.read ( ( char * ) &rand_gen_velx,	sizeof ( double ) );
        readConfig.read ( ( char * ) &rand_gen_vely,	sizeof ( double ) );
        readConfig.read ( ( char * ) &rand_gen_velz,	sizeof ( double ) );

        //std::cout << xind << "," << yind << ", " << zind << ", " << rand_gen_velx << ", " << rand_gen_vely << ", "<< rand_gen_velz << std::endl;

        if ( particleType == 0 )
            particles.push_back( {1.0 ,1.0 , {xind-bufferLen+rcutoff, yind , zind} ,{0., 0., 0.}, 0} );

        pCount++;
    }
    readConfig.close();
    ngbrIdxEnd=pCount-1;

    simProg << "finished initialization of  " << pCount << " particles inside squashed fibre" << std::endl;
    simProg << "***************************************************" << std::endl;
}
