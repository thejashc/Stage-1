xind_min = 0.01;
yind_min = 0.01;
zind_min = resCOMZ - resWdth * 0.5;
xind_max = boxEdge[x];
yind_max = boxEdge[y];
zind_max = resCOMZ + resWdth * 0.5; 

double resComVel = 0.;

simProg << "***************************************************" << std::endl;
simProg << "Started initialization of the wetting liquid above the reservoir" << std::endl;

simProg << "The minimum x-coord of wall is: "<< xind_min << std::endl;
simProg << "The minimum y-coord of wall is: "<< yind_min << std::endl;
simProg << "The minimum z-coord of wall is: "<< zind_min << std::endl;

simProg << "The maximum x-coord of wall is: "<< xind_max << std::endl;
simProg << "The maximum y-coord of wall is: "<< yind_max << std::endl;
simProg << "The maximum z-coord of wall is: "<< zind_max << std::endl;

xind = xind_min;
// Particle position intialization in a crystal structure 
while ( xind < xind_max){
    yind = yind_min;
    while( yind < yind_max){
        zind = zind_min;
        while( zind < zind_max){

                particles.push_back({1.0,1.0,{xind, yind, zind + 20. },{0., 0., resComVel},1});
                pCount++;

            // update zind
            zind += aCube * rcutoff;

        } // end of zind
        yind += aCube * rcutoff;
    } // end of yind			
    xind += aCube * rcutoff;
} // end of xind

simProg << "finished initialization of  " << pCount << " particles of the wetting liquid" << std::endl;
simProg << "***************************************************" << std::endl;
