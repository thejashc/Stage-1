#include "DPD.h"

int main () {

	time_t tstart, tend;
	//define an instance of the DPD solver
	DPD coll2p;	

	// set global parameters
	coll2p.box = 10.0;			// dimension of box
	coll2p.epsilon = 100.0;			// energy well depth
	coll2p.sigma = 1.0;			// zero-potential distance
	coll2p.aii = 25.0;			// DPD interaction parameter
	coll2p.rcutoff = 1.0;			// cut-off distance
	coll2p.rc2 = pow(coll2p.rcutoff,2);	// square of cut-off distance
	coll2p.dim = 3;				// 3D system

	coll2p.tau = 0.745;			// rate of thermalizing
	coll2p.dt = 4e-2;
	coll2p.step = 1;
	coll2p.stepMax = 5e5;
	coll2p.kBT = 1.0;			// Boltzmann constant
	coll2p.thermProb = coll2p.dt*coll2p.tau;// probability of thermalizing 	

	// Cell list parameters
	coll2p.rn = coll2p.box/(round(coll2p.box/coll2p.rcutoff)); // size of cell
	coll2p.Ncelx = round(coll2p.box/coll2p.rn);		   // no of cells in a dirn
	coll2p.Ncely = round(coll2p.box/coll2p.rn);		   // no of cells in a dirn
	coll2p.Ncelz = round(coll2p.box/coll2p.rn);		   // no of cells in a dirn
	
	// post-processing
	// g(r) calculation
	coll2p.gR_radMin = 0.0;			// minimum radius for g(r) 
	coll2p.gR_radDelta = 0.01;		// thickness of a shell
	coll2p.gR_radMax = coll2p.box/2.0;	// maximum radius for g(r)
	coll2p.gR_nElem  = round((coll2p.gR_radMax - coll2p.gR_radMin)/coll2p.gR_radDelta); // number of elements

	coll2p.gR_tStart = 5e4;
	coll2p.gR_tDelta = 100;
	coll2p.gR_tEnd = coll2p.stepMax;
	coll2p.gR_tSamples = round((coll2p.gR_tEnd - coll2p.gR_tStart)/(coll2p.gR_tDelta)) - 1;

	// velocity distribution
	coll2p.velHist_velMin = -25.0;			// minimum velocity 
	coll2p.velHist_velDelta = 0.005;		// resolution of DeltaVel
	coll2p.velHist_velMax = 25.0;			// maximum velocity
	coll2p.velHist_bins  = round((coll2p.velHist_velMax - coll2p.velHist_velMin)/coll2p.velHist_velDelta); // number of elements

	coll2p.velHist_tStart = 5e4;
	coll2p.velHist_tDelta = 100;
	coll2p.velHist_tEnd = coll2p.stepMax;

	// Initialization
	// coll2p.init();						// initialize particle positions and velocity

	// monitor total run time
	tstart = time(0);
	//start the DPD solver
	coll2p.solve();

	tend = time(0);

	std::cout << "Total time for "<< coll2p.stepMax << " steps is: "<< difftime(tend, tstart) <<" second(s)."<< std::endl;

	return 0;
}
