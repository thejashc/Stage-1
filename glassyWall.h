// read file
int particleType;
char fname[100];
unsigned int nTrials=6;
unsigned int npart;

double zOffset=15.;

for ( int trial=1;trial<=nTrials; trial++){

    simProg << "Reading slab from trial " << trial << std::endl;

    //sprintf(fname,"./inputGeometry/glassyWall/slabsFromTrials/Lx_45_Ly_40_Lz_15/posVel_trial%d.bin",trial);
    sprintf(fname,"./inputGeometry/solid.bin",trial);

    std::ifstream readConfig(fname, std::ios::binary | std::ios::in ); 
    if ( ! readConfig ) { simProg << "*** The file could not be opened/ does not exist *** \n Aborting !! " << std::endl; abort(); }

    readConfig.read ( ( char * ) &npart, sizeof (unsigned int) );
    simProg << npart << " particle positions and velocities being read" << std::endl;

    double offset = zOffset*(trial-1);

    for ( j = 0 ; j < npart ; ++ j ){	

        readConfig.read ( ( char * ) &particleType,		sizeof (unsigned int ) );

        readConfig.read ( ( char * ) &xind,		        sizeof ( double ) );
        readConfig.read ( ( char * ) &yind,		        sizeof ( double )  );
        readConfig.read ( ( char * ) &zind,		        sizeof ( double )  );

        readConfig.read ( ( char * ) &rand_gen_velx,	sizeof ( double ) );
        readConfig.read ( ( char * ) &rand_gen_vely,	sizeof ( double ) );
        readConfig.read ( ( char * ) &rand_gen_velz,	sizeof ( double ) );
        
        /*
        std::cout << type << "\t"   << xind << "\t" << std::setprecision(15)  
                                    << yind << "\t"  << std::setprecision(15)     
                                    << zind << "\t" << std::setprecision(15)   
                                    << rand_gen_velx << "\t" << std::setprecision(15)  
                                    << rand_gen_vely << "\t" << std::setprecision(15)  
                                    << rand_gen_velz << "\n";
        exit(0);
        */

        particles.push_back( {1.0 ,1.0 , {xind, yind, zind+offset} ,{rand_gen_velx, rand_gen_vely, rand_gen_velz}, particleType} );
    }
    readConfig.close();
}
