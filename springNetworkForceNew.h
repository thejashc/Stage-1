i=solidStartIndex;
while ( i <= solidEndIndex )
{
    for ( k=1; k<= particles[i].bondIndex[0]; ++k ) {

        j = particles[i].bondIndex[k];     // jth particle

        // find the vector separating the particles
        Rij = particles[i].r - particles[j].r;

        // nearest image distance
        Rij.X = Rij.X - boxEdge[x] * round( Rij.X / boxEdge[x] );
        Rij.Y = Rij.Y - boxEdge[y] * round( Rij.Y / boxEdge[y] );
        Rij.Z = Rij.Z - boxEdge[z] * round( Rij.Z / boxEdge[z] );

        // find spring extension 
        r2     = Rij.getLengthSquared();
        ext    = std::sqrt( r2 );
        refExt = particles[i].eqBondLength[k];

        /*
        std::cout << " particle j = " << j << "\t"
                  << " ext = " << ext << "\t" 
                  << " refExt = " << refExt << std::endl;
                  */

        // find the unit vector
        capRij = Rij/ ext;

        // net-force
        springForce = -kWall * ( ext - refExt ) * capRij;

        // calculate the force as extension of length
        particles[i].fHarmonic += springForce;
        particles[j].fHarmonic -= springForce; 

        springPotEn += 0.5 * kWall * pow( ext - refExt, 2. ); 
    }

    i++;
}
