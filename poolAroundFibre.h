double xOffset=2.5;
//  Set max and min dimensions of boxy
xind_min = 0.01;
yind_min = 0.01;
zind_min = 0.01;
xind_max = boxEdge[x];
yind_max = boxEdge[y];
zind_max = boxEdge[z];

simProg << "***************************************************" << std::endl;
simProg << "Started initialization of reservoir around fibre" << std::endl;

simProg << "The minimum x-coord is: "<< xind_min << std::endl;
simProg << "The minimum y-coord is: "<< yind_min << std::endl;
simProg << "The minimum z-coord is: "<< zind_min << std::endl;

simProg << "The maximum x-coord is: "<< xind_max << std::endl;
simProg << "The maximum y-coord is: "<< yind_max << std::endl;
simProg << "The maximum z-coord is: "<< zind_max << std::endl;

double aOuter=11.;   // minor axis of ellipse = fibRad - 2*rcutoff
double bOuter=3.;   // major axis of ellipse = fibRad + 2*rcutoff

xind = xind_min;
while ( xind < xind_max){
    yind = yind_min;
    while( yind < yind_max){
        zind = zind_min;
        while( zind < zind_max){

            bool outsideEllipse=( pow( (yind-cy)/aOuter,2.) + pow( (zind-cz)/bOuter,2.) >= 1. );
            bool xLims=((xind>0.01)&(xind<fibLen+xOffset));
            bool zLims=((zind>cz-fibRad)&(zind<cz+fibRad));

            if ( outsideEllipse & xLims & zLims ){
                particles.push_back({1.0,1.0,{xind, yind, zind},{0., 0., 0.},1});
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
