mx = (int) round( ( particles[solid_index[i]].r.X - boxHalve[x] ) * boxRecip[x] );	
my = (int) round( ( particles[solid_index[i]].r.Y - boxHalve[y] ) * boxRecip[y] );
mz = (int) round( ( particles[solid_index[i]].r.Z - boxHalve[z] ) * boxRecip[z] );

if ( abs(mx) > 1 || abs(my) > 1 )     // unacceptably large displacement
{
	simProg << "*** particle " << i << " has escaped" << std::endl;
	simProg << mx << "  " << my << "  " << std::endl;
	abort();
} // abs(mx)

particles[solid_index[i]].r.X -= mx * boxEdge[x];    // apply periodic boundary conditions
particles[solid_index[i]].r.Y -= my * boxEdge[y];
particles[solid_index[i]].r.Z -= mz * boxEdge[z];
