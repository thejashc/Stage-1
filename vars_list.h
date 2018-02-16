// particles of class Particle
std::vector<Particle> particles;     	// vector of particles

// global parameters
double radSqr;				// square of the radius of the initial droplet
double box;				// size of domain
double boxEdge[3];			// box length in x,y,z directions
double boxHalve[3];			// box length in x,y,z directions
double boxRecip[3];			// box length in x,y,z directions
double kBT;				// DPD fluid temperature
double sigma;				// DPD noise level
double gamma;				// DPD dissipative force parameter
double aii;				// DPD conservative force parameter -- soft pair potential
// double aVdW;				// DPD conservative force -- many body force (attractive force)
// double bVdW;	                        // DPD conservative force -- many body force (excluded volume)
double Aij;				// DPD Warren conservative force -- attractive parameter
double Bij;                             // DPD Warren conservative force -- repulsive parameter
double kappa;				// DPD conservative force -- surface tension force
double rcutoff;				// cut-off distance -- attractive force
double rd_cutoff;			// cut-off distance -- repulsive force
double rc2;				// square of the cutoff distance -- attractive force
double rd2;				// square of the cutoff distance -- repulsive force
double dt;				// time step
double inv_sqrt_dt;			// inverse of square root of time step
double half_dt;				// 0.5*dt 
double half_dt_sqr;			// 0.5*dt*dt
double dim;				// dimension of system
double dof;				// degrees of freedom

int step;				// counter for number of steps 
double stepMax;				// total number of steps
double pot_en;				// system potential energy
double kin_en;				// system kinetic energy
double vsqrSum;				// square of the velocity
double pair_pot_en;			// total pair potential energy
double tot_en;				// system total energy
double pressure;			// pressure
double idealComp;			// ideal component of pressure
double thermProb;			// probability of thermostatting
double tau;				// rate of thermalizing 
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

int dm[13][3] = {	
	{  0,  0,  1 },
	{  1,  0, -1 },
	{  1,  0,  0 },
	{  1,  0,  1 },
	{ -1,  1, -1 },
	{ -1,  1,  0 },
	{ -1,  1,  1 },
	{  0,  1, -1 },
	{  0,  1,  0 },
	{  0,  1,  1 },
	{  1,  1, -1 },
	{  1,  1,  0 },
	{  1,  1,  1 } 
};

std::vector< std::vector < std::vector< std::vector<int> > > > grid;	// define grid as vectors
std::vector< std::vector <int> > periodN;				// determining the periodic neighbours
std::vector< std::vector <double> > periodR;				// determining the actual distance

// loop variables
int i;
int j;
int k;
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
Vec3D fRij;
Vec3D fDij;
Vec3D capRij;
Vec3D Rij;
Vec3D minRij;
Vec3D tempVec;
Vec3D dR;

// file writing parameters
unsigned int saveCount = 10000;		// number of timestep between saves

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

std::vector<double> gR_nCount;// g(r) function

double ri;	// inner radius
double ro;	// outer radius
double rad;	// mean radius
int ig;		// index for r

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
double one_by_pi = 1.0/M_PI;
double five_by_pi = 5.0/M_PI;				// 5/PI used in Lucy weight function
double thirty_by_pi = 30.0/M_PI;			// 5/PI used in Lucy weight function
double neg_sixty_by_pi = -(60.0/M_PI);			// 60/PI used in first derivative of Lucy weight function
double neg_onetwenty_by_pi = 2.0*neg_sixty_by_pi;	// (120/PI) used in second derivative of Lucy weight function
double fifteen_by_twopi_by_rd;				// (15/(2*pi*rd^3))
double fifteen_by_twopi_by_rc;				// (15/(2*pi*rc^3))
double rd4;						// rd^4
double piThirty;					// pi/30.0
double k1;
double k2;

//initialise time, and counter/ofstream for data output
unsigned int counter;
unsigned int n; //mesh size
//define Cell as vector of Particle pointers
typedef std::vector<Particle*> Cell; 
std::vector<Cell> ll; //linked list is a 2D vector

// Gaussian random numbers
// random device class instance, source of 'true' randomness for initializing random seed
std::default_random_engine rd;
// Mersenne twister PRNG, initialized with seed from previous random device instance
// usage d{mean, std}
// std::normal_distribution<double> d{0,1};
std::uniform_real_distribution<double> d{0.0,1.0};
