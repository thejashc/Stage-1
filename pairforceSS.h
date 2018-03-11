// purely to calculate the density of the solid atoms
Rij = particles[i].r - particles[j].r ;	// use absolute distance instead of nearest-neighbour distance
r2 = Rij.getLengthSquared();

if ( r2 <= rdW2 ){

	dist = std::sqrt( r2 );
	wDij = ( 1.0 - dist/rdWcutoff );
	wDij2 = wDij * wDij; 
	rho_temp = fifteen_by_twopi_by_rd * wDij2;

	particles[i].dens_new += rho_temp;
	particles[j].dens_new += rho_temp;
}

