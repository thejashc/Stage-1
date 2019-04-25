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
	Vec3D fBody; // wall force due to soft external potential
	Vec3D fHarmonic; // spring force on the wall particles

	double dens; // calculating the local density at a particle
	double dens_new; // calculating the local density at a particle
	double rhoBar;	 // calculating self energy per particle
		
	Vec3D r0;  // initial position for wall particles
	Vec3D w_old;  // mid-step velocity

    unsigned int bondIndex[1000];
    double eqBondLength[1000];
    // double equilibriumBondLength[1000][3];
    // std::vector<Vec3D> equiBondLength;

    Vec3D rUnfolded;
    Vec3D dR;
};

#endif
