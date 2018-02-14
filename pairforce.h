Rij = particles[i].r - ( particles[j].r + dR );	
r2 = Rij.getLengthSquared();

if ( r2 <= rc2 ) {

	dist = std::sqrt(r2);
	capRij = Rij/dist;
	wCij = ( 1.0 - dist/rcutoff );
	wCij_pow_2 = wCij*wCij;
	term1 = 0.0;
	term2 = 0.0;
	term3 = 0.0;


	//	// Warren's force
	//	// Vapor-liquid coexistence in many-body dissipative particle dynamics, P. B. Warren, Phys. Rev. E 68, 066702 – Published 18 December 2003
	//	if (r2 <= rd2){
	//		wDij = ( 1.0 - dist/rd_cutoff );
	//		term2 = Bij*(particles[i].dens + particles[j].dens)*wDij;
	//
	//		/*
	//		   double wdij_pow_2 = wdij*wdij; 
	//		   double wij = fifteen_by_twopi*wdij_pow_2;
	//
	//		   p->dens_new += wij;
	//		   q->dens_new += wij;
	//		 */
	//
	//		// pair_pot_en_rep = -1.0*Bij*( p->dens + q->dens)*dist*(1.0 - dist/(2.0*rd_cutoff));
	//	}

	//	term1 = Aij*wCij;
	//	term3 = term1 + term2; 
	//	fCij.X = term3*capRij.X; 
	//	fCij.Y = term3*capRij.Y; 
	//	fCij.Z = term3*capRij.Z; 
	//
	//	particles[i].fC += fCij;
	//	particles[j].fC += fCij*(-1.0); 

	fCij.X = Bij*wCij*capRij.X;
	fCij.Y = Bij*wCij*capRij.Y;
	fCij.Z = Bij*wCij*capRij.Z;

	particles[i].fC += fCij;
	particles[j].fC += fCij*(-1.0); 

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

	// potential energy
	// pair_pot_en_att = -1.0*Aij*dist*(1.0 - dist/(2.0*rcutoff)); 
	pair_pot_en = (Bij/2.0)*wCij_pow_2;
	// pair_pot_en = pair_pot_en_att + pair_pot_en_rep; 
	pot_en += pair_pot_en;

	// non-ideal comp pressure
	// double nonIdealcomp = Vec3D::dot(minRij, fCij)*(1.0/(2.0*dim*volume));
	// pressure += nonIdealcomp;


} // rcutoff
