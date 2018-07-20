simProg << " ********************* READING PARAMETERS ************************* " << std::endl;	

std::string const in_fld = "param.in";
std::ifstream readParam (in_fld.c_str());
if ( ! readParam  ) { simProg << "*** input file is empty" << std::endl; abort(); }

simProg << "Reading Box Dimensions" << std::endl;
readParam >> buffer >> boxEdge[x];	readParam.ignore(256,'\n');		// L1
readParam >> buffer >> boxEdge[y];	readParam.ignore(256,'\n');		// L2
readParam >> buffer >> boxEdge[z];	readParam.ignore(256,'\n');		// L3
readParam >> buffer >> initRho;		readParam.ignore(256,'\n');		// L4

readParam >> emptyLine ;		readParam.ignore(256,'\n');		// L5

simProg << "Reading Liquid Liquid Interaction Parameters" << std::endl;
readParam >> buffer >> All;		readParam.ignore(256,'\n');		// L6
readParam >> buffer >> Bll;		readParam.ignore(256,'\n');		// L7
readParam >> buffer >> rcutoff;		readParam.ignore(256,'\n');		// L8
readParam >> buffer >> rd_cutoff;	readParam.ignore(256,'\n');		// L9
	
readParam >> emptyLine ;		readParam.ignore(256,'\n');		// L10
#if WALL_ON
simProg << "Reading Solid Solid Interaction Parameters" << std::endl;
readParam >> buffer >> Ass;		readParam.ignore(256,'\n');		// L11
readParam >> buffer >> Bss;		readParam.ignore(256,'\n');		// L12
readParam >> buffer >> kWall;		readParam.ignore(256,'\n');		// L13

readParam >> emptyLine ;		readParam.ignore(256,'\n');		// L14

simProg << "Reading Solid Liquid Interaction Parameters" << std::endl;
readParam >> buffer >> asl;		readParam.ignore(256,'\n');		// L15
readParam >> buffer >> Bsl;		readParam.ignore(256,'\n');		// L16
readParam >> buffer >> Brep;		readParam.ignore(256,'\n');		// L17
readParam >> buffer >> wallPenetration;	readParam.ignore(256,'\n');		// L18
readParam >> buffer >> rcWcutoff;	readParam.ignore(256,'\n');		// L19
readParam >> buffer >> rdWcutoff;	readParam.ignore(256,'\n');		// L20

readParam >> emptyLine ;		readParam.ignore(256,'\n');		// L21

simProg << "Reading Solid geometrical Parameters" << std::endl;
readParam >> buffer >> wallHeight;	readParam.ignore(256,'\n');		// L22
readParam >> buffer >> initWallRho;	readParam.ignore(256,'\n');		// L23

#else					
readParam.ignore(256, '\n');
readParam.ignore(256, '\n');		
readParam.ignore(256, '\n');		
readParam >> emptyLine;			readParam.ignore(256,'\n');
readParam.ignore(256, '\n');
readParam.ignore(256, '\n');		
readParam.ignore(256, '\n');		
readParam.ignore(256, '\n');		
readParam.ignore(256, '\n');		
readParam.ignore(256, '\n');		
readParam >> emptyLine;			readParam.ignore(256,'\n');
readParam.ignore(256, '\n');
readParam.ignore(256, '\n');
#endif
#if BODY_FORCE 
readParam >> buffer >> fBodyX;		readParam.ignore(256,'\n');		// L24
#else
readParam.ignore(256, '\n');
#endif
readParam >> emptyLine;			readParam.ignore(256,'\n');		// L25

#if RANDOM_DISSIPATIVE
simProg << "Reading Random and Dissipative Parameters" << std::endl;
readParam >> buffer >> kBT;		readParam.ignore(256,'\n');		// L26
readParam >> buffer >> sigma;		readParam.ignore(256,'\n');		// L27
#else
readParam.ignore(256, '\n');		
readParam.ignore(256, '\n');
#endif
readParam >> buffer >> dim;		readParam.ignore(256,'\n');		// L28

readParam >> emptyLine;			readParam.ignore(256,'\n');		// L29

simProg << "Reading Time Parameters" << std::endl;
readParam >> buffer >> dt;		readParam.ignore(256,'\n');		// L30
readParam >> buffer >> stepMax;		readParam.ignore(256,'\n');		// L31
readParam >> buffer >> rstrtFwrtFreq;	readParam.ignore(256,'\n');		// L32
readParam >> buffer >> saveCount;	readParam.ignore(256,'\n');		// L33
readParam >> buffer >> psaveCount;	readParam.ignore(256,'\n');		// L34

readParam >>  emptyLine;		readParam.ignore(256,'\n');		// L35

simProg << "Reading Structure Factor Parameters" << std::endl;
readParam >> buffer >> gR_radMin;	readParam.ignore(256,'\n');		// L36
readParam >> buffer >> gR_radDelta;	readParam.ignore(256,'\n');		// L37
readParam >> buffer >> gR_tStart;	readParam.ignore(256,'\n');		// L38
readParam >> buffer >> gR_tDelta;	readParam.ignore(256,'\n');		// L39

readParam >> emptyLine;			readParam.ignore(256,'\n');		// L40

simProg << "Reading Velocity Distribution Parameters" << std::endl;
readParam >> buffer >> velHist_velMin;		readParam.ignore(256,'\n');	// L41
readParam >> buffer >> velHist_velDelta;	readParam.ignore(256,'\n');	// L42
readParam >> buffer >> velHist_velMax;		readParam.ignore(256,'\n');	// L43
readParam >> buffer >> velHist_tStart;		readParam.ignore(256,'\n');	// L44
readParam >> buffer >> velHist_tDelta;		readParam.ignore(256,'\n');	// L45

readParam >> emptyLine;			readParam.ignore(256,'\n');		// L46
#if PLANAR_SLAB
simProg << "Reading Planar Slab Parameters" << std::endl;
readParam >> buffer >> slabWidth;	readParam.ignore(256,'\n');		// L47
readParam >> buffer >> rhoZ_Zmin;	readParam.ignore(256,'\n');		// L48
readParam >> buffer >> rhoZ_Zdelta;	readParam.ignore(256,'\n');		// L49
#if WALL_ON
	readParam >> buffer >> segPlane_zDelta;	readParam.ignore(256,'\n');	// L50
#else
	readParam.ignore(256, '\n');
#endif
#else
readParam.ignore(256, '\n');
readParam.ignore(256, '\n');		
readParam.ignore(256, '\n');
readParam.ignore(256, '\n');
#endif

readParam >> emptyLine;			readParam.ignore(256,'\n');		// L51
#if CYLINDER_DROPLET
simProg << "Reading Cylindrical Slab Parameters" << std::endl;
readParam >> buffer >> cylRad;		readParam.ignore(256,'\n');		// L52
readParam >> buffer >> cylHeight;	readParam.ignore(256,'\n');		// L53
readParam >> buffer >> rhor_rmin;	readParam.ignore(256,'\n');		// L54
readParam >> buffer >> rhor_rdelta;	readParam.ignore(256,'\n');		// L55
#else
readParam.ignore(256, '\n');
readParam.ignore(256, '\n');
readParam.ignore(256, '\n');
readParam.ignore(256, '\n');
#endif 

readParam >> emptyLine;			readParam.ignore(256,'\n');		// L56
#if LEES_EDWARDS_BC
	simProg << "Reading homogeneous-shear parameters (Lees-Edwards)" << std::endl;
	readParam >> buffer >> gammaDot;	readParam.ignore(256,'\n');	// L57
#else
	readParam.ignore(256, '\n');
#endif // LEES_EDWARDS_BC
readParam >> emptyLine;			readParam.ignore(256,'\n');		// L58

#if SACF
	simProg << "Reading parameters for the calculation of stress-autocorrelation" << std::endl;	
	readParam >> buffer >> n_vars;		readParam.ignore(256,'\n');		// L59
	readParam >> buffer >> corLevels;	readParam.ignore(256,'\n');		// L60
	readParam >> buffer >> pCorr;		readParam.ignore(256,'\n');		// L61
	readParam >> buffer >> mCorr;		readParam.ignore(256,'\n');		// L62
#else
	readParam.ignore(256, '\n');
	readParam.ignore(256, '\n');
	readParam.ignore(256, '\n');
	readParam.ignore(256, '\n');
#endif
readParam >> emptyLine;			readParam.ignore(256,'\n');			// L63

#if CAPILLARY_TUBE
	simProg << "Reading parameters for the capillary tube" << std::endl;	
	readParam >> buffer >> bufferLen;	readParam.ignore(256,'\n');		// L64 
	readParam >> buffer >> capLen;		readParam.ignore(256,'\n');		// L65 
	readParam >> buffer >> capRad;		readParam.ignore(256,'\n');		// L66 
	readParam >> buffer >> capWallWdth;	readParam.ignore(256,'\n');		// L67 
	readParam >> buffer >> resWdth;		readParam.ignore(256,'\n');		// L68 
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

	#if UPPER_WALL_ON && LOWER_WALL_ON
		wallLowPos = wallHeight;
		wallTopPos = boxEdge[z] - wallHeight;
	#elif LOWER_WALL_ON && !(UPPER_WALL_ON)
		wallLowPos = wallHeight;
	#elif CAPILLARY_TUBE
		capTubeStart = bufferLen;
		capTubeEnd   = capTubeStart + capLen;
		wallLowPos   = capTubeEnd + capWallWdth;

		#if PISTON
			pistonStart	= wallLowPos + resWdth; 
			pistonEnd 	= pistonStart + capWallWdth;	// thickness of piston is same as that of the wall adjacent to capillary
		#endif
	#endif
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
/*
// COM approach
segPlane_zMin = wallHeight;
segPlane_zMax = boxEdge[z] - wallHeight; 

segPlane_bins  = round( ( segPlane_zMax - segPlane_zMin )/ segPlane_zDelta); // number of elements
*/
// binning approach
nZ_tStart = 5e3;

nZ_xbinWidth = 0.1;
nZ_zbinWidth = 0.05; 
nZ_xMin = 0.;
nZ_xMax = boxEdge[x];

nZ_zMin = 0;
nZ_zMax = boxEdge[z];

nZ_xbins = round( ( nZ_xMax - nZ_xMin ) / nZ_xbinWidth );
nZ_zbins = round( ( nZ_zMax - nZ_zMin)  / nZ_zbinWidth );

#endif
#elif CYLINDER_DROPLET
rhor_rmax 	= boxEdge[x];
rhor_bins  	= round( ( rhor_rmax - rhor_rmin ) / rhor_rdelta ); // number of elements
#endif

#if LEES_EDWARDS_BC
strainRate = gammaDot * boxEdge[y];
#endif

#if SACF
pCorr2 = pCorr / 2;
#endif
