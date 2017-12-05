#include "DPD.h"

int main () {
	
	time_t tstart, tend;
	//define an instance of the DPD solver
	DPD coll2p;	

	// set global parameters
	coll2p.box = 6.0;			// dimension of box
	coll2p.epsilon = 1.0;			// energy well depth
	coll2p.sigma = 1.0;			// zero-potential distance
	coll2p.rcutoff = 3.0;			// cut-off distance
	coll2p.rc2 = pow(coll2p.rcutoff,2);	// square of cut-off distance

	coll2p.dt = 1e-5;
	coll2p.step = 1;
	coll2p.stepMax = 5e5;

	// Initialization
	coll2p.init();

	// monitor total run time
	tstart = time(0);
	//start the DPD solver
	coll2p.solve();

	tend = time(0);

	std::cout << "Total time for "<< coll2p.stepMax << " steps is: "<< difftime(tend, tstart) <<" second(s)."<< std::endl;

	return 0;
}
