	mx = (int) round( ( p.r.X - boxHalve[x] ) * boxRecip[x] );
	my = (int) round( ( p.r.Y - boxHalve[y] ) * boxRecip[y] );
	mz = (int) round( ( p.r.Z - boxHalve[z] ) * boxRecip[z] );

	if ( abs(mx) > 1 || abs(my) > 1 || abs(mz) > 1 )     // unacceptably large displacement
	{
		std::cout << "*** particle " << i << " has escaped" << std::endl;
		std::cout << mx << "  " << my << "  " << mz << std::endl;
		abort();
	} // abs(mx)

	p.r.X -= mx * boxEdge[x];    // apply periodic boundary conditions
	p.r.Y -= my * boxEdge[y];
	p.r.Z -= mz * boxEdge[z];
