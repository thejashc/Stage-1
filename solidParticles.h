// ****************** WORD OF CAUTION : Whenever a new for loop or while loop is created, make sure that the 
// ****************** loop variable is not i. i is the loop variable for counting fluid particles


// integrate equations of motion for wall -- using leap-frog algorithm
#if WALL_ON
	vx2 = 0.;
	vy2 = 0.;
	vz2 = 0.;

	i = 0;
	while ( i < solidCount ){
		
		particles[solid_index[i]].w_old = particles[solid_index[i]].w;		
	
		#include "fHarmonic.h"
	
		// switch on all the forces including force due to fluid particles starting from step = 50000
		particles[solid_index[i]].w += ( particles[solid_index[i]].fCW + particles[solid_index[i]].fC + particles[solid_index[i]].fHarmonic +  particles[solid_index[i]].fD + particles[solid_index[i]].fR ) * ( dt/ particles[solid_index[i]].m );
	
		particles[solid_index[i]].r += particles[solid_index[i]].w * dt;
	
		particles[solid_index[i]].v = 0.5*( particles[solid_index[i]].w + particles[solid_index[i]].w_old );
	
		
		// implement periodic boundary condition -- replace with special boundary conditions for the wall
		#include "pbcWall.h"
	
		// calculate the kinetic energy
		vx2 += particles[solid_index[i]].v.X * particles[solid_index[i]].v.X;
		vy2 += particles[solid_index[i]].v.Y * particles[solid_index[i]].v.Y;
		vz2 += particles[solid_index[i]].v.Z * particles[solid_index[i]].v.Z;

		#if PISTON
			if ( i >= solidCount - pistonParticles   )						// if i^{th} particle is solid
				forceOnPiston += particles[solid_index[i]].fCW.Z;				// z-component of force on the piston due to fluid, the conservative force cancels out because internal forces
		#endif												

		// update counter for solid particles
		i++;
	}

	// calculate force deficit
	#if PISTON
		#include "movePiston.h"
	#endif 
	
	// temperature calculation	
	v2 = ( vx2 + vy2 + vz2 ) / ( 3. * solidCount );
	kin_en = 0.5 * v2;		// unit mass assumption
	wallTemp = 2.*kin_en;
#endif // WALL_ON

//********** DEBUG STATEMENTS ****************//
// simProg << " i= " << i << ", r0 = " << particles[solid_index[i]].r0 << ", w = " << particles[solid_index[i]].w << ", dist = " << dist << std::endl;  	
// simProg << "conservative force on particle[i] is " << particles[solid_index[i]].fC.X << "\t" << particles[solid_index[i]].fC.Y << "\t" << particles[solid_index[i]].fC.Z << std::endl;

// particles[solid_index[i]].w += ( particles[solid_index[i]].fCW + particles[solid_index[i]].fC + particles[solid_index[i]].fHarmonic +  particles[solid_index[i]].fD + particles[solid_index[i]].fR ) * ( dt/ particles[solid_index[i]].m ) * ( step > 20000 );	// conservative + dissipative
// particles[solid_index[i]].w += ( particles[solid_index[i]].fCW + particles[solid_index[i]].fC + particles[solid_index[i]].fHarmonic ) * ( dt/ particles[solid_index[i]].m ) * ( step > 20000 );	// only conservative forces
// std::cout << " i= " << i << ", r0 = " << particles[solid_index[i]].r0 << ", r = " << particles[solid_index[i]].r << ", dist = " << dist << std::endl;  	
