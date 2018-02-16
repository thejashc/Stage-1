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

	//------ SOFT PAIR POTENTIAL -----//
	/*
	fCij.X = Bij*wCij*capRij.X;
	fCij.Y = Bij*wCij*capRij.Y;
	fCij.Z = Bij*wCij*capRij.Z;*/

	//------- SOFT PAIR POTENTIAL ENERGY--------//
	/*
	pair_pot_en = (Bij/2.0)*wCij_pow_2;
	pot_en += pair_pot_en;*/

	//------- WARREN POTENTIAL --------//
	// For implementation check:  Vapor-liquid coexistence in many-body dissipative particle dynamics, P. B. Warren, Phys. Rev. E 68, 066702 – Published 18 December 2003
	if ( r2 <= rd2 ){

		wDij = ( 1.0 - dist/rd_cutoff );
		term2 = Bij * ( particles[i].dens + particles[j].dens ) * wDij;

		wDij2 = wDij * wDij; 
		rho_temp = fifteen_by_twopi_by_rd * wDij2;

		particles[i].dens_new += rho_temp;
		particles[j].dens_new += rho_temp;
	}

	term1 = Aij * wCij;
	term3 = term1 + term2; 
	fCij.X = term3 * capRij.X; 
	fCij.Y = term3 * capRij.Y; 
	fCij.Z = term3 * capRij.Z; 

	// rhoBar calculation 
	rhoBartemp = fifteen_by_twopi_by_rc * wCij2;
	particles[i].rhoBar += rhoBartemp;
	particles[j].rhoBar += rhoBartemp;

	particles[i].fC += fCij;
	particles[j].fC -= fCij; 

	/*
	// random force	
	double uniRand = d(rd); 
	double thetaij = std::sqrt(12.0)*(uniRand-0.5); 
	double magRand = sigma*wR*thetaij;
	fRij.X = magRand*capRij.X;
	fRij.Y = magRand*capRij.Y;
	fRij.Z = magRand*capRij.Z;

	Vec3D sumForce = fRij*inv_sqrt_dt;
	p->fR += sumForce;
	q->fR += -1.0*sumForce;

	// dissipative force -- not calculated here
	double rDotv = Vec3D::dot( capRij, Vij );
	double magDiss = -1.0*gamma*wR_pow_2*rDotv;
	fDij.X = magDiss*capRij.X;
	fDij.Y = magDiss*capRij.Y;
	fDij.Z = magDiss*capRij.Z;

	p->fD += fDij;
	q->fD += -1.0*fDij;
	 */
	
	// non-ideal comp pressure
	// double nonIdealcomp = Vec3D::dot(minRij, fCij)*(1.0/(2.0*dim*volume));
	// pressure += nonIdealcomp;


} // rcutoff
