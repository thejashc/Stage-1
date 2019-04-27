Rij = particles[i].r - ( particles[j].r + particles[j].dR );

// nearest image distance
Rij.X = Rij.X - boxEdge[x] * round( Rij.X / boxEdge[x] );		// rij shear-flow correction : dR
Rij.Y = Rij.Y - boxEdge[y] * round( Rij.Y / boxEdge[y] );
Rij.Z = Rij.Z - boxEdge[z] * round( Rij.Z / boxEdge[z] );

/*
std::cout << " particles[i].type = " << particles[i].type 
          << " particles[j].type = " << particles[j].type 
          << " Brep = " << Brep[particles[i].type][particles[j].type] 
          << " Aatt = " << Aatt[particles[i].type][particles[j].type] 
          << " sigma = " << sigma[particles[i].type][particles[j].type] 
          << " gamma = " << gamma[particles[i].type][particles[j].type] << std::endl;
*/

#if RANDOM_DISSIPATIVE
	#if LEES_EDWARDS_BC 
		wij = particles[i].w - ( particles[j].w + velCorr ); 			// magnitude of velocity correction is ( gradient x boxLength in y ) in the direction of shear
	#else
		wij = particles[i].w - particles[j].w;
	#endif
#endif
r2 = Rij.getLengthSquared();

// setting fWCA to zero before force calculation routine and 
// check for WCA interaction between fluid and inner-core 
// colloidal particle
fWCA.setZero();
wcaInteraction = ( particles[i].type == 1 && particles[j].type == 3 ) || 
                 ( particles[i].type == 3 && particles[j].type == 1 ) ||
                 ( particles[i].type == 2 && particles[j].type == 3 ) ||
                 ( particles[i].type == 3 && particles[j].type == 2 );

if ( r2 <= rc2 ) {

	dist = std::sqrt(r2);
	capRij = Rij/dist;
	wCij = ( 1.0 - dist/rcutoff );
	wCij2 = wCij * wCij;
	term1 = 0.0;
	term2 = 0.0;
	term3 = 0.0;

	//------- WARREN POTENTIAL --------//
	// For implementation check:  Vapor-liquid coexistence in many-body dissipative particle dynamics, P. B. Warren, Phys. Rev. E 68, 066702 – Published 18 December 2003
	if ( r2 <= rd2 ){

		wDij = ( 1.0 - dist/rd_cutoff );
		term2 = Brep[particles[i].type][particles[j].type] * ( particles[i].dens + particles[j].dens ) * wDij;

		wDij2 = wDij * wDij; 

        rho_temp = fifteen_by_twopi_by_rd * wDij2;
        particles[i].dens_new += rho_temp;
        particles[j].dens_new += rho_temp;
	}

    /*
    std::cout << Aatt[0][0] << ", \n" <<
                 Aatt[0][1] << ", \n" << 
                 Aatt[0][2] << ", \n" << 
                 Aatt[0][3] << ", \n" << 
                 Aatt[0][4] << ", \n" << 
                 
                 Aatt[1][0] << ", \n" << 
                 Aatt[1][1] << ", \n" << 
                 Aatt[1][2] << ", \n" << 
                 Aatt[1][3] << ", \n" << 
                 Aatt[1][4] << ", \n" << 
                 
                 Aatt[2][0] << ", \n" <<
                 Aatt[2][1] << ", \n" <<
                 Aatt[2][2] << ", \n" << 
                 Aatt[2][3] << ", \n" <<
                 Aatt[2][4] << ", \n" << 
                 
                 Aatt[3][0] << ", \n" <<
                 Aatt[3][1] << ", \n" <<
                 Aatt[3][2] << ", \n" << 
                 Aatt[3][3] << ", \n" << 
                 Aatt[3][4] << ", \n" << 
            
                 Aatt[4][0] << ", \n" <<
                 Aatt[4][1] << ", \n" <<
                 Aatt[4][2] << ", \n" << 
                 Aatt[4][3] << ", \n" << 
                 Aatt[4][4] << ", \n" << std::endl;
                 */

    if ( r2 <= twoPower1_3_sigma2 && wcaInteraction ){

        r2i = 1. / r2; 
        r6i = pow( r2i, 3. ); 

        ff = 48. * epsilonWCA * sig6 * r2i * r6i * ( sig6*r6i - 0.5 );
        fWCA = ff * Rij;

        // pot_en += 4.0 * epsilonWCA * sig6 * r6i *( sig6*r6i - 1.0) - ecutWCA;
    }

    term1 = Aatt[particles[i].type][particles[j].type] * wCij;
    term3 = term1 + term2; 
    fCij.X = term3 * capRij.X; 
    fCij.Y = term3 * capRij.Y; 
    fCij.Z = term3 * capRij.Z; 

    // rhoBar calculation 
    rhoBartemp = fifteen_by_twopi_by_rc * wCij2;
    particles[i].rhoBar += rhoBartemp;
    particles[j].rhoBar += rhoBartemp;

    particles[i].fC += ( fCij + fWCA );
    particles[j].fC -= ( fCij + fWCA ); 

#if RANDOM_DISSIPATIVE
    // random force	
    /*
    uniRand = randNumGen(seed);
    thetaij = uniRand - 0.5; 
    */
    thetaij = normalDistribution(gen);
    magRand = sigma[particles[i].type][particles[j].type] * wCij * thetaij;

    //std::cout << "sigma = " << sigma[particles[i].type][particles[j].type] << std::endl;
    //std::cout << thetaij << std::endl;

    fRij.X = magRand * capRij.X;
    fRij.Y = magRand * capRij.Y;
    fRij.Z = magRand * capRij.Z;

    sumForce = fRij;			// inv_sqrt_dt is taken care by scaled value of sigma 
    particles[i].fR += sumForce;
    particles[j].fR -= sumForce;

    // dissipative force -- not calculated here
    rDotv = Vec3D::dot( capRij, wij );
    magDiss = -gamma[particles[i].type][particles[j].type] * wCij2 *rDotv;

    //std::cout << "gamma = " << gamma[particles[i].type][particles[j].type] << std::endl;

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

    pDissipative_temp[1][0]	    += Rij.Y * fDij.X;
    pDissipative_temp[1][1] 	+= Rij.Y * fDij.Y;
    pDissipative_temp[1][2] 	+= Rij.Y * fDij.Z;

    pDissipative_temp[2][0] 	+= Rij.Z * fDij.X;
    pDissipative_temp[2][1] 	+= Rij.Z * fDij.Y;
    pDissipative_temp[2][2] 	+= Rij.Z * fDij.Z;

    // Non-Ideal contribution to pressure -- Random forces
    pRandom_temp[0][0] 		+= Rij.X * fRij.X;
    pRandom_temp[0][1] 		+= Rij.X * fRij.Y;
    pRandom_temp[0][2] 		+= Rij.X * fRij.Z;

    pRandom_temp[1][0] 		+= Rij.Y * fRij.X;
    pRandom_temp[1][1] 		+= Rij.Y * fRij.Y;
    pRandom_temp[1][2] 		+= Rij.Y * fRij.Z;

    pRandom_temp[2][0] 		+= Rij.Z * fRij.X;
    pRandom_temp[2][1] 		+= Rij.Z * fRij.Y;
    pRandom_temp[2][2] 		+= Rij.Z * fRij.Z;	
#endif

    pTensCounter += 1;	

    //simProg << i << ", fC = " << particles[i].fC << ", fR= " << particles[i].fR << ", fD= " << particles[i].fD << std::endl;

} // rcutoff

