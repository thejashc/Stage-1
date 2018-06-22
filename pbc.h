#if WALL_ON	// pbc only in x and y and z
	mx = (int) round( ( particles[i].r.X - boxHalve[x] ) * boxRecip[x] );
	my = (int) round( ( particles[i].r.Y - boxHalve[y] ) * boxRecip[y] );
	mz = (int) round( ( particles[i].r.Z - boxHalve[z] ) * boxRecip[z] );

	// if ( abs(mx) > 1 || abs(my) > 1 || abs(mz) > 1 )     // unacceptably large displacement
	if ( abs(mx) > 1 || abs(my) > 1 )     // unacceptably large displacement
	{
		simProg << "*** particle " << i << " has escaped" << std::endl;
		simProg << mx << "  " << my << " " << std::endl;
		abort();
	} // abs(mx)

	particles[i].r.X -= mx * boxEdge[x];    // apply periodic boundary conditions in the x and y directions
	particles[i].r.Y -= my * boxEdge[y];

	// reverse the direction of the velocity of the particle in the normal direction
	// Assumptions:
	//		(a) Only lower wall present
	//		(b) is that the z-velocity of the particle is +ve
	#if LOWER_WALL_ON && !(UPPER_WALL_ON)
		if ( abs(mz) > 1.0 ) {

			zOld 			= particles[i].r.Z - particles[i].w.Z * dt;	// calculate the previous z-position
			tApp 			= ( boxEdge[z] - zOld ) / particles[i].w.Z; 	// time in which particle approaches from top face of the box
			tSep 			= dt - tApp;					// time in which particle separates from top face of the box
			particles[i].w.Z 	*= -1.0;					// reverse the velocity of the particle: away from the top of the box

			if ( tSep < 0.0 || tSep > dt ){
				simProg << "** tSep is " << tSep << " **" << std::endl;abort();}

			particles[i].r.Z 	= boxEdge[z] +  particles[i].w.Z * tSep ;	// finally, integrate EOM from the top face of the box with reverse velocity
		}
	#endif

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
