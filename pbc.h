	#if WALL_ON	// pbc only in x and y and z
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

	//if ( ( particles[i].r.Z < wallLowPos - 2*wallPenetration ) || ( particles[i].r.Z > wallTopPos + 2*wallPenetration ) )
	//	simProg << " Particle [" << i << "] infiltrating the wall: " << particles[i].r << std::endl;

	#else	
	mx = (int) round( ( particles[i].r.X - boxHalve[x] ) * boxRecip[x] );	
	my = (int) round( ( particles[i].r.Y - boxHalve[y] ) * boxRecip[y] );
	mz = (int) round( ( particles[i].r.Z - boxHalve[z] ) * boxRecip[z] );

	if ( abs(mx) > 1 || abs(my) > 1 || abs(mz) > 1 )     // unacceptably large displacement
	{
		simProg << "*** particle " << i << " has escaped" << std::endl;
		simProg << mx << "  " << my << "  " << mz << std::endl;
		abort();
	} // abs(mx)

	#if LEES_EDWARDS_BC
	particles[i].r.X -= my * strain; 		// position-correction : ( is correct for odd values of abs( my ), for example: 1, 3, 5, 7) 
	particles[i].w.X -= my * strainRate;		// velocity-correction ( mid-step velocity used )
	
	mx = (int) round( ( particles[i].r.X - boxHalve[x] ) * boxRecip[x] );	// recalculate mx
	#endif	// LEES_EDWARDS_BC
	
	particles[i].r.X -= mx * boxEdge[x];    // apply periodic boundary conditions
	particles[i].r.Y -= my * boxEdge[y];
	particles[i].r.Z -= mz * boxEdge[z];

	#endif // WALL_ON
