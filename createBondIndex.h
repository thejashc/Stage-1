Rij = particles[i].r - particles[j].r;

// nearest image distance
Rij.X = Rij.X - boxEdge[x] * round( Rij.X / boxEdge[x] );
Rij.Y = Rij.Y - boxEdge[y] * round( Rij.Y / boxEdge[y] );
Rij.Z = Rij.Z - boxEdge[z] * round( Rij.Z / boxEdge[z] );

r2 = Rij.getLengthSquared();
dist = std::sqrt(r2);

aCube = pow( 1. / initRho, 1./3. );

if ( ( dist < 0.5 ) && ( particles[i].type == 0 && particles[j].type == 0 ) ) {

    k = particles[i].bondIndex[0] + 1;

    // add neighbor to the list
    particles[i].bondIndex[k] = j;

    // create initial bond vectors --- notice that equilibriumBondLength[0][1:3] is not used
    particles[i].equilibriumBondLength[k][0] = Rij.X;
    particles[i].equilibriumBondLength[k][1] = Rij.Y;
    particles[i].equilibriumBondLength[k][2] = Rij.Z;

    // increment the number of neighbors
    particles[i].bondIndex[0]++;                    // increment the counter for particle i
}

// particles[j].bondIndex[particles[j].bondIndex[0] + 1] = i; -- for debugging and checking bonds
// particles[j].bondIndex[0] = particles[j].bondIndex[0] + 1;        // increment the counter for the number of neighbors
//
// printing the bondIndices
/*
i = 0;
while ( i < solidCount ) {
    simProg << " position of  " << i << " is " << particles[i].r << " , with bond indices " << particles[i].bondIndex[0] << std::endl;

    for ( j=1; j<= particles[i].bondIndex[0]; ++j ){ 
        simProg << " neighbor " << j+1 << " is: " << particles[i].bondIndex[j] << ", equilibrium distance : " << particles[i].equilibriumBondLength[j][0] << ", "
                                                                                                              << particles[i].equilibriumBondLength[j][1] << ", "
                                                                                                              << particles[i].equilibriumBondLength[j][2] << std::endl;}



    i++;
}
*/
