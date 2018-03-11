Rij = particles[i].r - ( particles[j].r + dR );	
r2 = Rij.getLengthSquared();

if ( r2 <= rcW2 ) {

	dist = std::sqrt(r2);

	if ( ( particles[i].type == particles[j].type )  ){
		std::cout << " not a solid liquid interaction between i, j:  " << i << " " << j << std::endl; abort();
	}

	// derivative of weight function
	// For reference: Dissipative particle dynamics simulation of multiphase fluid flow in microchannels and microchannel networks by Moubin Liu, Physics of Fluids, Paul Meakin and Hai Huang, 2007

	// constant force 
	if ( dist <= root2 )
		fSL = fWallcutoff;
	else{ 

		// w1P 
		if ( dist < rcWby2 )
			w1P = -18.75 * dist + 35.1563 * r2;
		else 
			w1P = -11.7188 * pow( dist - rcWcutoff , 2.0);

		// w2P
		if ( dist < rdWby2 )
			w2P = -24.4898 * dist + 52.4781 * r2;
		else if ( dist >= rdWby2 && dist <= rdWcutoff ) 
			w2P = -17.4927 * pow( dist - rdWcutoff , 2.0);

		fSL =  Asl * w2P + Bsl * w1P;		
	} // ( dist > root2 )

	// debug
	// std::cout << std::setw(10) << dist << " " << std::setw(10) << w1P << " " << std::setw(10) << w2P << " " << std::setw(10) << fSL << std::endl;

	// std::cout << "r.i = " << particles[i].r << ",\t r.j = " << particles[j].r << ",\t dist:" << std::sqrt(r2) << ",\t Rij: " << Rij << ",\t fSL: " << fSL << std::endl;
	capRij = Rij/dist;

	fCWij.X = fSL * capRij.X; 
	fCWij.Y = fSL * capRij.Y; 
	fCWij.Z = fSL * capRij.Z; 

	// adding the wall force contribution to both liquid and solid species
	particles[i].fCW += fCWij;
	particles[j].fCW -= fCWij;

	// density contribution to the particle due to wall -- add contribution to both
	// wDij = ( 1.0 - dist/rd_cutoff );
	// wDij2 = wDij * wDij; 
	// rho_temp = fifteen_by_twopi_by_rd * wDij2;

	// particles[i].dens_new += rho_temp;
	// particles[j].dens_new += rho_temp;

	// reset values of w1P, w2P
	w1P = 0.;
	w2P = 0.;

} // rcutoff
