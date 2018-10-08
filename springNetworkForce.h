i=0;
while ( i < solidCount )
{
    for ( k=1; k<= particles[solid_index[i]].bondIndex[0]; ++k ) {

        j = particles[solid_index[i]].bondIndex[k];     // jth particle

        Rij.X = particles[solid_index[i]].r.X - particles[solid_index[j]].r.X - particles[solid_index[i]].equilibriumBondLength[k][0];     // basically, finding r - r0 vector
        Rij.Y = particles[solid_index[i]].r.Y - particles[solid_index[j]].r.Y - particles[solid_index[i]].equilibriumBondLength[k][1];
        Rij.Z = particles[solid_index[i]].r.Z - particles[solid_index[j]].r.Z - particles[solid_index[i]].equilibriumBondLength[k][2];

        // nearest image distance
        Rij.X = Rij.X - boxEdge[x] * round( Rij.X / boxEdge[x] );		// rij shear-flow correction : dR
        Rij.Y = Rij.Y - boxEdge[y] * round( Rij.Y / boxEdge[y] );
        Rij.Z = Rij.Z - boxEdge[z] * round( Rij.Z / boxEdge[z] );

        r2 = Rij.getLengthSquared();

        particles[solid_index[i]].fHarmonic += -kWall * Rij;
        particles[solid_index[j]].fHarmonic +=  kWall * Rij;

        springPotEn += 0.5 * kWall * r2; 
    }

    i++;
}
