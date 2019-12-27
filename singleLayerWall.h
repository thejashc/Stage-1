//***************************************************************************************
//******************************* BCC LATTICE ******************************************//
//***************************************************************************************
   nPartPerStructure = 0;
// Lower positional indices of particles
xind_min = 0.01;	// avoid boundary edge -- non-moving case
xind_max = boxEdge[x];

// Upper positional indices of particles
yind_min = 0.01;	// avoid boundary edge -- non-moving case
yind_max = boxEdge[y];

//yind_min = boxEdge[y]*0.5 - slabWidth*0.5;	// avoid boundary edge -- non-moving case
//yind_max = boxEdge[y]*0.5 + slabWidth*0.5;

aCube = pow( 1. / initRho, 1./2. );
xind = xind_min;
// Particle position in upper wall
while ( xind < xind_max){
yind = yind_min;
while( yind < yind_max){

particles.push_back( {1.0, 1.0, {xind, yind, boxEdge[z]*0.5}, {0., 0., 0.}, 1} );

// increment no. of particles
nPartPerStructure++;

yind += aCube*rcutoff;
}// end of yind			
xind += aCube*rcutoff;
}// end of xind

pCount += nPartPerStructure;

simProg << "finished initialization of  " << nPartPerStructure << " particles" << std::endl;
simProg << "***************************************************" << std::endl;

//***************************************************************************************
//******************************* FCC LATTICE ******************************************//
//***************************************************************************************
/*
   pCount=0;

   double slabWidth=20.;
   double ymin=boxEdge[y]*0.5-slabWidth*0.5;
   double xOffset=0.0001;

   unsigned int nx=34.;
   unsigned int ny=34.;
   unsigned int nPerLattice=2;

   double b=sqrt(1./3.); // lattice spacing

   double base[2][2];
   double iterator[2];

   double xindP1, yindP1;
   double xindP2, yindP2;

   base[0][0]=0.;          // (x, y)-coord of particle 0 from template
   base[0][1]=0.;

   base[1][0]=(b/2.);      // (x, y)-coord of particle 1 from template
   base[1][1]=(b/2.);

   for (i=0;i<nx;++i){
   for(j=0;j<ny;++j){

   iterator[0]=b*i;
   iterator[1]=b*j;

   for(k=0;k<nPerLattice;++k){

   xindP1=iterator[0] + base[k][0];
   yindP1=iterator[1] + base[k][1];

//simProg << xindP1 << ", " << yindP1 << ", " << xindP2 << ", " << yindP2 << std::endl;

particles.push_back( {1.0, 1.0, {xindP1+xOffset, yindP1+ymin, boxEdge[z]*0.5}, {0., 0., 0.}, 1} );

pCount=pCount+1;
}
}
}

simProg << "finished initialization of  " << pCount << " particles" << std::endl;
simProg << "***************************************************" << std::endl;
*/

/*
   bvec = [0 0 0; b/2 b/2 0; b/2 0 b/2; 0 b/2 b/2];
   ip = 0;

   % Generate positions
   for ix = 0:nx-1
   for iy = 0:ny-1
   for iz = 0:nz-1
   r0 = b*[ix iy iz];  % Unit cell base position
   for k = 1:nPerLattice
   ip = ip + 1;    % Add particle
   r(ip,:) = r0 + bvec(k,:);
   end
   end
   end
   end
   */

//***************************************************************************************
//******************************* READ FROM FILE ******************************************//
//***************************************************************************************
/*
pCount=0;
char fname[100];
unsigned int particleType;

sprintf(fname,"./inputGeometry/test.bin");
//sprintf(fname,"/storage/thejas/Year3/porousStructure/2D_MDPD_simulations/thinFilm/set4/data/posVel100000.bin");
simProg << " Reading the equilibriated fluid particles from a file" << std::endl;

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

        particles.push_back({1.0,1.0,{xind, yind - boxEdge[y]*0.5, zind},{0., 0., 0.},1});
        //particles.push_back({1.0,1.0,{xind, yind - boxEdge[y]*0.5, zind},{rand_gen_velx, rand_gen_vely, rand_gen_velz},1});
        pCount++;

    }

}
else{
    std::cout << "Error opening file" << std::endl;
    exit(0);
}


readConfig.close();

simProg << "finished initialization of  " << pCount << " particles" << std::endl;
simProg << "***************************************************" << std::endl;
*/
