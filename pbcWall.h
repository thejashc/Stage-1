mx = (int) round( ( particles[i].r.X - boxHalve[x] ) * boxRecip[x] );	
my = (int) round( ( particles[i].r.Y - boxHalve[y] ) * boxRecip[y] );
// mz = (int) round( ( particles[i].r.Z - boxHalve[z] ) * boxRecip[z] );

if ( abs(mx) > 1 || abs(my) > 1 )     // unacceptably large displacement
{
	simProg << "*** particle " << i << " has escaped" << std::endl;
	simProg << mx << "  " << my << "  " << std::endl;
	abort();
} // abs(mx)

particles[i].r.X -= mx * boxEdge[x];    // apply periodic boundary conditions
particles[i].r.Y -= my * boxEdge[y];
// particles[i].r.Z -= mz * boxEdge[z];
