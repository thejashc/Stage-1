// particles of class Particle
std::vector<Particle> particles;     	// vector of DPD particles
Particle *pointParticle;

// global parameters
double boxEdge[3];	double boxHalve[3];     double boxRecip[3];			// (1) box length in x,y,z directions, (2) half box-length in x,y,z directions, (3)	reciprocal box-length in x,y,z directions
double initRho;	double aCube;			// (1) initial placement density of atoms, (2) length of a side of cube corresponding to prescribed density			

// interaction paramters
double All1;				// DPD Warren conservative force -- attractive parameter
double All2;				// DPD Warren conservative force -- attractive parameter
double All12;
double Asl;
double Asl1;
double Asl2;        
double Ass;         double Aatt[3][3];
double repParam;    double Brep[3][3];                 // DPD Warren conservative force -- repulsive parameter
double rcutoff;		double rc2;		// (1) cut-off distance -- attractive force, (2) square of the cutoff distance -- attractive force
double rd_cutoff;	double rd2;		// (1) cut-off distance -- repulsive force, (2)  square of the cutoff distance -- repulsive force

// wall parameters
double kWall;
double wallPenetration;
double initWallRho;
double wallHeight;

// random-dissipative terms
double kBT;				// DPD fluid temperature
double noise, friction; std::vector<std::vector<double>> sigma, gamma; // noise and friction  
double noise2, friction2, noise12, friction12; // noise and friction for 2nd fluid if MC on
double dim;				// dimension of system

double dt, inv_sqrt_dt, half_dt, half_dt_sqr, sqrtTwelve;				// time step, sqrt of inverse of time step, half of dt, square of half of dt
int step, stepMax;				// counter for step, total number of steps
int rstrtFwrtFreq;			// The frequency of writing a restart file ( writes positions and mid-step velocities )
unsigned int saveCount;		// number of timestep between saves
unsigned int psaveCount;	// number of timestep between saves for pressure calculation

// Till Line 30 of paramIn.h

double dof;				// degrees of freedom
unsigned int pCount;

double pot_en;				// system potential energy
double kin_en;				// system kinetic energy
double vx2;				// square of the x-velocity
double vy2;				// square of the y-velocity
double vz2;				// square of the z-velocity
double vxvy;				// product of vx*vy
double v2;				// square of the velocity vector
double pair_pot_en;			// total pair potential energy
double tot_en;				// system total energy
double pressure;			// pressure
double pIdeal[3][3];			// Ideal component of pressure tensor
double pNonIdeal[3][3];			// Conservative - Interactions
double pNonIdealKin[3][3];		// Conservative - Kinetic
double pDissipative[3][3];		// Dissipative
double pRandom[3][3];			// Random
double pNonIdeal_temp[3][3];		// NonIdeal interactions
double pNonIdealKin_temp[3][3];		// NonIdeal Kinetic
double pDissipative_temp[3][3];		// Dissipative component of pressure tensor
double pRandom_temp[3][3];		// Random component of pressure tensor
unsigned int pTensCounter;
double idealComp;			// ideal component of pressure
double temp;				// temperature
double tempSum;				// temperature sum
double tempAv;				// temperature average
int tempCount;				// counting number of samples

double volume;				// system volume
int    npart;				// number of particles
double rho;				// density of system 

// init function
double xind_min;
double yind_min;
double zind_min;
double xind_max;
double yind_max;
double zind_max;

double xind;
double yind;
double zind;
double rand_gen_velx;
double rand_gen_vely;
double rand_gen_velz;

unsigned int fluidCount;	// number of fluid particles
std::vector<int> fluid_index;	// index of fluid type
std::vector<int> solid_index;	// index of solid type

// geometry initialization
#if SPHERICAL_DROPLET 
double dropBox;
#elif CYLINDER_DROPLET 
double cylCenterX;
double cylCenterY;
double cylCenterZ;
double cylRad;	
double cylHeight;
double xCOM;
double yCOM;
double zCOM;

unsigned int pCount;

#elif PLANAR_SLAB
double slabWidth;
double xCOM;
double yCOM;
double zCOM;

#endif

// Cell list variables
double rn;				// size of a cell in x, y, z directions
int Ncelx;				// number of cells in x
int Ncely;				// number of cells in y
int Ncelz;				// number of cells in z

unsigned int MaxPerCell = 100;		// maximum number of particles in a cell
const int x = 0;			// value for x, y, z co-ordinates -- never change this
const int y = 1;			
const int z = 2;
int NrCells[3];				// Number of cells in x, y and z direction	
int mi[3];				// index for particle i in any cell
int mj[3];				// index for particle j in any cell
int mini[3];				// minimum in the x,y,z directions
int maxi[3];				// maximum in the x,y,z directions
double scale[3];			// scale factor
int MaxNrCells;				// maximum cells among x, y, z
int mx;
int my;
int mz;

#if LEES_EDWARDS_BC
int dm_LEbc[16][3] = {	
	{  0,  0,  1 },			
	{  1,  0, -1 },                 
	{  1,  0,  0 },                 
	{  1,  0,  1 },                 
                                        
	{  0,  1, -1 },                 // Cells in the xy plane in ( z - 1 )
	{  0,  1, -1 },                                                      
	{  0,  1, -1 },                                                      
	{  0,  1, -1 },                                                      
                                                                             
	{  0,  1,  0 },                 // Cells in the xy plane in ( z + 0 )                                     
	{  0,  1,  0 },                 
	{  0,  1,  0 },                                                      
	{  0,  1,  0 },                                                      
                                                                             
	{  0,  1,  1 },                 // Cells in the xy plane in ( z + 1 )                                     
	{  0,  1,  1 },                                                      
	{  0,  1,  1 },
	{  0,  1,  1 }                  
};

double gammaDot;
double strain;

int dmCell1;
int dmCell2;
int dmCell3;
int dmCell4;

double zeroTol = 1e-7;
double strainRate;
double dissipativeWork;
double randomWork;

int i2;
int i3;
#endif

int dm[13][3] = {	
	{  0,  0,  1 },			
	{  1,  0, -1 },                 
	{  1,  0,  0 },                 
	{  1,  0,  1 },                 
                                        
	{ -1,  1, -1 },                 // Cells in the xy plane in ( z - 1 )
	{  0,  1, -1 },                                                      
	{  1,  1, -1 },                                                      
                                                                             
	{ -1,  1,  0 },                 // Cells in the xy plane in ( z + 0 )                                     
	{  0,  1,  0 },                 
	{  1,  1,  0 },                                                      
                                                                             
	{ -1,  1,  1 },                 // Cells in the xy plane in ( z + 1 )                                     
	{  0,  1,  1 },                                                      
	{  1,  1,  1 }                  
};

std::vector< std::vector < std::vector< std::vector<int> > > > grid;	// define grid as vectors
std::vector< std::vector <int> > periodN;				// determining the periodic neighbours
std::vector< std::vector <double> > periodR;				// determining the actual distance

// loop variables
int i;
int i1;
int j;
int j1;
int k;
int k1;
int m;
int ii;		
int jj;
double r2;
double dist;
double wC;
double wCij;
double wCij2;
double wDij;
double wDij2;
double rho_temp;
double rhoBartemp;
double term1;
double term2;
double term3;

// Vector 3D variables
Vec3D fCij;
Vec3D capRij;
Vec3D Rij;
Vec3D minRij;
Vec3D tempVec;
Vec3D dR;
Vec3D velCorr;

// dissipative and random forces
#if RANDOM_DISSIPATIVE
Vec3D fRij;
Vec3D fDij;
Vec3D wij;
Vec3D sumForce;

	#if MULTI_VISCOSITY_LIQUIDS

        unsigned int fluidType1_idxStart;
        unsigned int fluidType1_idxEnd;
        unsigned int fluidType2_idxStart;
        unsigned int fluidType2_idxEnd;


	#endif
double uniRand;
double thetaij;
double magRand;
double rDotv;
double magDiss;
	
#endif

// file writing parameters

// parameters for post-processing
// g(r) -- structure function
double gR_radMin;			// minimum radius for g(r)
double gR_radDelta;			// thickness of shell 
double gR_radMax;			// maximum radius for g(r)
int gR_nElem;				// number of elements in g(r)    
int gR_tStart;				// start time for measuring g(r)
int gR_tDelta;				// time between measurements g(r)
int gR_tEnd;				// end time for the measurement g(r)
int gR_tSamples;		 	// number of samples for g(r)	

std::vector<std::vector<double>> gR_nCount;// g(r) function

double ri;	// inner radius
double ro;	// outer radius
double rad;	// mean radius
int ig;		// index for r

// rhoZ calculation-- liquid density calculation
#if PLANAR_SLAB
	double rhoZ_Zmin;		// minimum Z for rhoZ
	double rhoZ_Zdelta;		// thickness of differential slab 
	double rhoZ_Zmax;		// maximum Z for rhoZ
	double vol;			// volume of differential slab
	
	int rhoZ_bins;			// number of bins for rhoZ
	
	std::vector<double> rhoZ;// keeps count of rhoZ at given Z
	
	int iRhoZ;			// index -- bookkeeping
	
	double Zpos;
	
	char filename[40];		// filename for data writing

#elif CYLINDER_DROPLET
	double rhor_rmin;
	double rhor_rdelta;
	double rhor_rmax;
	double vol;
	
	int rhor_bins;
	
	std::vector<double> rhor;
	
	int iRhor;
	double radPos;
	
	char filename[40];
#endif // PLANAR_SLAB

// defining wall
#if WALL_ON
	
	double capRad;
	double capRadSqr;	
	double strength;
	double capSphXc;
	double capSphYc;
	double capSphZc;
	
	double fSL;
	double w1P;
	double w2P;
	double root2;
	double asl;
	double Bsl;
	double fWallcutoff;
	
	Vec3D fCWij;
	Vec3D fCW;
	
	std::vector<std::vector<int>> nZ;
	int nZ_indz;
	int nZ_indx;
	double nZ_zbinWidth;
	double nZ_xbinWidth;
	int nZ_xbins;
	int nZ_zbins;
	
	double nZ_xMin;
	double nZ_xMax;
	double nZ_zMin;
	double nZ_zMax;
	
	double nZ_tStart;
	
	double segPlane_zMin;
	double segPlane_zMax;
	double segPlane_zDelta;
	int segPlane_bins;
	int segPlane_ind;

	double wallLowPos;
	double wallTopPos;
	double wallLowDist;
	double wallTopDist;
	double wallTemp;

	unsigned int solidCount;

	std::vector<double> segPlane_xCOM;
	std::vector<double> segPlane_zCOM;
	std::vector<int> segPlane_count;

    #if CYLINDER_ARRAY         
           double zOld;
           double tApp;
           double tSep;

           double shortestDist;
           Vec3D p0, p1, p2;
           Vec3D x012Cross;

           double Num;
           double Den;
           unsigned int resizeBoxSizeTo;

           Vec3D x01; Vec3D x02; Vec3D x21;

    #endif

	#if LOWER_WALL_ON
		double zindLW_min;
		double zindLW_max;
	
		unsigned int lwp;					// counts the number of lower wall particles
		
		double zOld;
		double tApp;
		double tSep;
	#endif
	#if UPPER_WALL_ON
		double zindUW_min;
		double zindUW_max;
		
		unsigned int uwp;					// counts the number of upper wall particles
	#endif

	#if CAPILLARY_CYLINDER
		double cylCenterX;
		double cylCenterY;
		double bufferLen;
		double capRad;
		double capLen;
		double capWallWdth;
		double resWdth;
		double capThick;

		bool innerRadius;					// to define the region within the inner cylinder
		bool outerRadius;
		bool inCapTube;
		bool notInPoreEntry;


		double capTubeStart;
		double capTubeEnd;
		double penDist;
		double perDist;

		double zOld;
		double tApp;
		double tSep;


		double rInner;
		double rOuter;
		#if PISTON
			unsigned int pistonParticles;

			double pistonStart;
			double pistonEnd;
			double forceOnPiston;
			double distInPiston;

			double pistonArea;

			double pistonT0;
			double pistonW; 

			double appForce;
			double appPressure;
			double vzPist;
			double vz0Pist;
			double drPist;
		#endif

		double BslMin;
		double BslMax;
		double BslW;
		double BslT0;
	#endif
	#if CAPILLARY_SQUARE
        double capEntrance_startIndex;
        double capEntrance_endIndex;
		double cylCenterX;
		double cylCenterY;
		double bufferLen;
		double capRad;
		double capLen;
		double capWallWdth;
		double resWdth;
		double capThick;

		bool particleInSquareSmall;
		bool particleInSquareLarge;
		bool outerRadius;
		bool inCapTube;
		bool notInPoreEntry;

		bool reg1, reg2, reg3, reg4;

		double sqEdge;
		double sqInnerEdgeXmin;
		double sqInnerEdgeXmax;
                double sqInnerEdgeYmin;
                double sqInnerEdgeYmax;

		double distInLeftWall; 	
                double distInRightWall; 
                double distInBottomWall;
                double distInTopWall;	

		double capTubeStart;
		double capTubeEnd;
		double penDist;
		double perDist;

		double zOld;
		double tApp;
		double tSep;


		double rInner;
		double rOuter;
        #if PISTON 
			unsigned int pistonParticles;

			double pistonStart;
			double pistonEnd;
			double forceOnPiston;
			double distInPiston;

			double pistonArea;

			double pistonT0;
			double pistonW; 

			double appForce;
			double appPressure;
			double vzPist;
			double vz0Pist;
			double drPist;

			double delForce;
		#endif

		double BslMin;
		double BslMax;
		double BslW;
		double BslT0;

		double sqXmin;
		double sqYmin;
		double sqXmax;
		double sqYmax;
	#endif

#endif // WALL_ON

#if BODY_FORCE
	double fBodyX;
#endif

#if SACF
	std::vector<std::vector<std::vector<double>>> aCorr;	// raw-data on which the correlation will be performed
	std::vector<std::vector<std::vector<double>>> fCorr;	// the correlated data
	std::vector<std::vector<std::vector<double>>> fCorrAv;	// the averaged correlated data -- used for writing data
	std::vector<std::vector<std::vector<double>>> nCorr;	// the number of samples over which the data is correlated 
	std::vector<std::vector<int>> pointCorr;		// points to the latest element added 
	std::vector<double> normalizeCorr;		// used for normalizing the first point
	std::vector<double> normalizeCorrAv;		// used for normalizing the first point -- average value -- used for writing data
	
	unsigned int sacpunt;
	unsigned int n_vars;	// number of elements over which the correlation should be considered 
	unsigned int n_vars_counter;	// number of elements over which the correlation should be considered 
	unsigned int corLevels; // number of levels in the multi-tau correlator
	unsigned int pCorr;	// number of blocks within a level
	unsigned int pCorr2;	// pCorr/2 
	unsigned int mCorr;	// number of blocks over data is averaged
	unsigned int point;	// pointer in the aCorr array
	unsigned int normalizeCorr_count;
	unsigned int nf1;
	unsigned int nf2;
	unsigned int nf3;
	unsigned int n_base;
	
	double Sxx;
	double SxyC;
	double SxyR;
	double SxyD;
	double SxyVxy;
	double Sxz;
	
	double Syx;
	double Syy;
	double Syz;
	
	double Szx;
	double Szy;
	double Szz;
	
	double Nxy;
	double Nyz;
	double Nzx;
#endif

// momentum calculation
long double momX;
long double momY;
long double momZ;

// velHist -- velocity distribution
double velHist_velMin;			// minimum radius for g(r)
double velHist_velDelta;		// thickness of shell 
double velHist_velMax;			// maximum radius for g(r)
int velHist_tStart;			// start time for measuring velHist
int velHist_tDelta;			// time between measurements velHist
int velHist_tEnd;			// end time for measurements velHist
int velHist_bins;			// number of bins for velHist

std::vector<int> velHistX;	// velHist vector for X component
std::vector<int> velHistY;	// velHist vector for Y component
std::vector<int> velHistZ;	// velHist vector for Z component

int ivelX;
int ivelY;
int ivelZ;

double trapzAreaX;
double trapzAreaY;
double trapzAreaZ;
double bin_lower;
double bin_upper;
double vel;

// constants
double one_by_pi              = 1.0/M_PI;
double five_by_pi             = 5.0/M_PI;		// 5/PI used in Lucy weight function
double thirty_by_pi           = 30.0/M_PI;		// 5/PI used in Lucy weight function
double neg_sixty_by_pi        = -(60.0/M_PI);		// 60/PI used in first derivative of Lucy weight function
double neg_onetwenty_by_pi    = 2.0*neg_sixty_by_pi;	// (120/PI) used in second derivative of Lucy weight function
double piThirty               = M_PI/30.;		// Pi/30.0 
double piFifteen              = M_PI/15.;		// Pi/15.0
double fifteen_by_twopi_by_rd;				// (15/(2*pi*rd^3))
double fifteen_by_twopi_by_rc;				// (15/(2*pi*rc^3))
double rd4;						// rd^4
double rc4;						// rc^4
double K1;
double K2;

//initialise time, and counter/ofstream for data output
unsigned int counter;
unsigned int pcounter;
unsigned int n; //mesh size
//define Cell as vector of Particle pointers
typedef std::vector<Particle*> Cell; 
std::vector<Cell> ll; //linked list is a 2D vector

// Gaussian random numbers
// random device class instance, source of 'true' randomness for initializing random seed
// std::default_random_engine seed;
std::random_device rdev{};
std::default_random_engine seed{rdev()};
#if RESTART
std::default_random_engine seedRstrt;
#endif

// Mersenne twister PRNG, initialized with seed from previous random device instance
// usage d{mean, std}
// std::normal_distribution<double> d{0,1};
std::uniform_real_distribution<double> randNumGen{0.0,1.0};

// File streams
std::string buffer;
std::string emptyLine;
	
std::ofstream simProg;
std::ofstream flagList;
