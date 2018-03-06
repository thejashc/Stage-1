	#if WALL_ON	// pbc only in x and y
	mx = (int) round( ( particles[i].r.X - boxHalve[x] ) * boxRecip[x] );
	my = (int) round( ( particles[i].r.Y - boxHalve[y] ) * boxRecip[y] );

	if ( abs(mx) > 1 || abs(my) > 1 )     // unacceptably large displacement
	{
		std::cout << "*** particle " << i << " has escaped" << std::endl;
		std::cout << mx << "  " << my << std::endl;
		abort();
	} // abs(mx)

	particles[i].r.X -= mx * boxEdge[x];    // apply periodic boundary conditions
	particles[i].r.Y -= my * boxEdge[y];

	#else	// pbc only in x, y and z
	mx = (int) round( ( particles[i].r.X - boxHalve[x] ) * boxRecip[x] );
	my = (int) round( ( particles[i].r.Y - boxHalve[y] ) * boxRecip[y] );
	mz = (int) round( ( particles[i].r.Z - boxHalve[z] ) * boxRecip[z] );

	if ( abs(mx) > 1 || abs(my) > 1 || abs(mz) > 1 )     // unacceptably large displacement
	{
		std::cout << "*** particle " << i << " has escaped" << std::endl;
		std::cout << mx << "  " << my << "  " << mz << std::endl;
		abort();
	} // abs(mx)

	particles[i].r.X -= mx * boxEdge[x];    // apply periodic boundary conditions
	particles[i].r.Y -= my * boxEdge[y];
	particles[i].r.Z -= mz * boxEdge[z];
	#endif
