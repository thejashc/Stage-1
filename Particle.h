#ifndef _Particle_
#define _Particle_

#include "Vec2D.h"

//Declaration of particle class

class Particle { //properties of a spherical particle
public:
	double a; //radius
	double m; //mass
	Vec2D r;  //position
	Vec2D w;  //mid-step velocity
	Vec2D v;  //integer-step velocity
	Vec2D f;  //force
};

#endif
