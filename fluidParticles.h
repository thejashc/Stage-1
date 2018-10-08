// ****************** WORD OF CAUTION : Whenever a new for loop or while loop is created, make sure that the 
// ****************** loop variable is not i. i is the loop variable for counting fluid particles

i = 0;
while ( i < fluidCount ){


	// store velocity (mid-step)
	particles[fluid_index[i]].w_old = particles[fluid_index[i]].w;

    // totCOM += particles[fluid_index[i]].w;

    #include "wallBoundaryConditions.h"

	// update velocities (mid-step)
    particles[fluid_index[i]].w += ( particles[fluid_index[i]].fC + particles[fluid_index[i]].fD + 
                                     particles[fluid_index[i]].fR + particles[fluid_index[i]].fext )*( dt/particles[fluid_index[i]].m );

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
