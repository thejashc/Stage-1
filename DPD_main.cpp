#include "DPD.h"

int main () {

	time_t tstart, tend;
	//define an instance of the DPD solver
	DPD coll2p;	

	// set global parameters
	coll2p.box = 10.0;			// dimension of box
	coll2p.kBT = 1.0;			// DPD fluid temperature 
	// coll2p.aii = 00.0;			// DPD conservative force -- soft repulsive force
	// coll2p.bVdW = 0.016;			// DPD conservative force -- many body force
	// coll2p.aVdW = 1.9*coll2p.bVdW;	// DPD conservative force -- many body force
	coll2p.Aij = -40.0;			// DPD Warren conservative force -- attractive parameter
	coll2p.Bij = +40.0;			// DPD Warren conservative force -- repulsive parameter
	coll2p.kappa = 8e-4;			// DPD conservative force -- surface tension force
	coll2p.sigma = 1.0;			// DPD random force parameter
	coll2p.gamma = pow(coll2p.sigma,2.0)/(2.0*coll2p.kBT); // DPD dissipative force parameter
	coll2p.rcutoff = 1.0;			// cut-off distance for pairwise attraction
	coll2p.rd_cutoff = 0.75;			// cut-off distance for pairwise repulsion
	coll2p.fifteen_by_twopi_by_rd = 15.0/(2.0*M_PI*pow(coll2p.rd_cutoff,3.0));	// 15/(2*PI*rd^3) used in Lucy weight function
	coll2p.fifteen_by_twopi_by_rc = 15.0/(2.0*M_PI*pow(coll2p.rcutoff,3.0));	// 15/(2*PI*rc^3) used in Lucy weight function
	coll2p.rc2 = pow(coll2p.rcutoff,2);	// square of cut-off distance
	coll2p.rd2 = pow(coll2p.rd_cutoff,2);	// square of cut-off distance
	coll2p.dim = 3;				// 3D system

	coll2p.tau = 0.745;			// rate of thermalizing
	coll2p.dt = 1e-3;
	coll2p.step = 1;
	coll2p.stepMax = 2e5;
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

	// std::cout << "Total time for "<< coll2p.stepMax << " steps is: "<< difftime(tend, tstart) <<" second(s)."<< std::endl;
	std::fstream paraInfo;
  	paraInfo.open ("parainfo.txt", std::fstream::in | std::fstream::out | std::fstream::app);
	paraInfo << "Total Simulation time	" << "\t \t" <<  difftime(tend, tstart) << "seconds" << std::endl;
	paraInfo.close();

	return 0;
}
