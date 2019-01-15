// Reading positions of particles of individual structures
nPartPerStructure = 0;     // setting pCount to 0

sprintf(posfname, "readConfig/solids/WALL/A_40_B_25_k_12_5_Lx_%d_Ly_%d_H_2.xyz", int(boxEdge[x]), int(boxEdge[y]) );
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

    if ( type == 'O' )
        particles.push_back( {1.0 ,1.0 , { xind , yind , zind } ,{ rand_gen_velx, rand_gen_vely, rand_gen_velz }, 0} );
    // else if ( type == 'B' )
    //    particles.push_back( {1.0 ,1.0 , { xind , yind , zind } ,{ rand_gen_velx, rand_gen_vely, rand_gen_velz }, 3} );
}

pCount += nPartPerStructure;

// print confirmation
simProg << nPartPerStructure << " Particles within the piston were read " << std::endl;
simProg << " ********************************************************************** " << std::endl;

readConfig.close();
