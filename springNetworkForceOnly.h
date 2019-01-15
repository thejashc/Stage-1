idx = 0;
i = solid_index[idx];

while ( idx < solidCount )
{
    if ( i >= ngbrIdxStart && i <= ngbrIdxEnd ){

        /*std::cout << "************ Particle " << i << " - NGBR ************" << std::endl; */
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
            std::cout << " Particle " << j << " , eq bond length = " << refExt << std::endl; 
            std::cout << " particle j = " << j << "\t"
                      << " ext = " << ext << "\t" 
                      << " refExt = " << refExt << std::endl;
                      */

            // find the unit vector
            capRij = Rij/ ext;

            // net-force
            springForce = -kWallNgbr * ( ext - refExt ) * capRij;

            // calculate the force as extension of length
            particles[i].fHarmonic += springForce;
            particles[j].fHarmonic -= springForce; 

            springPotEn += 0.5 * kWallNgbr * pow( ext - refExt, 2. ); 
        }
    }
}
