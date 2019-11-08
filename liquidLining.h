///********************************* LIQUID IN CAPILLARY TUBE **************************/
cylCenterX		= boxEdge[x] / 2.;
cylCenterY		= boxEdge[y] / 2.;
pCount = 0;

bool untilInnerRadius;

double liquidLiningThickness=1.5;

double filmThickness1=2.5;
double filmThickness2=4.5;

xind_min = 0.01;
yind_min = 0.01;
zind_min = bufferLen-(filmThickness1+filmThickness2);

xind_max = boxEdge[x];
yind_max = boxEdge[y];
zind_max = bufferLen + capLen + 2*wallHeight + (filmThickness1 + filmThickness2);

double z_cylMin=bufferLen;
double z_cylMax=z_cylMin+2*wallHeight+capLen;

zind = zind_min;
aCube = pow( 1. / initRho, 1./3. );

simProg << "***************************************************" << std::endl;
simProg << "Started initialization of wetting liquid inside capillary tube" << std::endl;

simProg << "The starting x-coordinate of the capillary tube: "<< xind_min << std::endl;
simProg << "The starting y-coordinate of the capillary tube: "<< yind_min << std::endl;
simProg << "The starting z-coordinate of the capillary tube: "<< zind_min << std::endl;

simProg << "The ending x-coordinate of the capillary tube: "<< xind_max << std::endl;
simProg << "The ending y-coordinate of the capillary tube: "<< yind_max << std::endl;
simProg << "The ending z-coordinate of the capillary tube: "<< zind_max << std::endl;

while ( zind < zind_max ){
	xind = xind_min;
	while ( xind < xind_max){
		yind = yind_min;
		while( yind < yind_max){

			outerRadius = ( pow( xind - cylCenterX, 2.0 ) + pow( yind - cylCenterY, 2.0 ) <= pow(capRad, 2.0 ) );
			innerRadius = ( pow( xind - cylCenterX, 2.0 ) + pow( yind - cylCenterY, 2.0 ) >= pow(capRad - liquidLiningThickness, 2.0 ) );
			untilInnerRadius = ( pow( xind - cylCenterX, 2.0 ) + pow( yind - cylCenterY, 2.0 ) <= pow(capRad - liquidLiningThickness, 2.0 ) );

			if ( innerRadius && outerRadius && (zind >= z_cylMin) && (zind <= z_cylMax) ){

				// initializing particle radius, mass, position and velocity
				// if ( xind*xind + yind*yind + zind*zind <= radSqr )
				particles.push_back( { 1.0, 1.0, {xind, yind, zind}, {0., 0., 0.}, 1} );
				pCount++;

			}// inside cylinder
            else if ( innerRadius && (zind < z_cylMin) && (zind > z_cylMin-filmThickness1)){
				particles.push_back( { 1.0, 1.0, {xind, yind, zind}, {0., 0., 0.}, 1} );
				pCount++;
            }// below cylinder
            else if ( innerRadius && (zind > z_cylMax) && (zind < z_cylMax+filmThickness1) ){
				particles.push_back( { 1.0, 1.0, {xind, yind, zind}, {0., 0., 0.}, 1} );
				pCount++;
            }// above cylinder
            else if ( untilInnerRadius && (zind >= z_cylMin-filmThickness1) 
                                       && (zind <= z_cylMax+filmThickness1) ){

				// initializing particle radius, mass, position and velocity
				// if ( xind*xind + yind*yind + zind*zind <= radSqr )
				particles.push_back( { 1.0, 1.0, {xind, yind, zind}, {0., 0., 0.}, 5} );
				pCount++;

			}
            else if ( (zind <z_cylMin-filmThickness1) || ( zind > z_cylMax+filmThickness1)){
				particles.push_back( { 1.0, 1.0, {xind, yind, zind}, {0., 0., 0.}, 5} );
				pCount++;
            }

			yind += aCube * rcutoff;
		}// yind			
		xind += aCube * rcutoff;
	}
	zind += aCube * rcutoff;
}// zind

simProg << "finished initialization of  " << pCount << " particles inside capillary tube" << std::endl;
simProg << "***************************************************" << std::endl;
