Rij = particles[i].r - ( particles[j].r + dR );	
r2 = Rij.getLengthSquared();

if ( r2 <= rc2 ) {

	dist = std::sqrt(r2);
	capRij = Rij/dist;
	wCij = ( 1.0 - dist/rcutoff );
	wCij2 = wCij * wCij;
	term1 = 0.0;
	term2 = 0.0;
	term3 = 0.0;

	if ( particles[i].type != particles[j].type ){
		simProg << " not a  liquid liquid interaction between i, j:  " << i << " " << j << std::endl; abort();
	}

	if ( r2 <= rd2 ){

		wDij = ( 1.0 - dist/rd_cutoff );
		term2 = Bss * ( particles[i].dens + particles[j].dens ) * wDij;		// Here Bll is the same as Bss

		wDij2 = wDij * wDij; 
		rho_temp = fifteen_by_twopi_by_rd * wDij2;

		particles[i].dens_new += rho_temp;
		particles[j].dens_new += rho_temp;
	}

	term1 = Ass * wCij;		// Here All is the same as Ass
	term3 = term1 + term2; 
	fCij.X = term3 * capRij.X; 
	fCij.Y = term3 * capRij.Y; 
	fCij.Z = term3 * capRij.Z; 
	
	particles[i].fC += fCij;
	particles[j].fC -= fCij; 

} // rcutoff
