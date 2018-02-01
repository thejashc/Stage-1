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
	Vec3D v;  // integer-step velocity
	Vec3D fC;  // force
	Vec3D fD;  // dissipative force
	Vec3D fR;  // random force
	double dens; // calculating the local density at a particle
	double dens_new; // calculating the local density at a particle
};

#endif
