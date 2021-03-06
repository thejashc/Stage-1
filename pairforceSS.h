Rij = particles[i].r - ( particles[j].r + dR );	
#if RANDOM_DISSIPATIVE
	wij = particles[i].w - particles[j].w;
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
		simProg << " not a  solid-solid interaction between i, j:  " << i << " " << j << std::endl; abort();
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

	#if RANDOM_DISSIPATIVE
        // random force	
        uniRand = randNumGen(seed);
        thetaij = uniRand-0.5; 
        magRand = sigma[particles[i].type][particles[j].type] * wCij * thetaij;
        
        fRij.X = magRand * capRij.X;
        fRij.Y = magRand * capRij.Y;
        fRij.Z = magRand * capRij.Z;

        sumForce = fRij;			// inv_sqrt_dt is taken care by scaled value of sigma 
        particles[i].fR += sumForce;
        particles[j].fR -= sumForce;

        // dissipative force
        rDotv = Vec3D::dot( capRij, wij );
        magDiss = -gamma[particles[i].type][particles[j].type] * wCij2 *rDotv;
        
        fDij.X = magDiss * capRij.X;
        fDij.Y = magDiss * capRij.Y;
        fDij.Z = magDiss * capRij.Z;

        particles[i].fD += fDij;
        particles[j].fD -= fDij;
	#endif

} // rcutoff
