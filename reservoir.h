xind_min = 0.01;
yind_min = 0.01;
zind_min = resCOMZ - resWdth * 0.5;

xind_max = boxEdge[x];
yind_max = boxEdge[y];
//zind_max = resCOMZ + resWdth * 0.5; 
double zind_max1 = resCOMZ; 
double zind_max2 = resCOMZ + resWdth * 0.5; 

pCount = 0;

bool flagA = 1;

simProg << "***************************************************" << std::endl;
simProg << "Started initialization of the wetting liquid above the reservoir" << std::endl;

simProg << "The minimum x-coord of wall is: "<< xind_min << std::endl;
simProg << "The minimum y-coord of wall is: "<< yind_min << std::endl;
simProg << "The minimum z-coord of wall is: "<< zind_min << std::endl;

simProg << "The maximum x-coord of wall is: "<< xind_max << std::endl;
simProg << "The maximum y-coord of wall is: "<< yind_max << std::endl;
simProg << "The maximum z-coord of wall is: "<< zind_max << std::endl;

aCube 		= pow( 1. / initRho, 1./3. );

xind = xind_min;
// Particle position intialization in a crystal structure 
while ( xind < xind_max){
    yind = yind_min;
    while( yind < yind_max){
        zind = zind_min;
        while( zind < zind_max1){

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

xind = xind_min;
// Particle position intialization in a crystal structure 
while ( xind < xind_max){
    yind = yind_min;
    while( yind < yind_max){
        zind = resCOMZ;
        while( zind < zind_max2){

            particles.push_back({1.0,1.0,{xind, yind, zind},{0., 0., resCOMVel},2});
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

double ratio_HtoC = 0.5;
unsigned int typeHParticles = ratio_HtoC * pCount;

simProg << typeHParticles  << " of the total of  " <<  pCount << " fluid particles are converted to type H" <<  std::endl;
