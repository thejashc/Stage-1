#ifndef _Particle_
#define _Particle_

#include "Vec3D.h"

//Declaration of particle class

class Particle { //properties of a spherical particle
public:
	double a; // radius
	double m; // mass
	Vec3D r;  // position
	Vec3D w;  // mid-step velocity
	int type;	// species of the particle
	
	Vec3D v;  // integer-step velocity
	Vec3D fC;  // force

	Vec3D fD;  // dissipative force
	Vec3D fR;  // random force

	Vec3D fCW; // wall force
	Vec3D fext; // wall force due to soft external potential

	double dens; // calculating the local density at a particle
	double dens_new; // calculating the local density at a particle
	double rhoBar;	 // calculating self energy per particle

};

#endif
