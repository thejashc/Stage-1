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

            // net-force between particle i and j
            springForce = -kWallNgbr * ( ext - refExt ) * capRij;

            // calculate the force as extension of length
            particles[i].fHarmonic += springForce;
            particles[j].fHarmonic -= springForce; 

            // pressure tensor due to bonded-interactions
            pBondInteractions_temp[0][0] += Rij.X * springForce.X;
            pBondInteractions_temp[0][1] += Rij.X * springForce.Y;
            pBondInteractions_temp[0][2] += Rij.X * springForce.Z;

            pBondInteractions_temp[1][0] += Rij.Y * springForce.X;
            pBondInteractions_temp[1][1] += Rij.Y * springForce.Y;
            pBondInteractions_temp[1][2] += Rij.Y * springForce.Z;

            pBondInteractions_temp[2][0] += Rij.Z * springForce.X;
            pBondInteractions_temp[2][1] += Rij.Z * springForce.Y;
            pBondInteractions_temp[2][2] += Rij.Z * springForce.Z;

            springPotEn += 0.5 * kWallNgbr * pow( ext - refExt, 2. ); 
        }
    }

    idx++;
    i = solid_index[idx];
}
