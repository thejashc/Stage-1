	#if WALL_ON	// pbc only in x and y
	mx = (int) round( ( particles[i].r.X - boxHalve[x] ) * boxRecip[x] );
	my = (int) round( ( particles[i].r.Y - boxHalve[y] ) * boxRecip[y] );

	if ( abs(mx) > 1 || abs(my) > 1 )     // unacceptably large displacement
	{
		simProg << "*** particle " << i << " has escaped" << std::endl;
		simProg << mx << "  " << my << std::endl;
		abort();
	} // abs(mx)

	particles[i].r.X -= mx * boxEdge[x];    // apply periodic boundary conditions
	particles[i].r.Y -= my * boxEdge[y];

	if ( ( particles[i].r.Z < wallLowPos - 2*wallPenetration ) || ( particles[i].r.Z > wallTopPos + 2*wallPenetration ) )
		simProg << " Particle [" << i << "] infiltrating the wall: " << particles[i].r << std::endl;

	#else	// pbc only in x, y and z
	mx = (int) round( ( particles[i].r.X - boxHalve[x] ) * boxRecip[x] );
	my = (int) round( ( particles[i].r.Y - boxHalve[y] ) * boxRecip[y] );
	mz = (int) round( ( particles[i].r.Z - boxHalve[z] ) * boxRecip[z] );

	if ( abs(mx) > 1 || abs(my) > 1 || abs(mz) > 1 )     // unacceptably large displacement
	{
		simProg << "*** particle " << i << " has escaped" << std::endl;
		simProg << mx << "  " << my << "  " << mz << std::endl;
		abort();
	} // abs(mx)

	particles[i].r.X -= mx * boxEdge[x];    // apply periodic boundary conditions
	particles[i].r.Y -= my * boxEdge[y];
	particles[i].r.Z -= mz * boxEdge[z];
	#endif
