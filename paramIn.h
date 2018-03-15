simProg << " ********************* READING PARAMETERS ************************* " << std::endl;	

std::string const in_fld = "param.in";
std::ifstream readParam (in_fld.c_str());
if ( ! readParam  ) { simProg << "*** input file is empty" << std::endl; abort(); }

simProg << "Reading Box Dimensions" << std::endl;
readParam >> buffer >> boxEdge[x];	readParam.ignore(256,'\n');
readParam >> buffer >> boxEdge[y];	readParam.ignore(256,'\n');
readParam >> buffer >> boxEdge[z];	readParam.ignore(256,'\n');
readParam >> buffer >> initRho;		readParam.ignore(256,'\n');

// readParam >> buffer >> emptyLine;	readParam.ignore(256,'\n');

simProg << "Reading Liquid Liquid Interaction Parameters" << std::endl;
readParam >> buffer >> All;		readParam.ignore(256,'\n');
readParam >> buffer >> Bll;		readParam.ignore(256,'\n');
readParam >> buffer >> rcutoff;		readParam.ignore(256,'\n');
readParam >> buffer >> rd_cutoff;	readParam.ignore(256,'\n');

// readParam >> buffer >> emptyLine;	readParam.ignore(256,'\n');
#if WALL_ON

simProg << "Reading Solid Liquid Interaction Parameters" << std::endl;
readParam >> buffer >> Asl;		readParam.ignore(256,'\n');
readParam >> buffer >> Bsl;		readParam.ignore(256,'\n');
readParam >> buffer >> rcWcutoff;	readParam.ignore(256,'\n');
readParam >> buffer >> rdWcutoff;	readParam.ignore(256,'\n');

simProg << "Reading Solid geometrical Parameters" << std::endl;
readParam >> buffer >> wallHeight;	readParam.ignore(256,'\n');
readParam >> buffer >> initWallRho;	readParam.ignore(256,'\n');

#else
readParam.ignore(256, '\n');		// skip 8 lines 
readParam.ignore(256, '\n');		
readParam.ignore(256, '\n');
readParam.ignore(256, '\n');
readParam.ignore(256, '\n');
readParam.ignore(256, '\n');		
readParam.ignore(256, '\n');
readParam.ignore(256, '\n');
readParam.ignore(256, '\n');
#endif
//readParam >> buffer >> emptyLine;	readParam.ignore(256,'\n');

#if RANDOM_DISSIPATIVE
simProg << "Reading Random and Dissipative Parameters" << std::endl;
readParam >> buffer >> kBT;		readParam.ignore(256,'\n');
readParam >> buffer >> sigma;		readParam.ignore(256,'\n');
#else
readParam.ignore(256, '\n');		
readParam.ignore(256, '\n');
#endif
readParam >> buffer >> dim;		readParam.ignore(256,'\n');

//readParam >> buffer >> emptyLine;	readParam.ignore(256,'\n');

simProg << "Reading Time Parameters" << std::endl;
readParam >> buffer >> dt;		readParam.ignore(256,'\n');
readParam >> buffer >> stepMax;		readParam.ignore(256,'\n');
readParam >> buffer >> saveCount;	readParam.ignore(256,'\n');

//readParam >> buffer >> emptyLine;	readParam.ignore(256,'\n');

simProg << "Reading Structure Factor Parameters" << std::endl;
readParam >> buffer >> gR_radMin;	readParam.ignore(256,'\n');
readParam >> buffer >> gR_radDelta;	readParam.ignore(256,'\n');
readParam >> buffer >> gR_tStart;	readParam.ignore(256,'\n');
readParam >> buffer >> gR_tDelta;	readParam.ignore(256,'\n');

//readParam >> buffer >> emptyLine;	readParam.ignore(256,'\n');

simProg << "Reading Velocity Distribution Parameters" << std::endl;
readParam >> buffer >> velHist_velMin;		readParam.ignore(256,'\n');
readParam >> buffer >> velHist_velDelta;	readParam.ignore(256,'\n');
readParam >> buffer >> velHist_velMax;		readParam.ignore(256,'\n');
readParam >> buffer >> velHist_tStart;		readParam.ignore(256,'\n');
readParam >> buffer >> velHist_tDelta;		readParam.ignore(256,'\n');

//readParam >> buffer >> emptyLine;	readParam.ignore(256,'\n');
#if PLANAR_SLAB
simProg << "Reading Planar Slab Parameters" << std::endl;
readParam >> buffer >> slabWidth;	readParam.ignore(256,'\n');
readParam >> buffer >> rhoZ_Zmin;	readParam.ignore(256,'\n');
readParam >> buffer >> rhoZ_Zdelta;	readParam.ignore(256,'\n');
#if WALL_ON
	readParam >> buffer >> segPlane_zDelta;	readParam.ignore(256,'\n');
#endif
#else
#if WALL_ON
readParam.ignore(256, '\n');		
#endif
readParam.ignore(256, '\n');		// skip 4 lines -- one + three line
readParam.ignore(256, '\n');		
readParam.ignore(256, '\n');
readParam.ignore(256, '\n');
#endif

#if CYLINDER_DROPLET
simProg << "Reading Cylindrical Slab Parameters" << std::endl;
readParam >> buffer >> cylRad;		readParam.ignore(256,'\n');
readParam >> buffer >> cylHeight;	readParam.ignore(256,'\n');
readParam >> buffer >> rhor_rmin;	readParam.ignore(256,'\n');
readParam >> buffer >> rhor_rdelta;	readParam.ignore(256,'\n');
#else
readParam.ignore(256, '\n');		// skip two lines
readParam.ignore(256, '\n');
#endif 

readParam.close();

// Half box size
boxHalve[x] = boxEdge[x] / 2.0;
boxHalve[y] = boxEdge[y] / 2.0;
boxHalve[z] = boxEdge[z] / 2.0;

// Reciprocal box size
boxRecip[x] = 1.0 / boxEdge[x];
boxRecip[y] = 1.0 / boxEdge[y];
boxRecip[z] = 1.0 / boxEdge[z];

#if RANDOM_DISSIPATIVE
gamma = pow( sigma,2.0 )/( 2.0 * kBT ); // DPD dissipative force parameter
#endif

// Cutoff distances for liquid-liquid and solid-liquid interactions
fifteen_by_twopi_by_rd = 15.0/( 2.0 * M_PI * pow( rd_cutoff,3.0) );	// 15/(2*PI*rd^3) used in Lucy weight function
fifteen_by_twopi_by_rc = 15.0/( 2.0 * M_PI * pow( rcutoff,3.0 ) );	// 15/(2*PI*rc^3) used in Lucy weight function
rc2 = pow( rcutoff , 2);	// square of cut-off distance
rd2 = pow( rd_cutoff,2);	// square of cut-off distance

#if WALL_ON
rcW2 = pow( rcWcutoff , 2.);	// square of cut-off distance for solid liquid interactions -- attractive
rdW2 = pow( rdWcutoff , 2.);	// square of cut-off distance for solid-liquid interactions -- repulsive
#endif

//*****************************************************************************************//
//****************************** POST- PROCESSING *****************************************//
//*****************************************************************************************//
// g(r) calculation
gR_radMax = boxEdge[x] / 2.0;		// maximum radius for g(r)
gR_nElem  = round( ( gR_radMax - gR_radMin )/ gR_radDelta); // number of elements
gR_tEnd = stepMax;
gR_tSamples = round(( gR_tEnd - gR_tStart )/(gR_tDelta)) - 1;

// velocity distribution
velHist_bins  = round( ( velHist_velMax - velHist_velMin)/velHist_velDelta); // number of elements
velHist_tEnd = stepMax;

// density distribution along z
#if PLANAR_SLAB
rhoZ_Zmax 	= boxEdge[z];		// maximum Z
rhoZ_bins  	= round( ( rhoZ_Zmax - rhoZ_Zmin ) / rhoZ_Zdelta ); // number of elements
#if WALL_ON
segPlane_zMin = wallHeight;
segPlane_zMax = boxEdge[z] - wallHeight; 

segPlane_bins  = round( ( segPlane_zMax - segPlane_zMin )/ segPlane_zDelta); // number of elements
#endif
#elif CYLINDER_DROPLET
rhor_rmax 	= boxEdge[x];
rhor_bins  	= round( ( rhor_rmax - rhor_rmin ) / rhor_rdelta ); // number of elements
#endif
