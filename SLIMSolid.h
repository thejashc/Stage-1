// Initializing the square array
// layer 1
//
//
simProg << "***************************************************" << std::endl;
simProg << "Started initialization of square-pillar arrays " << std::endl;

double hcpEdge   = 5.0 * rcutoff;
double hcpHeight = 0.5 * sqrt(3) * hcpEdge;
double xind_cylX = 0.5 * hcpEdge;
double zind_cylZ_L1 = 0.5 * hcpEdge + bufferLen + pistonWidth + 3. + nonWettingLiquidLength;
double yind_cylY = 0.5 * hcpEdge;
double zind_cylZ_L2 = 0.5 * hcpEdge + hcpHeight + bufferLen + pistonWidth + 3. + nonWettingLiquidLength;
double squareEdge = 3.;
double rotationTheta = M_PI * 0.25;

pCount = 0;
while ( xind_cylX <= boxEdge[x] ){
    zind_cylZ_L1 = 0.5 * hcpEdge + bufferLen + pistonWidth + 3. + nonWettingLiquidLength;
    while ( zind_cylZ_L1 <= boxEdge[z] ){

        p1.X = xind_cylX;  p1.Z = zind_cylZ_L1;

        // std::cout << " Input p1 = " << p1 << ", Input p2 = " << p2 << std::endl;

        createSquareArray ( p1.X, p1.Z, squareEdge, rotationTheta);
        zind_cylZ_L1 += 2. * hcpHeight;

    }
    xind_cylX   += hcpEdge;
}

// Parallel Fiber
xind_cylX = hcpEdge;
zind_cylZ_L2 =0.5 * hcpEdge + bufferLen + pistonWidth + 3. + nonWettingLiquidLength;

while ( xind_cylX <= boxEdge[x] ){
    zind_cylZ_L2 =0.5 * hcpEdge + hcpHeight + bufferLen + pistonWidth + 3. + nonWettingLiquidLength;
    while ( zind_cylZ_L2 <= boxEdge[z] ){

        p2.X = xind_cylX;  p2.Z = zind_cylZ_L2;

        createSquareArray ( p2.X, p2.Z, squareEdge, rotationTheta);
        zind_cylZ_L2 += 2. * hcpHeight;

    }
    xind_cylX += hcpEdge;
}

simProg << "finished initialization of  " << pCount << " particles inside square pillar" << std::endl;
simProg << "***************************************************" << std::endl;
