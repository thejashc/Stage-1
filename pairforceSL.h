Rij = particles[i].r - ( particles[j].r + dR );	
r2 = Rij.getLengthSquared();

if ( r2 <= rcW2 ) {

	dist = std::sqrt(r2);
	capRij = Rij/dist;
	wCij = ( 1.0 - dist/rcWcutoff );
	term1 = 0.0;
	term2 = 0.0;
	term3 = 0.0;

	if ( particles[i].type == particles[j].type ){
		simProg << " not a solid liquid interaction between i, j:  " << i << " " << j << std::endl; abort();
	}

	if ( r2 <= rdW2 ){

		wDij = ( 1.0 - dist/rdWcutoff );
		term2 = Bsl * ( particles[i].dens + particles[j].dens ) * wDij;

		wDij2 = wDij * wDij; 
		rho_temp = fifteen_by_twopi_by_rd * wDij2;

		particles[i].dens_new += rho_temp;
		particles[j].dens_new += rho_temp;
	}

	if ( particles[i].type == 0 ){						
		term1 = Asl[i] * wCij;
	}
	else{									// if i^{th} particle is not solid, then j^{th} particle is solid
		term1 = Asl[j] * wCij;
	}

	term3 = term1 + term2; 
	fCWij.X = term3 * capRij.X; 
	fCWij.Y = term3 * capRij.Y; 
	fCWij.Z = term3 * capRij.Z; 

	particles[i].fCW += fCWij;
	particles[j].fCW -= fCWij; 
	
    #if RANDOM_DISSIPATIVE
        // random force	
        uniRand = randNumGen(seed);
        thetaij = uniRand - 0.5; 
        magRand = sigma[particles[i].type][particles[j].type] * wCij * thetaij;
        
        // std::cout << uniRand << std::endl;
        
        fRij.X = magRand * capRij.X;
        fRij.Y = magRand * capRij.Y;
        fRij.Z = magRand * capRij.Z;

        sumForce = fRij;			// inv_sqrt_dt is taken care by scaled value of sigma 
        particles[i].fR += sumForce;
        particles[j].fR -= sumForce;

        // dissipative force -- not calculated here
        rDotv = Vec3D::dot( capRij, wij );
        magDiss = -gamma[particles[i].type][particles[j].type] * wCij2 *rDotv;
        
        fDij.X = magDiss * capRij.X;
        fDij.Y = magDiss * capRij.Y;
        fDij.Z = magDiss * capRij.Z;

        particles[i].fD += fDij;
        particles[j].fD -= fDij;
	#endif

} // rcutoff
