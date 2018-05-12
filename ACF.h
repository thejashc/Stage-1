for ( n_vars_counter=0 ; n_vars_counter<n_vars ; ++n_vars_counter ){
	// std::cout << "step= " << step  << std::endl;
	recursive_addCorr(xind, n_vars_counter, 1);		// send data to the type-i correlator at level=0
	// std::cout << "n_vars_counter= " << n_vars_counter << " done" << std::endl; 
	// std::cout << std::endl;
}

normalizeCorr += xind * xind;
