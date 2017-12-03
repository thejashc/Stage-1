#include "DPD.h"

int main () {
	//define an instance of the DPD solver
	DPD coll2p;	

	// set global parameters
	coll2p.box = 10;
	coll2p.epsilon = 1.0;				
	coll2p.sigma = 1.0;			
	coll2p.rcutoff = 3.0;				
	coll2p.rc2 = pow(coll2p.rcutoff,2);		

	coll2p.dt = 1e-5;
	coll2p.step = 1;
	coll2p.stepMax = 1e5;

	//initialize two particles with mass, position, velocity
	double xind_min = -1.0*(coll2p.box/2.0) + 1;
	double yind_min = -1.0*(coll2p.box/2.0) + 1;
	double xind_max =  1.0*(coll2p.box/2.0);
	double yind_max =  1.0*(coll2p.box/2.0);

	double xind = xind_min;

	while ( xind < xind_max){
		double yind = yind_min;
		while( yind < yind_max){
			coll2p.particles.push_back({1.0,1.0,{xind, yind},{0.0, 0.0}});

			// update yind
			yind += 1.0;
		}			
		// update xind
		xind += 1.0;
	}

	//coll2p.initialize_position();

	//start the DPD solver
	coll2p.solve();

	return 0;
}
