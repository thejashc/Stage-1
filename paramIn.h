simProg << " ********************* READING PARAMETERS FROM param.in  ************************* " << std::endl;	

std::string const in_fld1 = "param.in";
std::ifstream readParam (in_fld1.c_str());
if ( ! readParam  ) { simProg << "*** input file is empty" << std::endl; abort(); }

simProg << "Opening the file stream for param.in \n";

simProg << "----------------------------" << std::endl;
simProg << "Reading Simulation Box Size" << std::endl;
simProg << "---------------------------" << std::endl;

readParam >> buffer;                readParam.ignore(256,'\n');		
readParam >> buffer;                readParam.ignore(256,'\n');		
readParam >> buffer;                readParam.ignore(256,'\n');		

readParam >> buffer >> boxEdge[x];	readParam.ignore(256,'\n');		
readParam >> buffer >> boxEdge[y];	readParam.ignore(256,'\n');		
readParam >> buffer >> boxEdge[z];	readParam.ignore(256,'\n');     
readParam >> buffer >> initRho;		readParam.ignore(256,'\n');     

//readParam >> emptyLine ;		readParam.ignore(256,'\n');         

simProg << "----------------------------------" << std::endl;
simProg << "Reading Repulsion parameter Bij" << std::endl;
simProg << "----------------------------------" << std::endl;

readParam >> buffer;                readParam.ignore(256,'\n');		
readParam >> buffer;                readParam.ignore(256,'\n');		
readParam >> buffer;                readParam.ignore(256,'\n');		

readParam >> buffer >> Bs1_s1;	    readParam.ignore(256,'\n');     // Solid 1
readParam >> buffer >> Bs1_l1;      readParam.ignore(256,'\n');
readParam >> buffer >> Bs1_l2;      readParam.ignore(256,'\n');
readParam >> buffer >> Bs1_s2;      readParam.ignore(256,'\n');
readParam >> buffer >> Bs1_s3;      readParam.ignore(256,'\n');
readParam >> buffer >> Bs1_l3;      readParam.ignore(256,'\n');

readParam >> emptyLine ;		readParam.ignore(256,'\n');         

readParam >> buffer >> Bl1_l1;      readParam.ignore(256,'\n');     // Liquid 1
readParam >> buffer >> Bl1_l2;      readParam.ignore(256,'\n');
readParam >> buffer >> Bl1_s2;      readParam.ignore(256,'\n');
readParam >> buffer >> Bl1_s3;      readParam.ignore(256,'\n');
readParam >> buffer >> Bl1_l3;      readParam.ignore(256,'\n');

readParam >> emptyLine ;		readParam.ignore(256,'\n');         

readParam >> buffer >> Bl2_l2;      readParam.ignore(256,'\n');     // Liquid 2
readParam >> buffer >> Bl2_s2;      readParam.ignore(256,'\n');
readParam >> buffer >> Bl2_s3;      readParam.ignore(256,'\n');
readParam >> buffer >> Bl2_l3;      readParam.ignore(256,'\n');

readParam >> emptyLine ;		readParam.ignore(256,'\n');         

readParam >> buffer >> Bs2_s2;      readParam.ignore(256,'\n');     // Solid 2
readParam >> buffer >> Bs2_s3;      readParam.ignore(256,'\n');
readParam >> buffer >> Bs2_l3;      readParam.ignore(256,'\n');

readParam >> emptyLine ;		readParam.ignore(256,'\n');         

readParam >> buffer >> Bs3_s3;      readParam.ignore(256,'\n');     // Solid 3
readParam >> buffer >> Bs3_l3;      readParam.ignore(256,'\n');

readParam >> emptyLine ;		readParam.ignore(256,'\n');  

readParam >> buffer >> Bl3_l3;      readParam.ignore(256,'\n');     // Liquid 3

simProg << "----------------------------------" << std::endl;
simProg << "Reading Attraction parameter Aij" << std::endl;
simProg << "----------------------------------" << std::endl;

readParam >> buffer;                readParam.ignore(256,'\n');		
readParam >> buffer;                readParam.ignore(256,'\n');		
readParam >> buffer;                readParam.ignore(256,'\n');		

readParam >> buffer >> As1_s1;	    readParam.ignore(256,'\n');     // Solid 1
readParam >> buffer >> As1_l1;      readParam.ignore(256,'\n');
readParam >> buffer >> As1_l2;      readParam.ignore(256,'\n');
readParam >> buffer >> As1_s2;      readParam.ignore(256,'\n');
readParam >> buffer >> As1_s3;      readParam.ignore(256,'\n');
readParam >> buffer >> As1_l3;      readParam.ignore(256,'\n');

readParam >> emptyLine ;		readParam.ignore(256,'\n');         

readParam >> buffer >> Al1_l1;      readParam.ignore(256,'\n');     // Liquid 1
readParam >> buffer >> Al1_l2;      readParam.ignore(256,'\n');
readParam >> buffer >> Al1_s2;      readParam.ignore(256,'\n');
readParam >> buffer >> Al1_s3;      readParam.ignore(256,'\n');
readParam >> buffer >> Al1_l3;      readParam.ignore(256,'\n');

readParam >> emptyLine ;		readParam.ignore(256,'\n');         

readParam >> buffer >> Al2_l2;      readParam.ignore(256,'\n');     // Liquid 2
readParam >> buffer >> Al2_s2;      readParam.ignore(256,'\n');
readParam >> buffer >> Al2_s3;      readParam.ignore(256,'\n');
readParam >> buffer >> Al2_l3;      readParam.ignore(256,'\n');

readParam >> emptyLine ;		readParam.ignore(256,'\n');         

readParam >> buffer >> As2_s2;      readParam.ignore(256,'\n');     // Solid 2
readParam >> buffer >> As2_s3;      readParam.ignore(256,'\n');
readParam >> buffer >> As2_l3;      readParam.ignore(256,'\n');

readParam >> emptyLine ;		readParam.ignore(256,'\n');         

readParam >> buffer >> As3_s3;      readParam.ignore(256,'\n');     // Solid 3
readParam >> buffer >> As3_l3;      readParam.ignore(256,'\n');

readParam >> emptyLine ;		readParam.ignore(256,'\n');  

readParam >> buffer >> Al3_l3;      readParam.ignore(256,'\n');     // Liquid 3

#if RANDOM_DISSIPATIVE
    simProg << "----------------------------------" << std::endl;
    simProg << "Reading noise parameter : gamma_ij" << std::endl;
    simProg << "----------------------------------" << std::endl;

    readParam >> buffer;                readParam.ignore(256,'\n');    
    readParam >> buffer;                readParam.ignore(256,'\n');    
    readParam >> buffer;                readParam.ignore(256,'\n');    

    readParam >> buffer >> kBT;             readParam.ignore(256,'\n'); 

    readParam >> buffer >> noise_s1_s1;     readParam.ignore(256,'\n');     // Solid 1
    readParam >> buffer >> noise_s1_l1;      readParam.ignore(256,'\n');
    readParam >> buffer >> noise_s1_l2;      readParam.ignore(256,'\n');
    readParam >> buffer >> noise_s1_s2;      readParam.ignore(256,'\n');
    readParam >> buffer >> noise_s1_s3;      readParam.ignore(256,'\n');
    readParam >> buffer >> noise_s1_l3;      readParam.ignore(256,'\n');

    readParam >> emptyLine ;        readParam.ignore(256,'\n');    

    readParam >> buffer >> noise_l1_l1;      readParam.ignore(256,'\n');     // Liquid 1
    readParam >> buffer >> noise_l1_l2;      readParam.ignore(256,'\n');
    readParam >> buffer >> noise_l1_s2;      readParam.ignore(256,'\n');
    readParam >> buffer >> noise_l1_s3;      readParam.ignore(256,'\n');
    readParam >> buffer >> noise_l1_l3;      readParam.ignore(256,'\n');

    readParam >> emptyLine ;        readParam.ignore(256,'\n');    

    readParam >> buffer >> noise_l2_l2;      readParam.ignore(256,'\n');     // Liquid 2
    readParam >> buffer >> noise_l2_s2;      readParam.ignore(256,'\n');
    readParam >> buffer >> noise_l2_s3;      readParam.ignore(256,'\n');
    readParam >> buffer >> noise_l2_l3;      readParam.ignore(256,'\n');

    readParam >> emptyLine ;        readParam.ignore(256,'\n');    

    readParam >> buffer >> noise_s2_s2;      readParam.ignore(256,'\n');     // Solid 2
    readParam >> buffer >> noise_s2_s3;      readParam.ignore(256,'\n');
    readParam >> buffer >> noise_s2_l3;      readParam.ignore(256,'\n');

    readParam >> emptyLine ;        readParam.ignore(256,'\n');    

    readParam >> buffer >> noise_s3_s3;      readParam.ignore(256,'\n');     // Solid 3
    readParam >> buffer >> noise_s3_l3;      readParam.ignore(256,'\n');

    readParam >> emptyLine ;        readParam.ignore(256,'\n');  

    readParam >> buffer >> noise_l3_l3;      readParam.ignore(256,'\n');     // Liquid 3
#else
    readParam.ignore(256, '\n');
    readParam.ignore(256, '\n');    
    readParam.ignore(256, '\n');

    readParam.ignore(256, '\n');    // kbT

    readParam.ignore(256, '\n');
    readParam.ignore(256, '\n');
    readParam.ignore(256, '\n');
    readParam.ignore(256, '\n');
    readParam.ignore(256, '\n');
    readParam.ignore(256, '\n');

    readParam.ignore(256, '\n');    // empty line

    readParam.ignore(256, '\n');
    readParam.ignore(256, '\n');
    readParam.ignore(256, '\n');
    readParam.ignore(256, '\n');
    readParam.ignore(256, '\n');

    readParam.ignore(256, '\n');    /// empty line

    readParam.ignore(256, '\n');
    readParam.ignore(256, '\n');
    readParam.ignore(256, '\n');
    readParam.ignore(256, '\n');

    readParam.ignore(256, '\n');    // empty line

    readParam.ignore(256, '\n');
    readParam.ignore(256, '\n');
    readParam.ignore(256, '\n');

    readParam.ignore(256, '\n');    // empty line

    readParam.ignore(256, '\n');
    readParam.ignore(256, '\n');

    readParam.ignore(256, '\n');    // empty line

    readParam.ignore(256, '\n');
#endif  // RANDOM_DISSIPATIVE





#if WALL_ON
    simProg << "----------------------------------" << std::endl;
    simProg << "Reading Wall properties" << std::endl;
    simProg << "----------------------------------" << std::endl;

    readParam >> buffer;                readParam.ignore(256,'\n');		
    readParam >> buffer;                readParam.ignore(256,'\n');		
    readParam >> buffer;                readParam.ignore(256,'\n');		

    readParam >> buffer >> kWallBckg;       readParam.ignore(256,'\n');     
    readParam >> buffer >> kWallNgbr1;       readParam.ignore(256,'\n');     
    readParam >> buffer >> kWallNgbr2;       readParam.ignore(256,'\n');     
    readParam >> buffer >> wallHeight;	    readParam.ignore(256,'\n');		
#else
   	readParam.ignore(256,'\n');		    // 3 Lines for the heading
   	readParam.ignore(256,'\n');		
   	readParam.ignore(256,'\n');		

   	readParam.ignore(256,'\n');		    // 3 Lines for the 3 parameters
   	readParam.ignore(256,'\n');		    // 3 Lines for the 3 parameters
   	readParam.ignore(256,'\n');		
   	readParam.ignore(256,'\n');		
#endif  // WALL_ON

simProg << "----------------------------------" << std::endl;
simProg << "Reading WCA Interaction parameters" << std::endl;
simProg << "----------------------------------" << std::endl;

readParam >> buffer;                readParam.ignore(256,'\n');		
readParam >> buffer;                readParam.ignore(256,'\n');		
readParam >> buffer;                readParam.ignore(256,'\n');		

readParam >> buffer >> sigmaWCA;	readParam.ignore(256,'\n');		
readParam >> buffer >> epsilonWCA;	readParam.ignore(256,'\n');		

simProg << "----------------------------------" << std::endl;
simProg << "Reading Interaction cutoff's" << std::endl;
simProg << "----------------------------------" << std::endl;

readParam >> buffer;                readParam.ignore(256,'\n');		
readParam >> buffer;                readParam.ignore(256,'\n');		
readParam >> buffer;                readParam.ignore(256,'\n');		

readParam >> buffer >> rcutoff;         readParam.ignore(256,'\n');     
readParam >> buffer >> rd_cutoff;       readParam.ignore(256,'\n');     

simProg << "----------------------------------" << std::endl;
simProg << "Reading Simulation time parameters" << std::endl;
simProg << "----------------------------------" << std::endl;

readParam >> buffer;                readParam.ignore(256,'\n');		
readParam >> buffer;                readParam.ignore(256,'\n');		
readParam >> buffer;                readParam.ignore(256,'\n');		

readParam >> buffer >> dt;		        readParam.ignore(256,'\n');		
readParam >> buffer >> stepMax;		    readParam.ignore(256,'\n');		
readParam >> buffer >> rstrtFwrtFreq;	readParam.ignore(256,'\n');		
readParam >> buffer >> saveCount;	    readParam.ignore(256,'\n');		
readParam >> buffer >> psaveCount;	    readParam.ignore(256,'\n');		
readParam >> buffer >> pCorrTime;	    readParam.ignore(256,'\n');		

#if LEES_EDWARDS_BC
    simProg << "----------------------------------" << std::endl;
    simProg << "Reading LEES-EDWARDS SHEAR BC" << std::endl;
    simProg << "----------------------------------" << std::endl;

    readParam >> buffer;                readParam.ignore(256,'\n');		
    readParam >> buffer;                readParam.ignore(256,'\n');		
    readParam >> buffer;                readParam.ignore(256,'\n');		

    readParam >> buffer >> gammaDot;	readParam.ignore(256,'\n');	    
#else 
	readParam.ignore(256, '\n');    // 3 Lines for the Heading
	readParam.ignore(256, '\n');
	readParam.ignore(256, '\n');

	readParam.ignore(256, '\n');    // 4 Lines for the parameters
#endif  // LEES_EDWARDS_BC

#if SACF
    simProg << "----------------------------------" << std::endl;
    simProg << "Reading Stress Autocorrelation (SACF)" << std::endl;
    simProg << "----------------------------------" << std::endl;

    readParam >> buffer;                readParam.ignore(256,'\n');		
    readParam >> buffer;                readParam.ignore(256,'\n');		
    readParam >> buffer;                readParam.ignore(256,'\n');		

	readParam >> buffer >> n_vars;		readParam.ignore(256,'\n');		// L58
	readParam >> buffer >> corLevels;	readParam.ignore(256,'\n');		// L59
	readParam >> buffer >> pCorr;		readParam.ignore(256,'\n');		// L60
	readParam >> buffer >> mCorr;		readParam.ignore(256,'\n');		// L61
#else
	readParam.ignore(256, '\n');    // 3 Lines for the Heading
	readParam.ignore(256, '\n');
	readParam.ignore(256, '\n');

	readParam.ignore(256, '\n');    // 4 Lines for the parameters
	readParam.ignore(256, '\n');
	readParam.ignore(256, '\n');
	readParam.ignore(256, '\n');
#endif  // SACF

#if PLANAR_SLAB
    simProg << "----------------------------------" << std::endl;
    simProg << "Reading Geometry : PLANAR SLAB  " << std::endl;
    simProg << "----------------------------------" << std::endl;

    readParam >> buffer;                readParam.ignore(256,'\n');		
    readParam >> buffer;                readParam.ignore(256,'\n');		
    readParam >> buffer;                readParam.ignore(256,'\n');		

    readParam >> buffer >> slabWidth;	readParam.ignore(256,'\n');		    
#else
    readParam.ignore(256, '\n');    // 3 Lines for the heading
    readParam.ignore(256, '\n');		
    readParam.ignore(256, '\n');

    readParam.ignore(256, '\n');
#endif  // PLANAR_SLAB

#if SPHERICAL_DROPLET
    simProg << "----------------------------------" << std::endl;
    simProg << "Reading Geometry : SPHERICAL DROPLET  " << std::endl;
    simProg << "----------------------------------" << std::endl;

    readParam >> buffer;                readParam.ignore(256,'\n');		
    readParam >> buffer;                readParam.ignore(256,'\n');		
    readParam >> buffer;                readParam.ignore(256,'\n');		

    readParam >> buffer >> sphDropRad;	readParam.ignore(256,'\n');		    
#else
    readParam.ignore(256, '\n');    // 3 Lines for the heading
    readParam.ignore(256, '\n');		
    readParam.ignore(256, '\n');

    readParam.ignore(256, '\n');
#endif  // SPHERICAL_DROPLET

#if CYLINDRICAL_DROPLET
    simProg << "----------------------------------" << std::endl;
    simProg << "Reading Geometry : CYLINDRICAL DROPLET  " << std::endl;
    simProg << "----------------------------------" << std::endl;

    readParam >> buffer;                readParam.ignore(256,'\n');		
    readParam >> buffer;                readParam.ignore(256,'\n');		
    readParam >> buffer;                readParam.ignore(256,'\n');		

    readParam >> buffer >> cylRad;	readParam.ignore(256,'\n');		    
#else
    readParam.ignore(256, '\n');    // 3 Lines for the heading
    readParam.ignore(256, '\n');		
    readParam.ignore(256, '\n');

    readParam.ignore(256, '\n');
#endif  // CYLINDRICAL_DROPLET

#if CAPILLARY_CYLINDER || CAPILLARY_SQUARE || FCC_WALL 
    simProg << "----------------------------------" << std::endl;
    simProg << "Reading Geometry : CAPILLARY CYLINDER " << std::endl;
    simProg << "----------------------------------" << std::endl;

    readParam >> buffer;                readParam.ignore(256,'\n');		
    readParam >> buffer;                readParam.ignore(256,'\n');		
    readParam >> buffer;                readParam.ignore(256,'\n');		

	readParam >> buffer >> bufferLen;	readParam.ignore(256,'\n');		// L63 
	readParam >> buffer >> capLen;		readParam.ignore(256,'\n');		// L64 
	readParam >> buffer >> capRad;		readParam.ignore(256,'\n');		// L65 
	readParam >> buffer >> capWallWdth;	readParam.ignore(256,'\n');		// L66 
	readParam >> buffer >> resWdth;		readParam.ignore(256,'\n');		// L67 
	readParam >> buffer >> resCOMZ;		readParam.ignore(256,'\n');		// L68 
	readParam >> buffer >> resCOMVel;	readParam.ignore(256,'\n');		// L69 

#else
	readParam.ignore(256, '\n');
	readParam.ignore(256, '\n');
	readParam.ignore(256, '\n');

	readParam.ignore(256, '\n');
	readParam.ignore(256, '\n');
	readParam.ignore(256, '\n');
	readParam.ignore(256, '\n');
	readParam.ignore(256, '\n');
	readParam.ignore(256, '\n');
	readParam.ignore(256, '\n');
#endif // WALL_ON

#if PISTON
    simProg << "----------------------------------" << std::endl;
    simProg << "Reading Geometry : PISTON" << std::endl;
    simProg << "----------------------------------" << std::endl;

    readParam >> buffer;                readParam.ignore(256,'\n');		
    readParam >> buffer;                readParam.ignore(256,'\n');		
    readParam >> buffer;                readParam.ignore(256,'\n');		

    readParam >> buffer >> pistonZStart; readParam.ignore(256,'\n');		// L71
    readParam >> buffer >> pistonThickness; readParam.ignore(256,'\n');		// L71
    readParam >> buffer >> appPressure;	 readParam.ignore(256,'\n');		// L71
    readParam >> buffer >> pistonT0;	 readParam.ignore(256,'\n');		// L72
    readParam >> buffer >> pistonW;		 readParam.ignore(256,'\n');		// L73
#else 
    readParam.ignore(256, '\n');    // 3 Heading Lines
    readParam.ignore(256, '\n');
    readParam.ignore(256, '\n');

    readParam.ignore(256, '\n');    // 5 parameter lines
    readParam.ignore(256, '\n');
    readParam.ignore(256, '\n');
    readParam.ignore(256, '\n');
    readParam.ignore(256, '\n');
#endif // PISTON

#if HARD_SPHERES
    simProg << "----------------------------------" << std::endl;
    simProg << "Reading Geometry : SPHERICAL COLLOIDS" << std::endl;
    simProg << "----------------------------------" << std::endl;

    readParam >> buffer;                readParam.ignore(256,'\n');		
    readParam >> buffer;                readParam.ignore(256,'\n');		
    readParam >> buffer;                readParam.ignore(256,'\n');		

    readParam >> buffer >> NColloids;	    readParam.ignore(256,'\n');	
#else
    readParam.ignore(256,'\n');		
    readParam.ignore(256,'\n');		
    readParam.ignore(256,'\n');		

    readParam.ignore(256,'\n');		
#endif  // HARD_SPHERES

simProg << "Closing the file stream for param.in \n";

readParam.close();

// Half box size
boxHalve[x] 	= boxEdge[x] / 2.0;
boxHalve[y] 	= boxEdge[y] / 2.0;
boxHalve[z] 	= boxEdge[z] / 2.0;

// Reciprocal box size
boxRecip[x] 	= 1.0 / boxEdge[x];
boxRecip[y] 	= 1.0 / boxEdge[y];
boxRecip[z] 	= 1.0 / boxEdge[z];


#if RANDOM_DISSIPATIVE
    inv_sqrt_dt = 1.0 / std::sqrt(dt);                  // inverse of square root of the time step

    friction_s1_s1  = pow( noise_s1_s1, 2.0 )/( 2.0 * kBT );        // DPD dissipative force parameter
    friction_s1_l1  = pow( noise_s1_l1, 2.0 )/( 2.0 * kBT );    
    friction_s1_l2  = pow( noise_s1_l2, 2.0 )/( 2.0 * kBT );    
    friction_s1_s2  = pow( noise_s1_s2, 2.0 )/( 2.0 * kBT );    
    friction_s1_s3  = pow( noise_s1_s3, 2.0 )/( 2.0 * kBT );    
    friction_s1_l3  = pow( noise_s1_l3, 2.0 )/( 2.0 * kBT );    

    friction_l1_l1  = pow( noise_l1_l1, 2.0 )/( 2.0 * kBT );    
    friction_l1_l2  = pow( noise_l1_l2, 2.0 )/( 2.0 * kBT );    
    friction_l1_s2  = pow( noise_l1_s2, 2.0 )/( 2.0 * kBT );    
    friction_l1_s3  = pow( noise_l1_s3, 2.0 )/( 2.0 * kBT );    
    friction_l1_l3  = pow( noise_l1_l3, 2.0 )/( 2.0 * kBT );    

    friction_l2_l2  = pow( noise_l2_l2, 2.0 )/( 2.0 * kBT );    
    friction_l2_s2  = pow( noise_l2_s2, 2.0 )/( 2.0 * kBT );    
    friction_l2_s3  = pow( noise_l2_s3, 2.0 )/( 2.0 * kBT );    
    friction_l2_l3  = pow( noise_l2_l3, 2.0 )/( 2.0 * kBT );    

    friction_s2_s2  = pow( noise_s2_s2, 2.0 )/( 2.0 * kBT );    
    friction_s2_s3  = pow( noise_s2_s3, 2.0 )/( 2.0 * kBT );    
    friction_s2_l3  = pow( noise_s2_l3, 2.0 )/( 2.0 * kBT );    

    friction_s3_s3  = pow( noise_s3_s3, 2.0 )/( 2.0 * kBT );    
    friction_s3_l3  = pow( noise_s3_l3, 2.0 )/( 2.0 * kBT );    

    friction_l3_l3  = pow( noise_l3_l3, 2.0 )/( 2.0 * kBT );    

    noise_s1_s1     = noise_s1_s1 * inv_sqrt_dt;                    // Rescaled sigma
    noise_s1_l1     = noise_s1_l1 * inv_sqrt_dt;    
    noise_s1_l2     = noise_s1_l2 * inv_sqrt_dt;    
    noise_s1_s2     = noise_s1_s2 * inv_sqrt_dt;    
    noise_s1_s3     = noise_s1_s3 * inv_sqrt_dt;    
    noise_s1_l3     = noise_s1_l3 * inv_sqrt_dt;    

    noise_l1_l1     = noise_l1_l1 * inv_sqrt_dt;    
    noise_l1_l2     = noise_l1_l2 * inv_sqrt_dt;    
    noise_l1_s2     = noise_l1_s2 * inv_sqrt_dt;    
    noise_l1_s3     = noise_l1_s3 * inv_sqrt_dt;    
    noise_l1_l3     = noise_l1_l3 * inv_sqrt_dt;    

    noise_l2_l2     = noise_l2_l2 * inv_sqrt_dt;    
    noise_l2_s2     = noise_l2_s2 * inv_sqrt_dt;    
    noise_l2_s3     = noise_l2_s3 * inv_sqrt_dt;    
    noise_l2_l3     = noise_l2_l3 * inv_sqrt_dt;    

    noise_s2_s2     = noise_s2_s2 * inv_sqrt_dt;    
    noise_s2_s3     = noise_s2_s3 * inv_sqrt_dt;    
    noise_s2_l3     = noise_s2_l3 * inv_sqrt_dt;    

    noise_s3_s3     = noise_s3_s3 * inv_sqrt_dt;    
    noise_s3_l3     = noise_s3_l3 * inv_sqrt_dt;    

    noise_l3_l3     = noise_l3_l3 * inv_sqrt_dt;    
#endif // RANDOM_DISSIPATIVE

// Cutoff distances for liquid-liquid and solid-liquid interactions
fifteen_by_twopi_by_rd = 15.0/( 2.0 * M_PI * pow( rd_cutoff,3.0) );	// 15/(2*PI*rd^3) used in Lucy weight function
fifteen_by_twopi_by_rc = 15.0/( 2.0 * M_PI * pow( rcutoff,3.0 ) );	// 15/(2*PI*rc^3) used in Lucy weight function

rc2 = pow( rcutoff, 2. );
rd2 = pow( rd_cutoff, 2. );

sig2 = pow( sigmaWCA, 2.);
sig6 = pow( sig2, 3.);

twoPower1_3_sigma2 = pow( 2. , 1. / 3. ) * sig2;

rc2i = 1. / rc2;
rc6i = pow( rc2i, 3. );
ecutLJ  = 4. * 1.0 * sig6 * rc6i *( sig6*rc6i - 1.);
ecutWCA = -1.0; 

#if WALL_ON
	#if UPPER_WALL_ON && LOWER_WALL_ON
		wallLowPos = wallHeight;
		wallTopPos = boxEdge[z] - wallHeight;
	#elif LOWER_WALL_ON && !(UPPER_WALL_ON)
		wallLowPos = wallHeight;
	#elif CAPILLARY_CYLINDER || CAPILLARY_SQUARE
		capTubeStart = bufferLen;
		capTubeEnd   = capTubeStart + capLen;
		wallLowPos   = capTubeEnd + capWallWdth;
	#endif
    #if PISTON
        /*
        delOverTau = 1. / pistonW;
        expFactor = exp( -delOverTau );
        avgWindow = 10. * pistonW;
        */

        pistonArea 	= boxEdge[x] * boxEdge[y];	// Area over which the piston acts
        //vz0Pist		= 0.;
    #endif
#endif

#if LEES_EDWARDS_BC
    strainRate = gammaDot * boxEdge[y];
#endif

#if SACF
    pCorr2 = pCorr / 2;
#endif
