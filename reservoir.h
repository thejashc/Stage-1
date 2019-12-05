/*
   xind_min = 0.01;
   yind_min = 0.01;
   zind_min = resCOMZ - resWdth * 0.5;

   xind_max = boxEdge[x];
   yind_max = boxEdge[y];
   zind_max = resCOMZ + resWdth * 0.5; 
//double zind_max1 = resCOMZ; 
//double zind_max2 = resCOMZ + resWdth * 0.5; 

pCount = 0;

bool flagA = 1;

simProg << "***************************************************" << std::endl;
simProg << "Started initialization of the wetting liquid above the reservoir" << std::endl;

simProg << "The minimum x-coord of reservoir is: "<< xind_min << std::endl;
simProg << "The minimum y-coord of reservoir is: "<< yind_min << std::endl;
simProg << "The minimum z-coord of reservoir is: "<< zind_min << std::endl;

simProg << "The maximum x-coord of reservoir is: "<< xind_max << std::endl;
simProg << "The maximum y-coord of reservoir is: "<< yind_max << std::endl;
simProg << "The maximum z-coord of reservoir is: "<< zind_max << std::endl;

aCube 		= pow( 1. / initRho, 1./3. );

xind = xind_min;
// Particle position intialization in a crystal structure 
while ( xind < xind_max){
yind = yind_min;
while( yind < yind_max){
zind = zind_min;
//while( zind < zind_max1){
while( zind < zind_max){

particles.push_back({1.0,1.0,{xind, yind, zind},{0., 0., resCOMVel},2});
pCount++;

// update zind
zind += 2.0 * aCube * rcutoff;
//zind += 1.0 * aCube * rcutoff;

} // end of zind
yind += aCube * rcutoff;
} // end of yind			
xind += aCube * rcutoff;
} // end of xind

xind = xind_min;
// Particle position intialization in a crystal structure 
while ( xind < xind_max){
yind = yind_min;
while( yind < yind_max){
zind = zind_min;
//while( zind < zind_max2){
while( zind < zind_max){

particles.push_back({1.0,1.0,{xind, yind, zind},{0., 0., resCOMVel},1});
pCount++;

// update zind
//zind += 2.0 * aCube * rcutoff;
zind += 1.0 * aCube * rcutoff;

} // end of zind
yind += aCube * rcutoff;
} // end of yind			
xind += aCube * rcutoff;
} // end of xind

simProg << "finished initialization of  " << pCount << " particles of the wetting liquid" << std::endl;
simProg << "***************************************************" << std::endl;
*/
// read file
//sprintf(fname,"/storage/thejas/src/inputGeometry/fccLattice/fcc_Fluid_Lattice/fcc_fluid_lattice_50pc_wetting.dat");
sprintf(fname,"./inputGeometry/fccLattice/fcc_Fluid_Lattice/fcc_fluid_lattice_50pc_wetting.dat");
//sprintf(fname,"./inputGeometry/fccLattice/fcc_Fluid_Lattice/fcc_fluid_lattice.dat");
//sprintf(fname,"../inputGeometry/fcc_fluid_lattice_40pc_wetting.dat");
readConfig.open(fname, std::ios::in ); 
if ( ! readConfig ) { simProg << "*** The file could not be opened/ does not exist *** \n Aborting !! " << std::endl; abort(); }

readConfig >> npart;
simProg << npart << " particle positions and velocities for the fluid reservoir being read" << std::endl;

pCount =0;
char dummy;
double offset = 0.000001;
double zoffSet = resCOMZ;

for ( j = 0 ; j < npart ; ++ j ){	

    readConfig >> dummy;

    readConfig >> xind;
    readConfig >> yind;
    readConfig >> zind;

    if( (xind < boxEdge[x]) && (yind < boxEdge[y]) && (zind<boxEdge[z]) ){

        if ( dummy == 'H' ){
            particles.push_back({1.0,1.0,{xind+offset, yind+offset, zind+zoffSet},{0., 0., resCOMVel},1});
            pCount++;
        }
        else if ( dummy == 'C' ){
            particles.push_back({1.0,1.0,{xind+offset, yind+offset, zind+zoffSet},{0., 0., resCOMVel},2});
            pCount++;
        }
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
simProg << pCount << " fluid particles indexed " << std::endl;
readConfig.close();
