#include "DPD.h"

int main () {

	time_t tstart, tend;
	//define an instance of the DPD solver
	DPD coll2p;	

	// set global parameters
	coll2p.box = 10.0;			// dimension of box
	coll2p.epsilon = 1.0;			// energy well depth
	coll2p.sigma = 1.0;			// zero-potential distance
	coll2p.rcutoff = 3.0;			// cut-off distance
	coll2p.rc2 = pow(coll2p.rcutoff,2);	// square of cut-off distance

	coll2p.dt = 1e-5;
	coll2p.step = 1;
	coll2p.stepMax = 2e5;

	// cut-off correction to potential
	coll2p.sig6 = pow(coll2p.sigma,6);
	coll2p.rc2i = 1/coll2p.rc2;
	coll2p.rc6i = pow(coll2p.rc2i,3);
	coll2p.ecut = 4*coll2p.epsilon*coll2p.sig6*coll2p.rc6i*(coll2p.sig6*coll2p.rc6i - 1.0);
	
	// post-processing
	coll2p.gR_radMin = 0.5;			// minimum radius for g(r) 
	coll2p.gR_radDelta = 0.25;		// thickness of a shell
	coll2p.gR_radMax = coll2p.box/2.0;		// maximum radius for g(r)
	coll2p.gR_nElem  = round((coll2p.gR_radMax - coll2p.gR_radMin)/coll2p.gR_radDelta); // number of elements

	coll2p.gR_tStart = 4e4;
	coll2p.gR_tDelta = 100;
	coll2p.gR_tEnd = coll2p.stepMax;
	coll2p.gR_tSamples = round((coll2p.gR_tEnd - coll2p.gR_tStart)/(coll2p.gR_tDelta)) - 1;

	// Initialization
	coll2p.init();						// initialize particle positions and velocity

	// monitor total run time
	tstart = time(0);
	//start the DPD solver
	coll2p.solve();

	tend = time(0);

	std::cout << "Total time for "<< coll2p.stepMax << " steps is: "<< difftime(tend, tstart) <<" second(s)."<< std::endl;

	return 0;
}
