// Reading positions of particles of individual structures
nPartPerStructure = 0;     // setting pCount to 0

sprintf(posfname, "readConfig/fluids/PLANAR_SLAB/RESERVOIR/A_40_B_25_Lx_%d_Ly_%d_H_%d.xyz", int(boxEdge[x]), int(boxEdge[y]), int(resWdth) );
readConfig.open(posfname, std::ifstream::in);

if ( ! readConfig ) { simProg << "*** The configuration file " << posfname << " could not be opened/ does not exist *** \n Aborting !! " << std::endl; abort(); }

readConfig >> nPartPerStructure;        // count the total number of particles within the structure
readConfig >> dummyString;
readConfig >> dummyString;
readConfig >> dummyString;
readConfig >> dummyString;

for ( j = 1 ; j <= nPartPerStructure ; ++ j ){	

    rand_gen_velx = 0.;
    rand_gen_vely = 0.;
    rand_gen_velz = 0.;

    readConfig >> type;
    readConfig >> xind;
    readConfig >> yind;
    readConfig >> zind;

    if ( type == 'H' )
        particles.push_back( {1.0 ,1.0 , { xind , yind , zind + resCOMZ } ,{ rand_gen_velx, rand_gen_vely, rand_gen_velz }, 1} );
    else if ( type == 'C' )
        particles.push_back( {1.0 ,1.0 , { xind , yind , zind + resCOMZ } ,{ rand_gen_velx, rand_gen_vely, rand_gen_velz }, 2} );
}

// print confirmation
simProg << nPartPerStructure << " Particles within the planar slab of fluid were read " << std::endl;
simProg << " ********************************************************************** " << std::endl;

readConfig.close();
