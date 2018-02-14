#include "DPD.h"

int main () {

	time_t tstart, tend;

	//define an instance of the DPD solver
	DPD coll2p;	

	#include "vars_initialize.h"
	
	// monitor total run time
	tstart = time(0);

	coll2p.solve();

	tend = time(0);

	// std::cout << "Total time for "<< coll2p.stepMax << " steps is: "<< difftime(tend, tstart) <<" second(s)."<< std::endl;
	std::fstream paraInfo;
  	paraInfo.open ("parainfo.txt", std::fstream::in | std::fstream::out | std::fstream::app);
	paraInfo << "Total Simulation time	" << "\t \t" <<  difftime(tend, tstart) << " seconds" << std::endl;
	paraInfo.close();

	return 0;
}
