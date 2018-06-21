Rij = particles[i].r - ( particles[j].r + dR );

// nearest image distance
Rij.X = Rij.X - boxEdge[x] * round( Rij.X / boxEdge[x] );		// rij shear-flow correction : dR
Rij.Y = Rij.Y - boxEdge[y] * round( Rij.Y / boxEdge[y] );
Rij.Z = Rij.Z - boxEdge[z] * round( Rij.Z / boxEdge[z] );

#if RANDOM_DISSIPATIVE
	#if LEES_EDWARDS_BC 
		wij = particles[i].w - ( particles[j].w + velCorr ); 			// magnitude of velocity correction is ( gradient x boxLength in y ) in the direction of shear
	#else
		wij = particles[i].w - particles[j].w;
	#endif
#endif
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
		simProg << " not a  liquid liquid interaction between i, j:  " << particles[i].type << " " << particles[j].type << std::endl; abort();
	}

	//------ SOFT PAIR POTENTIAL -----//
	/*
	fCij.X = Bll*wCij*capRij.X;
	fCij.Y = Bll*wCij*capRij.Y;
	fCij.Z = Bll*wCij*capRij.Z;*/

	//------- SOFT PAIR POTENTIAL ENERGY--------//
	/*
	pair_pot_en = (Bll/2.0)*wCij_pow_2;
	pot_en += pair_pot_en;*/

	//------- WARREN POTENTIAL --------//
	// For implementation check:  Vapor-liquid coexistence in many-body dissipative particle dynamics, P. B. Warren, Phys. Rev. E 68, 066702 – Published 18 December 2003
	if ( r2 <= rd2 ){

		wDij = ( 1.0 - dist/rd_cutoff );
		term2 = Bll * ( particles[i].dens + particles[j].dens ) * wDij;

		wDij2 = wDij * wDij; 

		#if !(DENS_EXACT)
		rho_temp = fifteen_by_twopi_by_rd * wDij2;
		particles[i].dens_new += rho_temp;
		particles[j].dens_new += rho_temp;
		#endif
	}

	term1 = All * wCij;
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

	// simProg << "i, mi[x], mi[y], mi[z] = " <<  i << " " << mi[x] << " " << mi[y] << " " <<  mi[z] ;
	// simProg << ", j, mj[x], mj[y], mj[z] = " << j << " " << mj[x] << " " << mj[y] << " " <<  mj[z] ;
	// simProg << ", fCij = " << fCij  << ", fC = " << particles[i].fC << std::endl; 

	#if RANDOM_DISSIPATIVE
	// random force	
	uniRand = randNumGen(seed);
	thetaij = std::sqrt(12.0)*(uniRand-0.5); 
	magRand = sigma * wCij * thetaij;
	
	// std::cout << uniRand << std::endl;
	
	fRij.X = magRand * capRij.X;
	fRij.Y = magRand * capRij.Y;
	fRij.Z = magRand * capRij.Z;

	sumForce = fRij*inv_sqrt_dt;
	particles[i].fR += sumForce;
	particles[j].fR -= sumForce;

	// dissipative force -- not calculated here
	rDotv = Vec3D::dot( capRij, wij );
	magDiss = -1.0 * gamma * wCij2 *rDotv;
	
	fDij.X = magDiss * capRij.X;
	fDij.Y = magDiss * capRij.Y;
	fDij.Z = magDiss * capRij.Z;

	particles[i].fD += fDij;
	particles[j].fD -= fDij;
	#endif

	// Non-Ideal contribution to pressure -- Conservative forces	
	pNonIdeal_temp[0][0] 		+= Rij.X * fCij.X;
	pNonIdeal_temp[0][1] 		+= Rij.X * fCij.Y;
	pNonIdeal_temp[0][2] 		+= Rij.X * fCij.Z;

	pNonIdeal_temp[1][0] 		+= Rij.Y * fCij.X;
	pNonIdeal_temp[1][1] 		+= Rij.Y * fCij.Y;
	pNonIdeal_temp[1][2] 		+= Rij.Y * fCij.Z;

	pNonIdeal_temp[2][0] 		+= Rij.Z * fCij.X;
	pNonIdeal_temp[2][1] 		+= Rij.Z * fCij.Y;
	pNonIdeal_temp[2][2] 		+= Rij.Z * fCij.Z;

	#if RANDOM_DISSIPATIVE
	// Non-Ideal contribution to pressure -- Dissipative forces
	pDissipative_temp[0][0] 	+= Rij.X * fDij.X;
	pDissipative_temp[0][1] 	+= Rij.X * fDij.Y;
	pDissipative_temp[0][2] 	+= Rij.X * fDij.Z;

	pDissipative_temp[1][0]	        += Rij.Y * fDij.X;
	pDissipative_temp[1][1] 	+= Rij.Y * fDij.Y;
	pDissipative_temp[1][2] 	+= Rij.Y * fDij.Z;

	pDissipative_temp[2][0] 	+= Rij.Z * fDij.X;
	pDissipative_temp[2][1] 	+= Rij.Z * fDij.Y;
	pDissipative_temp[2][2] 	+= Rij.Z * fDij.Z;

	// Non-Ideal contribution to pressure -- Random forces
	pRandom_temp[0][0] 		+= Rij.X * fRij.X * inv_sqrt_dt;
	pRandom_temp[0][1] 		+= Rij.X * fRij.Y * inv_sqrt_dt;
	pRandom_temp[0][2] 		+= Rij.X * fRij.Z * inv_sqrt_dt;

	pRandom_temp[1][0] 		+= Rij.Y * fRij.X * inv_sqrt_dt;
	pRandom_temp[1][1] 		+= Rij.Y * fRij.Y * inv_sqrt_dt;
	pRandom_temp[1][2] 		+= Rij.Y * fRij.Z * inv_sqrt_dt;

	pRandom_temp[2][0] 		+= Rij.Z * fRij.X * inv_sqrt_dt;
	pRandom_temp[2][1] 		+= Rij.Z * fRij.Y * inv_sqrt_dt;
	pRandom_temp[2][2] 		+= Rij.Z * fRij.Z * inv_sqrt_dt;	
	#endif

	pTensCounter += 1;	

	//simProg << i << ", fC = " << particles[i].fC << ", fR= " << particles[i].fR << ", fD= " << particles[i].fD << std::endl;

} // rcutoff
