#ifndef _Particle_
#define _Particle_

#include "Vec3D.h"

//Declaration of particle class

class Particle { //properties of a spherical particle
public:
	double a; // radius
	double m; // mass
	Vec3D r;  // position
	Vec3D v;  // integer-step velocity
	Vec3D w;  // mid-step velocity
	Vec3D f;  // force
};

#endif
