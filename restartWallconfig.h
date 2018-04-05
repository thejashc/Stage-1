// read position and velocity data from the file posvelrestartfile.dat
std::ifstream readConfig; 
if ( ! readConfig ) { std::cout << "*** input file is empty" << std::endl; abort(); }
readConfig.open( "./restart/roughWall_boxEgdex_40_boxEdgey_10_boxEdgez_10.dat", std::ios::binary | std::ios::in );

readConfig.read ( ( char * ) &npart, sizeof( int ) );

for ( j = 1 ; j <= npart ; ++ j ){	

	readConfig.read ( ( char * ) &xind, sizeof ( double ) );
	readConfig.read ( ( char * ) &yind, sizeof ( double )  );
	readConfig.read ( ( char * ) &zind, sizeof ( double )  );
	readConfig.read ( ( char * ) &rand_gen_velx, sizeof ( double ) );
	readConfig.read ( ( char * ) &rand_gen_vely, sizeof ( double ) );
	readConfig.read ( ( char * ) &rand_gen_velz, sizeof ( double ) );

	if ( zind <= wallHeight || zind >= boxEdge[z] - wallHeight )
		particles.push_back( { 1.0 ,1.0 , {xind, yind, zind} ,{rand_gen_velx, rand_gen_vely, rand_gen_velz} } );
	// std::cout << xind << " " << yind << " " << zind << " " << rand_gen_velx << " " << rand_gen_vely << " " << rand_gen_velz << std::endl;
}

readConfig.close();
