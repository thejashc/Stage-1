Rij = particles[solid_index[i]].r - particles[solid_index[i]].r0;	

Rij.X = Rij.X - boxEdge[x] * round( Rij.X / boxEdge[x] );
Rij.Y = Rij.Y - boxEdge[y] * round( Rij.Y / boxEdge[y] );
Rij.Z = Rij.Z - boxEdge[z] * round( Rij.Z / boxEdge[z] );

r2 = Rij.getLengthSquared();
dist = std::sqrt(r2);

if ( dist != 0 )
	capRij = Rij/dist;
else
	capRij.setZero();

particles[solid_index[i]].fHarmonic.X = -1. * kWall * dist * capRij.X;	// spring force on the solid particle
particles[solid_index[i]].fHarmonic.Y = -1. * kWall * dist * capRij.Y;	
particles[solid_index[i]].fHarmonic.Z = -1. * kWall * dist * capRij.Z;	
