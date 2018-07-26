// ****************** WORD OF CAUTION : Whenever a new for loop or while loop is created, make sure that the 
// ****************** loop variable is not i. i is the loop variable for counting fluid particles

i = 0;
while ( i < fluidCount ){
	// store velocity (mid-step)
	particles[fluid_index[i]].w_old = particles[fluid_index[i]].w;

	#if WALL_ON

		#if UPPER_WALL && LOWER_WALL

			// additional force from soft potential to avoid particles from entering into wall
			wallLowDist = particles[fluid_index[i]].r.Z - ( wallLowPos - wallPenetration );
			wallTopDist = particles[fluid_index[i]].r.Z - ( wallTopPos + wallPenetration );

			if ( wallLowDist < 0. ){
				strength = 0.5 * ( 1 + tanh( ( step - 2e4 ) / 5e3 ) );  
				particles[fluid_index[i]].fext.Z = -Brep * wallLowDist * ( strength );		// Lower wall -- act on particles below threshold: wallHeight + tolerance
			}
			else if ( wallTopDist > 0. ){
				strength = 0.5 * ( 1 + tanh( ( step - 2e4 ) / 5e3 ) );  
				particles[fluid_index[i]].fext.Z = -Brep * wallTopDist * ( strength );		// Upper wall -- act on particles above threshold: wallHeight - tolerance
			}

			//simProg << "step = " << step << ", ( step > 50 ) " << ( step > 50 ) << ", particles[fluid_index[i]].fext.Z " << particles[fluid_index[i]].fext.Z << std::endl;

			// no external force in X and Y directions : ** Buggy : this should be calculated for the wall-normal direction ** 
			particles[fluid_index[i]].fext.X = 0.; 
			particles[fluid_index[i]].fext.Y = 0.;

		#elif !(UPPER_WALL) && LOWER_WALL

			// additional force from soft potential to avoid particles from entering into wall
			wallLowDist = particles[fluid_index[i]].r.Z - ( wallLowPos - wallPenetration );

			if ( wallLowDist < 0. ){
				strength = 0.5 * ( 1 + tanh( ( step - 2e4 ) / 5e3 ) );  
				particles[fluid_index[i]].fext.Z = -Brep * wallLowDist * ( strength );		// Lower wall -- act on particles below threshold: wallHeight + tolerance
			}

			//simProg << "step = " << step << ", ( step > 50 ) " << ( step > 50 ) << ", particles[fluid_index[i]].fext.Z " << particles[fluid_index[i]].fext.Z << std::endl;

			// no external force in X and Y directions : ** Buggy : this should be calculated for the wall-normal direction ** 
			particles[fluid_index[i]].fext.X = 0.; 
			particles[fluid_index[i]].fext.Y = 0.;

		#elif CAPILLARY_CYLINDER
			
			// (1)  wall force can be in the region where the reservoir separating wall is present
			wallLowDist 	= particles[fluid_index[i]].r.Z - ( wallLowPos - wallPenetration );
			#if PISTON
				distInPiston 	= particles[fluid_index[i]].r.Z - ( pistonStart + wallPenetration );
			#endif

			Rij.X	=  ( cylCenterX - particles[fluid_index[i]].r.X );		// Finding the inward normal to the cylinder
			Rij.Y	=  ( cylCenterY - particles[fluid_index[i]].r.Y );
			Rij.Z	=  0.;

			r2	= Rij.getLengthSquared();
			dist	= std::sqrt( r2 );
			
			rInner	= capRad + wallPenetration;
			rOuter  = capRad + capThick; 

			penDist = dist - rInner;	// distance penetrated
			perDist = dist - rOuter;	// distance from the capillary tube periphery
			inCapTube   = ( ( particles[fluid_index[i]].r.Z > capTubeStart ) && ( particles[fluid_index[i]].r.Z < capTubeEnd ) );
			notInPoreEntry = dist > capRad;

			// the force acts only: (1.1) if it has penetrated beyond wallPenetration inside capillary tube [if]
			// 		   	(1.2) if it is inside the wall region next to capillary [else if]
			if ( wallLowDist < 0. && particles[fluid_index[i]].r.Z > ( wallLowPos - capWallWdth ) && notInPoreEntry ){

				particles[fluid_index[i]].fext.X = 0;
				particles[fluid_index[i]].fext.Y = 0;
				particles[fluid_index[i]].fext.Z = -Brep * wallLowDist ; 
			}
			else if ( ( innerRadius >= 0 ) && ( outerRadius < 0 ) && inCapTube ){

				capRij =  Rij / dist;

				particles[fluid_index[i]].fext.X = Brep * innerRadius * capRij.X ;
				particles[fluid_index[i]].fext.Y = Brep * innerRadius * capRij.Y ;
				particles[fluid_index[i]].fext.Z =  0.;
			}
			#if PISTON
				else if ( distInPiston > 0. ){
					particles[fluid_index[i]].fext.X = 0;
					particles[fluid_index[i]].fext.Y = 0;
					particles[fluid_index[i]].fext.Z = -Brep * distInPiston; 
				}
			#endif

		#elif CAPILLARY_SQUARE

			#include "penetrationIntoSquare.h"

			// (1)  wall force can be in the region where the reservoir separating wall is present
			wallLowDist 	= particles[fluid_index[i]].r.Z - ( wallLowPos - wallPenetration );
			#if PISTON
				distInPiston 	= particles[fluid_index[i]].r.Z - ( pistonStart + wallPenetration );
			#endif
			notInPoreEntry  = ( particles[fluid_index[i]].r.X > sqInnerEdgeXmax ) || 
					  ( particles[fluid_index[i]].r.X < sqInnerEdgeXmin ) || 
					  ( particles[fluid_index[i]].r.Y > sqInnerEdgeYmax ) || 
					  ( particles[fluid_index[i]].r.Y < sqInnerEdgeYmin ); 

			if ( wallLowDist < 0. && particles[fluid_index[i]].r.Z > ( wallLowPos - capWallWdth ) && notInPoreEntry ){

				particles[fluid_index[i]].fext.X = 0;
				particles[fluid_index[i]].fext.Y = 0;
				particles[fluid_index[i]].fext.Z = -Brep * wallLowDist ; 
			}

			#if PISTON
				else if ( distInPiston > 0. ){
					particles[fluid_index[i]].fext.X = 0;
					particles[fluid_index[i]].fext.Y = 0;
					particles[fluid_index[i]].fext.Z = -Brep * distInPiston; 
				}
			#endif

		#endif
	#endif // WALL_ON

	// update velocities (mid-step)
	#if RANDOM_DISSIPATIVE
		#if WALL_ON
			#if BODY_FORCE
				particles[fluid_index[i]].w += ( particles[fluid_index[i]].fC + particles[fluid_index[i]].fCW +  particles[fluid_index[i]].fD + particles[fluid_index[i]].fR + particles[fluid_index[i]].fext + particles[fluid_index[i]].fBody )*(dt/particles[fluid_index[i]].m);
			#else
				particles[fluid_index[i]].w += ( particles[fluid_index[i]].fC + particles[fluid_index[i]].fCW +  particles[fluid_index[i]].fD + particles[fluid_index[i]].fR + particles[fluid_index[i]].fext )*( dt/particles[fluid_index[i]].m );
			#endif
		#else 	
			#if BODY_FORCE
				particles[fluid_index[i]].fBody.X = ( particles[fluid_index[i]].r.Y < 0.5 * boxEdge[y] ) ? fBodyX : -1. * fBodyX;	
				particles[fluid_index[i]].fBody.Y = 0.;
				particles[fluid_index[i]].fBody.Z = 0.; 	

				particles[fluid_index[i]].w += ( particles[fluid_index[i]].fC + particles[fluid_index[i]].fD + particles[fluid_index[i]].fR + particles[fluid_index[i]].fBody )*(dt/particles[fluid_index[i]].m);
			#else
				particles[fluid_index[i]].w += ( particles[fluid_index[i]].fC + particles[fluid_index[i]].fD + particles[fluid_index[i]].fR )*(dt/particles[fluid_index[i]].m);
			#endif
		#endif // WALL_ON
	#else 
		#if WALL_ON
			particles[fluid_index[i]].w += ( particles[fluid_index[i]].fC + particles[fluid_index[i]].fCW + particles[fluid_index[i]].fext )*(dt/particles[fluid_index[i]].m);
		#else
			particles[fluid_index[i]].w += ( particles[fluid_index[i]].fC )*(dt/particles[fluid_index[i]].m);
		#endif
	#endif // RANDOM_DISSIPATIVE

	// update position (integral time step) using the velocities (mid-step)
	particles[fluid_index[i]].r += particles[fluid_index[i]].w*dt;				
	// simProg << i << " " << particles[fluid_index[i]].r << std::endl;

	// implement periodic boundary condition 
	#include "pbc.h"

	// calculate velocity (integral time step)
	particles[fluid_index[i]].v = 0.5*( particles[fluid_index[i]].w_old + particles[fluid_index[i]].w );

	#if LEES_EDWARDS_BC
		dissipativeWork += Vec3D::dot( particles[fluid_index[i]].w, particles[fluid_index[i]].fD ) * ( ( particles[fluid_index[i]].r.Y >= 0.25 * boxEdge[y] ) && ( particles[fluid_index[i]].r.Y <= 0.75 * boxEdge[y] ) );
		randomWork      += Vec3D::dot( particles[fluid_index[i]].w, particles[fluid_index[i]].fR * inv_sqrt_dt );
	#endif

	// distribute velocities into velocity bins
	if ( (step > velHist_tStart) && (step % velHist_tDelta == 0) ) {
		ivelX = ceil( ( velHist_velMax - particles[fluid_index[i]].v.X )/ velHist_velDelta ); 
		ivelY = ceil( ( velHist_velMax - particles[fluid_index[i]].v.Y )/ velHist_velDelta ); 
		ivelZ = ceil( ( velHist_velMax - particles[fluid_index[i]].v.Z )/ velHist_velDelta ); 

		velHistX[ivelX] += 1;
		velHistY[ivelY] += 1;
		velHistZ[ivelZ] += 1;
	}

	#if PLANAR_SLAB
		// calculate density profile
		iRhoZ = ceil ( ( particles[fluid_index[i]].r.getComponent(z) - rhoZ_Zmin ) / rhoZ_Zdelta ) - 1;		
	
		if ( iRhoZ < 0 || iRhoZ > rhoZ_bins  ) { simProg << " rhoZ calculation -- planar slab particle out of bounds" << std::endl; abort(); } 						
		rhoZ[ iRhoZ ] += 1;
	
			/*
			#if WALL_ON
			if ( step > nZ_tStart ){
				
				// finding COM for fluid particles
				xCOM += particles[fluid_index[i]].r.X;
				yCOM += particles[fluid_index[i]].r.Y;
				zCOM += particles[fluid_index[i]].r.Z;
	
				#include "liquidvaporProfileCalculate.h"
			}
			#endif
			*/
		
	#elif CYLINDER_DROPLET
		// calculate radial density profile
		radPos 	= std::sqrt( pow( particles[fluid_index[i]].r.X - xCOM, 2.0 ) + pow( particles[fluid_index[i]].r.Y - yCOM, 2.0 ) );
		iRhor 	= round ( ( radPos - rhor_rmin ) / rhor_rdelta );

		if ( iRhor < 0 || iRhor > rhor_bins  ) { simProg << " cylindrical particle out of bounds" << std::endl; abort(); } 						

		rhor[ iRhor ] += 1;
	#endif

	// calculate the kinetic energy
	vx2  += particles[fluid_index[i]].v.X * particles[fluid_index[i]].v.X;
	vy2  += particles[fluid_index[i]].v.Y * particles[fluid_index[i]].v.Y;
	vz2  += particles[fluid_index[i]].v.Z * particles[fluid_index[i]].v.Z;

	#include "pNonIdealKinCalc.h"	

	// calculate momentum
	momX += particles[fluid_index[i]].w.X;
	momY += particles[fluid_index[i]].w.Y;
	momZ += particles[fluid_index[i]].w.Z;

	// update count for fluid particles
	i++;

}

// temperature calculation	
//v2 = ( vx2 + vy2 + vz2 ) / ( 3. * fluidCount );
v2 =  vx2 + vy2 + vz2 ;
kin_en = 0.5 * v2;		// unit mass assumption
temp = ( 2. / 3. )* ( kin_en / fluidCount );

tempSum += temp;
tempCount += 1;
