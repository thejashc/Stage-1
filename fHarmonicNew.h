j = 0;
i = solid_index[j];
while ( j < solidCount )
{
    Rij = particles[i].r - particles[i].r0;	

    Rij.X = Rij.X - boxEdge[x] * round( Rij.X / boxEdge[x] );
    Rij.Y = Rij.Y - boxEdge[y] * round( Rij.Y / boxEdge[y] );
    Rij.Z = Rij.Z - boxEdge[z] * round( Rij.Z / boxEdge[z] );

    r2 = Rij.getLengthSquared();
    dist = std::sqrt(r2);

    if ( dist != 0 )
        capRij = Rij/dist;
    else
        capRij.setZero();

    particles[i].fHarmonic.X = -1. * kWall * dist * capRij.X;	// spring force on the solid particle
    particles[i].fHarmonic.Y = -1. * kWall * dist * capRij.Y;	
    particles[i].fHarmonic.Z = -1. * kWall * dist * capRij.Z;	

    j++;
    i = solid_index[j];
} // if there are no harmonic forces then that is taken care by resetVar() func where everything is set to 0
