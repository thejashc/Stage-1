for ( i = fluid_index[0] ; i <= fluid_index[fluidCount-1] ; ++i ) {
	// store velocity (mid-step)
	particles[i].w_old = particles[i].w;

	#if WALL_ON
		// additional force from soft potential to avoid particles from entering into wall
		wallLowDist = particles[i].r.Z - ( wallLowPos - wallPenetration );
		wallTopDist = particles[i].r.Z - ( wallTopPos + wallPenetration );

		if ( wallLowDist < 0. ){
			strength = 0.5 * ( 1 + tanh( ( step - 2e4 ) / 5e3 ) );  
			particles[i].fext.Z = -Brep * wallLowDist * ( strength );		// Lower wall -- act on particles below threshold: wallHeight + tolerance
		}
		else if ( wallTopDist > 0. ){
			strength = 0.5 * ( 1 + tanh( ( step - 2e4 ) / 5e3 ) );  
			particles[i].fext.Z = -Brep * wallTopDist * ( strength );		// Upper wall -- act on particles above threshold: wallHeight - tolerance
		}

		//simProg << "step = " << step << ", ( step > 50 ) " << ( step > 50 ) << ", particles[i].fext.Z " << particles[i].fext.Z << std::endl;

		// no external force in X and Y directions : ** Buggy : this should be calculated for the wall-normal direction ** 
		particles[i].fext.X = 0.; 
		particles[i].fext.Y = 0.;
	#endif // WALL_ON

	// update velocities (mid-step)
	#if RANDOM_DISSIPATIVE
		#if WALL_ON
			#if BODY_FORCE
				particles[i].w += ( particles[i].fC + particles[i].fCW +  particles[i].fD + particles[i].fR + particles[i].fext + particles[i].fBody )*(dt/particles[i].m);
			#else
				particles[i].w += ( particles[i].fC + particles[i].fCW +  particles[i].fD + particles[i].fR + particles[i].fext )*( dt/particles[i].m );
			#endif
		#else 	
			#if BODY_FORCE
				particles[i].fBody.X = ( particles[i].r.Y < 0.5 * boxEdge[y] ) ? fBodyX : -1. * fBodyX;	
				particles[i].fBody.Y = 0.;
				particles[i].fBody.Z = 0.; 	

				particles[i].w += ( particles[i].fC + particles[i].fD + particles[i].fR + particles[i].fBody )*(dt/particles[i].m);
			#else
				particles[i].w += ( particles[i].fC + particles[i].fD + particles[i].fR )*(dt/particles[i].m);
			#endif
		#endif // WALL_ON
	#else 
		#if WALL_ON
			particles[i].w += ( particles[i].fC + particles[i].fCW + particles[i].fext )*(dt/particles[i].m);
		#else
			particles[i].w += ( particles[i].fC )*(dt/particles[i].m);
		#endif
	#endif // RANDOM_DISSIPATIVE

	// update position (integral time step) using the velocities (mid-step)
	particles[i].r += particles[i].w*dt;				
	// simProg << i << " " << particles[i].r << std::endl;

	// implement periodic boundary condition 
	#include "pbc.h"

	// calculate velocity (integral time step)
	particles[i].v = 0.5*( particles[i].w_old + particles[i].w );

	#if LEES_EDWARDS_BC
		dissipativeWork += Vec3D::dot( particles[i].w, particles[i].fD ) * ( ( particles[i].r.Y >= 0.25 * boxEdge[y] ) && ( particles[i].r.Y <= 0.75 * boxEdge[y] ) );
		randomWork      += Vec3D::dot( particles[i].w, particles[i].fR * inv_sqrt_dt );
	#endif

	// distribute velocities into velocity bins
	if ( (step > velHist_tStart) && (step % velHist_tDelta == 0) ) {
		ivelX = ceil( ( velHist_velMax - particles[i].v.X )/ velHist_velDelta ); 
		ivelY = ceil( ( velHist_velMax - particles[i].v.Y )/ velHist_velDelta ); 
		ivelZ = ceil( ( velHist_velMax - particles[i].v.Z )/ velHist_velDelta ); 

		velHistX[ivelX] += 1;
		velHistY[ivelY] += 1;
		velHistZ[ivelZ] += 1;
	}

	#if PLANAR_SLAB
		// calculate density profile
		iRhoZ = ceil ( ( particles[i].r.getComponent(z) - rhoZ_Zmin ) / rhoZ_Zdelta ) - 1;		
	
		if ( iRhoZ < 0 || iRhoZ > rhoZ_bins  ) { simProg << " rhoZ calculation -- planar slab particle out of bounds" << std::endl; abort(); } 						
		rhoZ[ iRhoZ ] += 1;
	
			/*
			#if WALL_ON
			if ( step > nZ_tStart ){
				
				// finding COM for fluid particles
				xCOM += particles[i].r.X;
				yCOM += particles[i].r.Y;
				zCOM += particles[i].r.Z;
	
				#include "liquidvaporProfileCalculate.h"
			}
			#endif
			*/
		
	#elif CYLINDER_DROPLET
		// calculate radial density profile
		radPos 	= std::sqrt( pow( particles[i].r.X - xCOM, 2.0 ) + pow( particles[i].r.Y - yCOM, 2.0 ) );
		iRhor 	= round ( ( radPos - rhor_rmin ) / rhor_rdelta );

		if ( iRhor < 0 || iRhor > rhor_bins  ) { simProg << " cylindrical particle out of bounds" << std::endl; abort(); } 						

		rhor[ iRhor ] += 1;
	#endif

	// calculate the kinetic energy
	vx2  += particles[i].v.X * particles[i].v.X;
	vy2  += particles[i].v.Y * particles[i].v.Y;
	vz2  += particles[i].v.Z * particles[i].v.Z;

	#include "pNonIdealKinCalc.h"	

	// calculate momentum
	momX += particles[i].w.X;
	momY += particles[i].w.Y;
	momZ += particles[i].w.Z;

}

// temperature calculation	
//v2 = ( vx2 + vy2 + vz2 ) / ( 3. * fluidCount );
v2 =  vx2 + vy2 + vz2 ;
kin_en = 0.5 * v2;		// unit mass assumption
temp = ( 2. / 3. )* ( kin_en / fluidCount );

tempSum += temp;
tempCount += 1;
