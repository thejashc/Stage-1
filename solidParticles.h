// integrate equations of motion for wall -- using leap-frog algorithm
#if WALL_ON
	vx2 = 0.;
	vy2 = 0.;
	vz2 = 0.;
	
	for ( i = solid_index[0] ; i <= solid_index[solidCount-1] ; ++i ) {
		
		particles[i].w_old = particles[i].w;		
	
		#include "fHarmonic.h"
	
		// switch on all the forces including force due to fluid particles starting from step = 50000
		particles[i].w += ( particles[i].fCW + particles[i].fC + particles[i].fHarmonic +  particles[i].fD + particles[i].fR ) * ( dt/ particles[i].m );
	
		particles[i].r += particles[i].w * dt;
	
		particles[i].v = 0.5*( particles[i].w + particles[i].w_old );
	
		
		// implement periodic boundary condition -- replace with special boundary conditions for the wall
		#include "pbcWall.h"
	
		// calculate the kinetic energy
		vx2 += particles[i].v.X * particles[i].v.X;
		vy2 += particles[i].v.Y * particles[i].v.Y;
		vz2 += particles[i].v.Z * particles[i].v.Z;
	}
	
	// temperature calculation	
	v2 = ( vx2 + vy2 + vz2 ) / ( 3. * solidCount );
	kin_en = 0.5 * v2;		// unit mass assumption
	wallTemp = 2.*kin_en;
#endif // WALL_ON

//********** DEBUG STATEMENTS ****************//
// simProg << " i= " << i << ", r0 = " << particles[i].r0 << ", w = " << particles[i].w << ", dist = " << dist << std::endl;  	
// simProg << "conservative force on particle[i] is " << particles[i].fC.X << "\t" << particles[i].fC.Y << "\t" << particles[i].fC.Z << std::endl;

// particles[i].w += ( particles[i].fCW + particles[i].fC + particles[i].fHarmonic +  particles[i].fD + particles[i].fR ) * ( dt/ particles[i].m ) * ( step > 20000 );	// conservative + dissipative
// particles[i].w += ( particles[i].fCW + particles[i].fC + particles[i].fHarmonic ) * ( dt/ particles[i].m ) * ( step > 20000 );	// only conservative forces
// std::cout << " i= " << i << ", r0 = " << particles[i].r0 << ", r = " << particles[i].r << ", dist = " << dist << std::endl;  	
