double slitWidth = 5.;
double slitHeight = 10.;

// defining the limits in the x direction
double xMin1 = 0.5 * boxEdge[x] - 0.5 * slitWidth - capWallWdth;        // biggest rectangle -- Region 1
double xMax1 = 0.5 * boxEdge[x] + 0.5 * slitWidth + capWallWdth;

double xMin2 = 0.5 * boxEdge[x] - 0.5 * ( slitWidth + capWallWdth );    // middle rectangle -- Region 2
double xMax2 = 0.5 * boxEdge[x] + 0.5 * ( slitWidth + capWallWdth );

double xMin3 = 0.5 * boxEdge[x] - 0.5 * slitWidth;                      // smallest rectangle -- Region 3
double xMax3 = 0.5 * boxEdge[x] + 0.5 * slitWidth;

// defining the inner and the outermost boundaries for the x-coordinate of the  rectangle
sqInnerEdgeXmin = xMin3;
sqInnerEdgeXmax = xMax3;

sqXmin = xMin1;
sqXmax = xMax1;

// defining the limits in the y direction
double yMin1 = 0.5 * boxEdge[y] - 0.5 * slitHeight - capWallWdth;        // biggest rectangle -- Region 1
double yMax1 = 0.5 * boxEdge[y] + 0.5 * slitHeight + capWallWdth;

double yMin2 = 0.5 * boxEdge[y] - 0.5 * ( slitHeight + capWallWdth );    // middle rectangle -- Region 2
double yMax2 = 0.5 * boxEdge[y] + 0.5 * ( slitHeight + capWallWdth );

double yMin3 = 0.5 * boxEdge[y] - 0.5 * slitHeight;                     // smallest rectangle -- Region 3
double yMax3 = 0.5 * boxEdge[y] + 0.5 * slitHeight;

// defining the inner and the outermost boundaries for the x-coordinate of the  rectangle
sqInnerEdgeYmin = yMin3;
sqInnerEdgeYmax = yMax3;

sqYmin = yMin1;
sqYmax = yMax1;

// logical variables
bool region1, region2, region3;

// other variables
double delY, delX;
double xindReflect, yindReflect;
double offset = 0.2;

/*
   std::cout << xMin1 << std::endl;
   std::cout << xMax1 << std::endl;

   std::cout << xMin2 << std::endl;
   std::cout << xMax2 << std::endl;

   std::cout << xMin3 << std::endl;
   std::cout << xMax3 << std::endl;
   */

bckgIdxStart = 0;
pCount 		= 0;

// initializing the min and max of co-ordinates
xind_min 	= 0.5 * boxEdge[x] + offset;
yind_min 	= 0.5 * boxEdge[y] + offset;
zind_min 	= bufferLen;

xind_max	= boxEdge[x];
yind_max	= boxEdge[y];
zind_max 	= bufferLen + capLen;

zind 		= zind_min;
aCube 		= pow( 1. / initWallRho, 1./3. );

simProg << "***************************************************" << std::endl;
simProg << "Started initialization of the square capillary tube" << std::endl;

simProg << "The starting x-coordinate of the square capillary tube: "<< xind_min << std::endl;
simProg << "The starting y-coordinate of the square capillary tube: "<< yind_min << std::endl;
simProg << "The starting z-coordinate of the square capillary tube: "<< zind_min << std::endl;

simProg << "The ending x-coordinate of the square capillary tube: "<< xind_max << std::endl;
simProg << "The ending y-coordinate of the square capillary tube: "<< yind_max << std::endl;
simProg << "The ending z-coordinate of the square capillary tube: "<< zind_max << std::endl;

// defining the beginning and end of the capillary tube 
capTubeStart = bufferLen; 
capTubeEnd = bufferLen + capLen;

while ( zind < zind_max ){
    xind = xind_min;
    while( xind < xind_max ){
        yind = yind_min;
        while( yind < yind_max ){

            region1 = ( xind >= xMin1 ) && ( xind <= xMax1 ) && ( yind >= yMin1 ) && ( yind <= yMax1 );
            region2 = ( xind >= xMin2 ) && ( xind <= xMax2 ) && ( yind >= yMin2 ) && ( yind <= yMax2 );
            region3 = ( xind >= xMin3 ) && ( xind <= xMax3 ) && ( yind >= yMin3 ) && ( yind <= yMax3 );

            if ( region1 && !(region2) ){
                particles.push_back( {1.0, 1.0, {xind, yind, zind}, {0., 0., 0.}, 3} );
                pCount++;

                if ( pCount != 0 ){

                    delX = xind - 0.5 * boxEdge[x];
                    delY = yind - 0.5 * boxEdge[y];

                    xindReflect = 0.5 * boxEdge[x] - delX;
                    yindReflect = 0.5 * boxEdge[y] - delY;

                    particles.push_back( {1.0, 1.0, {xind, yindReflect, zind}, {0., 0., 0.}, 3} );
                    pCount++;

                    particles.push_back( {1.0, 1.0, {xindReflect, yind, zind}, {0., 0., 0.}, 3} );
                    pCount++;

                    particles.push_back( {1.0, 1.0, {xindReflect, yindReflect, zind}, {0., 0., 0.}, 3} );
                    pCount++;
                }
            } // should entirely be type 3 -- if not background potential
            else if ( region2 && !region3 ){
                particles.push_back( { 1.0, 1.0, {xind, yind, zind}, {0., 0., 0.}, 0} );
                pCount++;

                if ( pCount != 0 ){

                    delX = xind - 0.5 * boxEdge[x];
                    delY = yind - 0.5 * boxEdge[y];

                    xindReflect = 0.5 * boxEdge[x] - delX;
                    yindReflect = 0.5 * boxEdge[y] - delY;

                    particles.push_back( {1.0, 1.0, {xind, yindReflect, zind}, {0., 0., 0.}, 0} );
                    pCount++;

                    particles.push_back( {1.0, 1.0, {xindReflect, yind, zind}, {0., 0., 0.}, 0} );
                    pCount++;

                    particles.push_back( {1.0, 1.0, {xindReflect, yindReflect, zind}, {0., 0., 0.}, 0} );
                    pCount++;
                }
            }
            yind += aCube * rcutoff;
        }// yind			
        xind += aCube * rcutoff;
    }
    zind += aCube * rcutoff;
}// zind

bckgIdxEnd += pCount;

simProg << "finished initialization of  " << pCount << " particles inside square capillary tube" << std::endl;
simProg << "***************************************************" << std::endl;

double entranceWallWdth = 2.0;
// Set max and min dimensions of boxy
xind_min = 0.5 * boxEdge[x] + offset;
yind_min = 0.5 * boxEdge[y] + offset;
zind_min = bufferLen + capLen;

xind_max = boxEdge[x];
yind_max = boxEdge[y];
zind_max = zind_min + entranceWallWdth;

// defining wallLowPos
wallLowPos = capTubeEnd + entranceWallWdth;

pCount	 = 0;

simProg << "***************************************************" << std::endl;
simProg << "Started initialization of the wall adjacent to reservoir" << std::endl;

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

            region1 = ( xind >= xMin1 ) && ( xind <= xMax1 ) && ( yind >= yMin1 ) && ( yind <= yMax1 );
            region2 = ( xind >= xMin2 ) && ( xind <= xMax2 ) && ( yind >= yMin2 ) && ( yind <= yMax2 );
            region3 = ( xind >= xMin3 ) && ( xind <= xMax3 ) && ( yind >= yMin3 ) && ( yind <= yMax3 );

            delX = xind - 0.5 * boxEdge[x];
            delY = yind - 0.5 * boxEdge[y];

            xindReflect = 0.5 * boxEdge[x] - delX;
            yindReflect = 0.5 * boxEdge[y] - delY;

            if ( zind < zind_min + 0.5 * entranceWallWdth ){

                if ( !region1 ){
                    particles.push_back({1.0,1.0,{xind, yind, zind},{0., 0., 0.},3});
                    pCount++;

                    particles.push_back( {1.0, 1.0, {xind, yindReflect, zind}, {0., 0., 0.}, 3} );
                    pCount++;

                    particles.push_back( {1.0, 1.0, {xindReflect, yind, zind}, {0., 0., 0.}, 3} );
                    pCount++;

                    particles.push_back( {1.0, 1.0, {xindReflect, yindReflect, zind}, {0., 0., 0.}, 3} );
                    pCount++;
                } // should entirely be typ 3 -- if not background potential
                else if ( region1 && !region2 ){
                    particles.push_back({1.0,1.0,{xind, yind, zind},{0., 0., 0.},3});
                    pCount++;

                    particles.push_back( {1.0, 1.0, {xind, yindReflect, zind}, {0., 0., 0.}, 3} );
                    pCount++;

                    particles.push_back( {1.0, 1.0, {xindReflect, yind, zind}, {0., 0., 0.}, 3} );
                    pCount++;

                    particles.push_back( {1.0, 1.0, {xindReflect, yindReflect, zind}, {0., 0., 0.}, 3} );
                    pCount++;

                }// should entirely be typ 3 -- if not background potential
                else if ( region2 && !region3 ){
                    particles.push_back({1.0,1.0,{xind, yind, zind},{0., 0., 0.},0});
                    pCount++;

                    particles.push_back( {1.0, 1.0, {xind, yindReflect, zind}, {0., 0., 0.}, 0} );
                    pCount++;

                    particles.push_back( {1.0, 1.0, {xindReflect, yind, zind}, {0., 0., 0.}, 0} );
                    pCount++;

                    particles.push_back( {1.0, 1.0, {xindReflect, yindReflect, zind}, {0., 0., 0.}, 0} );
                    pCount++;
                }
            }
            else if ( zind >= zind_min + 0.5 * entranceWallWdth ){
                if ( !region3 ){
                    particles.push_back({1.0,1.0,{xind, yind, zind},{0., 0., 0.},0});
                    pCount++;

                    particles.push_back( {1.0, 1.0, {xind, yindReflect, zind}, {0., 0., 0.}, 0} );
                    pCount++;

                    particles.push_back( {1.0, 1.0, {xindReflect, yind, zind}, {0., 0., 0.}, 0} );
                    pCount++;

                    particles.push_back( {1.0, 1.0, {xindReflect, yindReflect, zind}, {0., 0., 0.}, 0} );
                    pCount++;
                }
            }

            // update zind
            zind += aCube * rcutoff;

        } // end of zind
        yind += aCube * rcutoff;
    } // end of yind			
    xind += aCube * rcutoff;
} // end of xind

bckgIdxEnd += pCount - 1;

simProg << "finished initialization of  " << pCount << " particles inside wall adj. capillary" << std::endl;
simProg << "***************************************************" << std::endl;
