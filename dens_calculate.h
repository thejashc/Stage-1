Rij = particles[i].r - ( particles[j].r + dR );	
r2 = Rij.getLengthSquared();
dist = std::sqrt(r2);
capRij = Rij/dist;

if ( r2 <= rc2 ){

	wCij = ( 1.0 - dist/rcutoff);
    wCij2 = wCij * wCij;

	if ( r2 <= rd2 ){
		wDij = ( 1.0 - dist/rd_cutoff );
		wDij2 = wDij*wDij; 
		rho_temp = fifteen_by_twopi_by_rd * wDij2;

		particles[i].dens += rho_temp;
		particles[j].dens += rho_temp;
	}

	// rhoBar calculation 
	rhoBartemp = fifteen_by_twopi_by_rc * wCij2;
	particles[i].rhoBar += rhoBartemp;
	particles[j].rhoBar += rhoBartemp;
}
