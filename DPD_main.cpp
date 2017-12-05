#include "DPD.h"

int main () {
	
	time_t tstart, tend;
	//define an instance of the DPD solver
	DPD coll2p;	

	// set global parameters
	coll2p.box = 6.0;
	coll2p.epsilon = 1.0;				
	coll2p.sigma = 1.0;			
	coll2p.rcutoff = 3.0;				
	coll2p.rc2 = pow(coll2p.rcutoff,2);		

	coll2p.dt = 1e-5;
	coll2p.step = 1;
	coll2p.stepMax = 5e5;

	//initialize two particles with mass, position, velocity
	double xind_min = -1.0*(coll2p.box/2.0) + 1.0;
	double yind_min = -1.0*(coll2p.box/2.0) + 1.0;
	double zind_min = -1.0*(coll2p.box/2.0) + 1.0;
	double xind_max =  1.0*(coll2p.box/2.0);
	double yind_max =  1.0*(coll2p.box/2.0);
	double zind_max =  1.0*(coll2p.box/2.0);

	double xind = xind_min;
	// initialization
	while ( xind < xind_max){
		double yind = yind_min;
		while( yind < yind_max){
			double zind = zind_min;
				while( zind < zind_max){
					// generate random velocities
					double rand_gen_velx = ((double) rand() / (RAND_MAX));
					double rand_gen_vely = ((double) rand() / (RAND_MAX));
					double rand_gen_velz = ((double) rand() / (RAND_MAX));

					// initializing particle mass, radius, position and mid-step-velocity
					coll2p.particles.push_back({0.2,1.0,{xind, yind, zind},{rand_gen_velx, rand_gen_vely, rand_gen_velz}});

					// update zind
					zind += 1.0;

				}// end of zind
				yind += 1.0;
		}// end of yind			
		xind += 1.0;
	}// end of xind

	// monitor total run time
	tstart = time(0);
	//start the DPD solver
	coll2p.solve();

	tend = time(0);

	std::cout << "Total time for "<< coll2p.stepMax << " steps is: "<< difftime(tend, tstart) <<" second(s)."<< std::endl;

	return 0;
}
