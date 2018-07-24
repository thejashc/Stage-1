#if WALL_ON	// pbc only in x and y and z

	mx = (int) round( ( particles[fluid_index[i]].r.X - boxHalve[x] ) * boxRecip[x] );
	my = (int) round( ( particles[fluid_index[i]].r.Y - boxHalve[y] ) * boxRecip[y] );
	mz = (int) round( ( particles[fluid_index[i]].r.Z - boxHalve[z] ) * boxRecip[z] );

	// if ( abs(mx) > 1 || abs(my) > 1 || abs(mz) > 1 )     // unacceptably large displacement
	if ( abs(mx) > 1 || abs(my) > 1 )     // unacceptably large displacement
	{
		simProg << "*** particle " << i << " has escaped" << std::endl;
		simProg << mx << "  " << my << " " << std::endl;
		abort();
	} // abs(mx)

	particles[fluid_index[i]].r.X -= mx * boxEdge[x];    // apply periodic boundary conditions in the x and y directions
	particles[fluid_index[i]].r.Y -= my * boxEdge[y];

	// reverse the direction of the velocity of the particle in the normal direction
	// Assumptions:
	//		(a) Only lower wall present
	//		(b) is that the z-velocity of the particle is +ve
	#if LOWER_WALL_ON && !(UPPER_WALL_ON)

		if ( abs(mz) == 1.0 ) {

			zOld 			= particles[fluid_index[i]].r.Z - particles[fluid_index[i]].w.Z * dt;	// calculate the previous z-position
			tApp 			= ( boxEdge[z] - zOld ) / particles[fluid_index[i]].w.Z; 	// time in which particle approaches from top face of the box
			tSep 			= dt - tApp;					// time in which particle separates from top face of the box
			particles[fluid_index[i]].w.Z 	*= -1.0;					// reverse the velocity of the particle: away from the top of the box

			if ( tSep < 0.0 || tSep > dt ){
				simProg << "** tSep is " << tSep << " **" << std::endl;abort();}

			particles[fluid_index[i]].r.Z 	= boxEdge[z] +  particles[fluid_index[i]].w.Z * tSep ;	// finally, integrate EOM from the top face of the box with reverse velocity
		}

	#endif
	#if CAPILLARY_CYLINDER || CAPILLARY_SQUARE
		
		if ( abs(mz) == 1.0 ) {

			// simProg << " Particle " << i << " has temporarily gone out of the box " << std::endl;
			
			// simProg << " initial particle position = " << particles[fluid_index[i]].r.Z << ", initial particle velocity = " << particles[fluid_index[i]].w.Z << std::endl;

			zOld 			= particles[fluid_index[i]].r.Z - particles[fluid_index[i]].w.Z * dt;	// calculate the previous z-position
			tApp 			= ( particles[fluid_index[i]].w.Z > 0. ) ? ( ( boxEdge[z] - zOld ) / particles[fluid_index[i]].w.Z ) : ( zOld / fabs( particles[fluid_index[i]].w.Z ) ); 
			tSep 			= dt - tApp;					// time in which particle separates from top face of the box
			particles[fluid_index[i]].w.Z 	*= -1.0;					// reverse the velocity of the particle: away from the top of the box

			// simProg << " zOld = " << zOld << ", tApp = " << tApp << ", tSep = " << tSep << std::endl;

			if ( tSep < 0.0 || tSep > dt ){
				simProg << "** tSep is     = " 	<< tSep << " **" << std::endl;
				simProg << "** tApp is     = " 	<< tApp << " **" << std::endl;
				simProg << "** velocity is = " 	<< particles[fluid_index[i]].w.Z << " **" << std::endl;
				simProg << "** zOld is     = " 	<< zOld << " **" << std::endl;
				abort();
			}

			particles[fluid_index[i]].r.Z 	= ( particles[fluid_index[i]].w.Z < 0. ) ? ( boxEdge[z] +  particles[fluid_index[i]].w.Z * tSep ) : ( particles[fluid_index[i]].w.Z * tSep ) ;	// finally, integrate EOM from the top face of the box with reverse velocity
			
			// simProg << " final particle position = " << particles[fluid_index[i]].r.Z << ", initial particle velocity = " << particles[fluid_index[i]].w.Z << std::endl;
		}

	#endif

	//if ( ( particles[fluid_index[i]].r.Z < wallLowPos - 2*wallPenetration ) || ( particles[fluid_index[i]].r.Z > wallTopPos + 2*wallPenetration ) )
	//	simProg << " Particle [" << i << "] infiltrating the wall: " << particles[fluid_index[i]].r << std::endl;

#else	
	mx = (int) round( ( particles[fluid_index[i]].r.X - boxHalve[x] ) * boxRecip[x] );	
	my = (int) round( ( particles[fluid_index[i]].r.Y - boxHalve[y] ) * boxRecip[y] );
	mz = (int) round( ( particles[fluid_index[i]].r.Z - boxHalve[z] ) * boxRecip[z] );

	if ( abs(mx) > 1 || abs(my) > 1 || abs(mz) > 1 )     // unacceptably large displacement
	{
		simProg << "*** particle " << i << " has escaped" << std::endl;
		simProg << mx << "  " << my << "  " << mz << std::endl;
		abort();
	} // abs(mx)

	#if LEES_EDWARDS_BC
		particles[fluid_index[i]].r.X -= my * strain; 		// position-correction : ( is correct for odd values of abs( my ), for example: 1, 3, 5, 7) 
		particles[fluid_index[i]].w.X -= my * strainRate;		// velocity-correction ( mid-step velocity used )

		mx = (int) round( ( particles[fluid_index[i]].r.X - boxHalve[x] ) * boxRecip[x] );	// recalculate mx
	#endif	// LEES_EDWARDS_BC

	particles[fluid_index[i]].r.X -= mx * boxEdge[x];    // apply periodic boundary conditions
	particles[fluid_index[i]].r.Y -= my * boxEdge[y];
	particles[fluid_index[i]].r.Z -= mz * boxEdge[z];
#endif // WALL_ON
