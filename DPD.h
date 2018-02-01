#ifndef _DPD_
#define _DPD_

#include <vector>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdlib>
#include "Particle.h"
#include <ctime>
#include <random> 

//declare DPD solver
class DPD {
	public:
		// global parameters
		double radSqr;				// square of the radius of the initial droplet
		double box;				// size of domain
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
		double tot_en;				// system total energy
		double pressure;			// pressure
		double thermProb;			// probability of thermostatting
		double tau;				// rate of thermalizing 
		double temp;				// temperature
		double tempSum;				// temperature sum
		double tempAv;				// temperature average
		int tempCount;				// counting number of samples

		double volume;				// system volume
		double npart;				// number of particles
		double rho;				// density of system 

		// Cell list variables
		double rn;				// size of a cell in x, y, z directions
		int Ncelx;				// number of cells in x
		int Ncely;				// number of cells in y
		int Ncelz;				// number of cells in z

		// file writing parameters
		unsigned int saveCount = 1000;		// number of timestep between saves

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

		std::vector<Particle> particles;     	// vector of particles

		// constants
		double one_by_pi = 1.0/M_PI;
		double five_by_pi = 5.0/M_PI;				// 5/PI used in Lucy weight function
		double thirty_by_pi = 30.0/M_PI;			// 5/PI used in Lucy weight function
		double neg_sixty_by_pi = -(60.0/M_PI);			// 60/PI used in first derivative of Lucy weight function
		double neg_onetwenty_by_pi = 2.0*neg_sixty_by_pi;	// (120/PI) used in second derivative of Lucy weight function
		double fifteen_by_twopi;				// (15/(2*pi*rd^3))

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

		/******************************************************************************************************/
		/**************************************** INITIALIZATION ROUTINE **************************************/
		/******************************************************************************************************/
		void init(){

			// defining instance of random number class
			// std::mt19937 gen(rd());

			// Set max and min dimensions of boxy
			/*
			double xind_min = -1.0*(box/2.0) + 0.25;
			double yind_min = -1.0*(box/2.0) + 0.25;
			double zind_min = -1.0*(box/2.0) + 0.25;
			double xind_max =  1.0*(box/2.0);
			double yind_max =  1.0*(box/2.0);
			double zind_max =  1.0*(box/2.0);

			double xind = xind_min;

			// Particle position intialization in a crystal structure 
			while ( xind < xind_max){
				double yind = yind_min;
				while( yind < yind_max){
					double zind = zind_min;
					while( zind < zind_max){
						// generate random velocities
						double rand_gen_velx = ((double) rand() / (RAND_MAX));
						double rand_gen_vely = ((double) rand() / (RAND_MAX));
						double rand_gen_velz = ((double) rand() / (RAND_MAX));

						// initializing particle radius, mass, position and velocity
						particles.push_back({0.2,1.0,{xind, yind, zind},{rand_gen_velx, rand_gen_vely, rand_gen_velz}});

						// update zind
						zind += 1.0;

					}// end of zind
					yind += 1.0;
				}// end of yind			
				xind += 1.0;
			}// end of xind
			*/

			// Droplet-friendly initialization
			// Set max and min dimensions of box
			/*
			   double xind_min = -2.5;
			   double yind_min = -2.5;
			   double zind_min = -1.0/2.0;
			   double xind_max = +2.5;
			   double yind_max = +2.5;
			   double zind_max = +1.0/2.0;

			   double xind = xind_min;

			// Particle position intialization in a crystal structure 
			while ( xind < xind_max){
			double yind = yind_min;
			while( yind < yind_max){
			double zind = zind_min;
			while( zind < zind_max){
			// generate random velocities
			double rand_gen_velx = ((double) rand() / (RAND_MAX));
			double rand_gen_vely = ((double) rand() / (RAND_MAX));
			double rand_gen_velz = ((double) rand() / (RAND_MAX));

			// initializing particle radius, mass, position and velocity
			particles.push_back({0.2,1.0,{xind, yind, zind},{rand_gen_velx, rand_gen_vely, rand_gen_velz}});

			// update zind
			zind += 0.1;
			}// end of zind
			yind += 0.25;
			}// end of yind			
			xind += 0.25;
			}// end of xind
			 */
		
			// Droplet Initialization	
			double rad = 5.00;
			radSqr = rad*rad;
			double dropBox = 10.0;
			double xind_min = -1.0*(dropBox/2.0) + 0.25;
			double yind_min = -1.0*(dropBox/2.0) + 0.25;
			double zind_min = -1.0*(dropBox/2.0) + 0.25;
			double xind_max =  1.0*(dropBox/2.0);
			double yind_max =  1.0*(dropBox/2.0);
			double zind_max =  1.0*(dropBox/2.0);

			double xind = xind_min;

			// Particle position intialization in a crystal structure 
			while ( xind < xind_max){
				double yind = yind_min;
				while( yind < yind_max){
					double zind = zind_min;
					while( zind < zind_max){
						// generate random velocities
						double rand_gen_velx = ((double) rand() / (RAND_MAX));
						double rand_gen_vely = ((double) rand() / (RAND_MAX));
						double rand_gen_velz = ((double) rand() / (RAND_MAX));

						// initializing particle radius, mass, position and velocity
						// if ( xind*xind + yind*yind + zind*zind <= radSqr )
						particles.push_back({0.2,1.0,{xind, yind, zind},{rand_gen_velx, rand_gen_vely, rand_gen_velz}});

						// update zind
						zind += 0.63*rcutoff;
					}// end of zind
					yind += 0.63*rcutoff;
				}// end of yind			
				xind += 0.63*rcutoff;
			}// end of xind

			// Initialize the gR_nCount array
			for (int i=0; i < gR_nElem ; ++i){
				gR_nCount.push_back(0.0);
			}	

			// Initialize the velocity histogram array
			for (int i=0; i < velHist_bins ; ++i){
				velHistX.push_back(0);
				velHistY.push_back(0);
				velHistZ.push_back(0);
			}	

			/*
			Vec3D velAvg={0.0, 0.0, 0.0};
			// Remove excess velocity
			for (Particle& p: particles){
				velAvg += p.v/particles.size();
			}

			for (Particle& p: particles){
				p.v -= velAvg;
			}
			*/

			Vec3D velAvg={0.0, 0.0, 0.0};
			// Remove excess velocity
			for (Particle& p: particles){
				velAvg += p.w/particles.size();
			}

			for (Particle& p: particles){
				p.w -= velAvg;
			}

		}

		/******************************************************************************************************/
		/**************************************** SOLVE ROUTINE ***********************************************/
		/******************************************************************************************************/
		//solve contains the time stepping algorithm and the output routines 
		void solve () {

			// initialize position, velocity, hoc, ll
			init();
			ll.resize(Ncelx*Ncelx*Ncelx); 

			vtkFileWritePosVel();

			// parameters declaration
			vsqrSum = 0.0;
			temp = 0.0;
			tempSum = 0.0;
			tempAv = 0.0;
			tempCount = 0;
			volume = pow(box, 3.0);			// system volume
			npart = 1.0*particles.size();		// number of particles
			rho = npart/volume;			// density of system 
			dof = dim*(npart - 1);			// total degrees of freedom (momentum only, no energy conservation)
			inv_sqrt_dt = 1.0/std::sqrt(dt);	// inverse of square root of the time step
			half_dt = 0.5*dt;			// 0.5*dt to be used in the integrateEOM()
			half_dt_sqr = half_dt*dt;		// 0.5*dt*dt to be used in the integrateEOM()

			counter = 0;					// initialize time, counter for file writing
			std::ofstream enStats("./data/en_data.dat");	// initialize file stream for energy
			std::ofstream eosStats("./data/eos_data.dat");	// pressure and temperature data
			std::ofstream momStats("./data/mom_data.dat");	// pressure and temperature data

			//	energyMinimization();

			if ( rcutoff > 1.0){
				std::cout << " rcutoff is more than one: please check variables wR, wR_pow_2, wij_1, wij_3, p->dens, q->dens " << std::endl;
				std::cout << " Aborting ... " << std::endl;
				exit(0);}

			// force calculation for first step
			// createList() and calculate random, dissipative and conservative
			// createList();
			dens_calculate();
			//forceCalc_CRD();

			// print the densities for particles	
			// for (Particle& p : particles)
			//	std::cout << "Position: " << p.r << ", Density: " << p.dens <<", conservative force: " << p.fC << std::endl;

			while (step<stepMax) {

				// std::cout << rn << "\t" << Ncelx << "\t" << Ncely << "\t" << Ncelz << std::endl;
				// force calculation
				// forceCalc_conservative();
				createList();
				forceCalc_CRD();

				//exit(0);
				// for (Particle&p : particles){
				//	std::cout << "position: " << p.r << " and force: " << p.f << std::endl;
				//}

				//for (Particle& p: particles)
				//	std::cout << "the force on particle is: "<< p.f << std::endl;

				// std::cout << step << std::endl;
				// std::cout << "step == 1, particles = " << npart << std::endl;

				// Integrate equations of motion using Verlet leap_frog method
				integrateEOS();
				//std::cout << "integration done" << std::endl;

				// total energy and g(r) sample calculation
				tot_en = kin_en + pot_en;
				if ( (step > gR_tStart) && (step % gR_tDelta == 0) ) grSample();

				// Apply periodic boundary conditions
				pbc();

				// filewriting 
				fileWrite(enStats, eosStats, momStats);
				// std::cout << "filewrite done" << std::endl;

				// reset variables to zero
				resetVar();

				// increment time step
				step += 1;

				// for (Particle& p : particles)
				//	std::cout << "Position: " << p.r << ", Density: " << p.dens <<", conservative force: " << p.fC << std::endl;

				//exit(0);

			}//end time loop

			// post-processing
			grCalc();	 
			velHistCalc();
			enStats.close();
			eosStats.close();
			momStats.close();

			std::ofstream paraInfo("parainfo.txt");
			paraWrite(paraInfo);
			paraInfo.close();

		} //void solve()

		/******************************************************************************************************/
		/**************************************** FUNCTIONS ***************************************************/
		/******************************************************************************************************/
		// sample collection for g(r)
		void grSample(){

			//loop over all contacts p=1..N-1, q=p+1..N to evaluate forces
			for (auto p = particles.begin();  p!=particles.end()-1; ++p){
				for (auto q = p+1;  q!=particles.end(); ++q) {

					Vec3D Rij = p->r - q->r;	
					Vec3D temp;
					temp.X = Vec3D::roundOff_x(Rij, box);
					temp.Y = Vec3D::roundOff_y(Rij, box);
					temp.Z = Vec3D::roundOff_z(Rij, box);
					Vec3D minRij = Rij - temp*box;
					double r2 = minRij.getLengthSquared();
					double dist = std::sqrt(r2);

					if ( dist < box/2.0 ) { 
						int ig = ceil(dist/gR_radDelta)-1;

						if ( (ig < 0) || (ig >= gR_nElem)) {std::cout << " out of bounds " << std::endl;} 

						gR_nCount[ig] += 2.0;	
					}
				}
			}
		}

		/******************************************************************************************************/
		/**************************************** CELL-LIST FORCE CALCULATION  ********************************/
		/******************************************************************************************************/
		/******************************************************************************************************/
		/**************************************** CONSERVATIVE FORCES *****************************************/
		/******************************************************************************************************/
		// Cell list implementation of the force calculation
		void forceCalc_CRD(){

			// creating the list for force calculation

			// identifying neighbors
			// LL: loop over all contacts p=1..N, q=p+1..N to evaluate forces
			for (unsigned int indx=0; indx<Ncelx; ++indx)
				for (unsigned int indy=0; indy<Ncely; ++indy)
					for (unsigned int indz=0; indz<Ncelz; ++indz) {
						unsigned int ix = Ncelx*(Ncelx*indz + indy) + indx + 1 - 1;

						unsigned int Nbor_indx; 
						unsigned int Nbor_indy; 
						unsigned int Nbor_indz; 
						unsigned int Nbor_ix;  

						Cell& cell = ll[ix];
						// std::cout << "The cell number is: " << ix << ", with neighbours: ";

						if (cell.size()==0) continue;
						computeForces(cell);		//compute forces between the particle pairs within a cell

						// compute forces between neighbours
						// neighbour 1
						Nbor_indx = MOD(indx + 1, Ncelx); 
						Nbor_indy = indy; 
						Nbor_indz = indz; 
						Nbor_ix = Ncelx*(Ncelx*Nbor_indz + Nbor_indy) + Nbor_indx + 1 - 1;
						// std::cout << Nbor_ix << ", ";				

						// compute forces between cell and neighbour 1
						computeForces(cell, ll[Nbor_ix]);

						// neighbour 2
						Nbor_indx = (indx + 1)% Ncelx; 
						Nbor_indy = (indy + 1)% Ncely; 
						Nbor_indz = indz; 
						Nbor_ix = Ncelx*(Ncelx*Nbor_indz + Nbor_indy) + Nbor_indx + 1 - 1;
						// std::cout << Nbor_ix << ", ";				

						// compute forces between cell and neighbour 2
						computeForces(cell, ll[Nbor_ix]);

						// neighbour 3
						Nbor_indx = indx; 
						Nbor_indy = (indy + 1)% Ncely; 
						Nbor_indz = indz; 
						Nbor_ix = Ncelx*(Ncelx*Nbor_indz + Nbor_indy) + Nbor_indx + 1 - 1;
						// std::cout << Nbor_ix << ", ";				

						// compute forces between cell and neighbour 3
						computeForces(cell,ll[Nbor_ix]);

						// neighbour 4
						// Nbor_indx = (indx - 1)% Ncelx; 
						Nbor_indx = MOD(indx-1, Ncelx);
						Nbor_indy = (indy + 1)% Ncely; 
						Nbor_indz = indz; 
						Nbor_ix = Ncelx*(Ncelx*Nbor_indz + Nbor_indy) + Nbor_indx + 1 - 1;
						// std::cout << Nbor_ix << ", ";				

						// compute forces between cell and neighbour 4
						computeForces(cell, ll[Nbor_ix]);

						// neighbour 5	
						Nbor_indx = (indx + 1)% Ncelx; 
						Nbor_indy = indy; 
						Nbor_indz = MOD(indz - 1, Ncelz); 
						Nbor_ix = Ncelx*(Ncelx*Nbor_indz + Nbor_indy) + Nbor_indx + 1 - 1;
						// std::cout << Nbor_ix << ", ";				

						// compute forces between cell and neighbour 5
						computeForces(cell, ll[Nbor_ix]);

						// neighbour 6
						Nbor_indx = (indx + 1)% Ncelx; 
						Nbor_indy = (indy + 1)% Ncely; 
						Nbor_indz = MOD(indz - 1, Ncelz); 
						Nbor_ix = Ncelx*(Ncelx*Nbor_indz + Nbor_indy) + Nbor_indx + 1 - 1;
						// std::cout << Nbor_ix << ", ";				

						// compute forces between cell and neighbour 6
						computeForces(cell, ll[Nbor_ix]);

						// neighbour 7
						Nbor_indx = indx; 
						Nbor_indy = (indy + 1)% Ncely; 
						Nbor_indz = MOD(indz - 1, Ncelz); 
						Nbor_ix = Ncelx*(Ncelx*Nbor_indz + Nbor_indy) + Nbor_indx + 1 - 1;
						// std::cout << Nbor_ix << ", ";				

						// compute forces between cell and neighbour 7
						computeForces(cell, ll[Nbor_ix]);

						// neighbour 8
						Nbor_indx = MOD(indx - 1, Ncelx); 
						Nbor_indy = (indy + 1)% Ncely; 
						Nbor_indz = MOD(indz - 1, Ncelz); 
						Nbor_ix = Ncelx*(Ncelx*Nbor_indz + Nbor_indy) + Nbor_indx + 1 - 1;
						// std::cout << Nbor_ix << ", ";				

						// compute forces between cell and neighbour 8
						computeForces(cell, ll[Nbor_ix]);

						// neihgbour 9
						Nbor_indx = (indx + 1)% Ncelx; 
						Nbor_indy = indy; 
						Nbor_indz = (indz + 1)% Ncelz; 
						Nbor_ix = Ncelx*(Ncelx*Nbor_indz + Nbor_indy) + Nbor_indx + 1 - 1;
						// std::cout << Nbor_ix << ", ";				

						// compute forces between cell and neighbour 9
						computeForces(cell, ll[Nbor_ix]);

						// neighbour 10
						Nbor_indx = (indx + 1)% Ncelx; 
						Nbor_indy = (indy + 1)% Ncely; 
						Nbor_indz = (indz + 1)% Ncelz; 
						Nbor_ix = Ncelx*(Ncelx*Nbor_indz + Nbor_indy) + Nbor_indx + 1 - 1;
						// std::cout << Nbor_ix << ", ";				

						// compute forces between cell and neighbour 10
						computeForces(cell, ll[Nbor_ix]);

						// neighbour 11
						Nbor_indx = indx; 
						Nbor_indy = (indy + 1)% Ncely; 
						Nbor_indz = (indz + 1)% Ncelz; 
						Nbor_ix = Ncelx*(Ncelx*Nbor_indz + Nbor_indy) + Nbor_indx + 1 - 1;
						// std::cout << Nbor_ix << ", ";				

						// compute forces between cell and neighbour 11
						computeForces(cell, ll[Nbor_ix]);

						// neighbour 12
						Nbor_indx = MOD(indx - 1, Ncelx); 
						Nbor_indy = (indy + 1)% Ncely; 
						Nbor_indz = (indz + 1)% Ncelz; 
						Nbor_ix = Ncelx*(Ncelx*Nbor_indz + Nbor_indy) + Nbor_indx + 1 - 1;
						// std::cout << Nbor_ix << ", ";				

						// compute forces between cell and neighbour 12
						computeForces(cell, ll[Nbor_ix]);

						// neighbour 13
						Nbor_indx = indx; 
						Nbor_indy = indy; 
						Nbor_indz = (indz + 1)% Ncelz; 
						Nbor_ix = Ncelx*(Ncelx*Nbor_indz + Nbor_indy) + Nbor_indx + 1 - 1;
						// std::cout << Nbor_ix << ", ";				

						// compute forces between cell and neighbour 13
						computeForces(cell, ll[Nbor_ix]);

						// std::cout << std::endl;	
					}
		}

		void computeForces(Cell& cell1, Cell& cell2) {
			for (Particle* p : cell1)
				for (Particle* q : cell2)
					computeForce(p,q);
		}// computeForces() -- between 2 cells	

		void computeForces(Cell& cell) {
			for (auto p = cell.begin();  p!=cell.end(); ++p)
				for (auto q = p+1;  q!=cell.end(); ++q)
					computeForce(*p,*q);
		}// computeForces() -- entire cell	

		void computeForce(Particle* p, Particle* q) {

			Vec3D Rij = p->r - q->r;	
			Vec3D Vij = p->v - q->v;	
			Vec3D temp;
			temp.X = Vec3D::roundOff_x(Rij, box);
			temp.Y = Vec3D::roundOff_y(Rij, box);
			temp.Z = Vec3D::roundOff_z(Rij, box);
			Vec3D minRij = Rij - temp*box;
			double r2 = minRij.getLengthSquared();

			if ( r2 <= rc2 ) {

				//	if (opt == 3){

				Vec3D fCij;
				Vec3D fRij;
				Vec3D fDij;
				double dist = std::sqrt(r2);
				Vec3D capRij = minRij/dist;
				double wR = ( 1.0 - dist);
				double wR_pow_2 = wR*wR;
				double term2 = 0.0;
				// conservative force -- soft potential 
				/*
				   fCij.X = aii*wR*capRij.X;
				   fCij.Y = aii*wR*capRij.Y;
				   fCij.Z = aii*wR*capRij.Z;
				   fCij.X = thirty_by_pi*aii*wR*capRij.X;
				   fCij.Y = thirty_by_pi*aii*wR*capRij.Y;
				   fCij.Z = thirty_by_pi*aii*wR*capRij.Z;
				 */

				// conservative force -- many body potential
				// Van der Waals force
				// Dissipative-particle-dynamics model for two-phase flows, Anupam Tiwari and John Abraham, Phys. Rev. E 74, 056701 – Published 1 November 2006 
				/*
				   double wij_1 = neg_sixty_by_pi*dist*wR_pow_2;
				   double wij_3 = neg_onetwenty_by_pi*(-2.0 + 3.0*dist);
				   double term1 = 1.0/( 1.0 - bVdW*(p->dens) );
				   double term2 = 1.0/( 1.0 - bVdW*(q->dens) );
				   double term3 = ( -1.0*bVdW*kBT*( term1 + term2) + 2.0*aVdW )*wij_1 + kappa*wij_3;
				   fCij.X = term3*capRij.X; 
				   fCij.Y = term3*capRij.Y; 
				   fCij.Z = term3*capRij.Z; 

				   p->fC += fCij;
				   q->fC += fCij*(-1.0); 
				 */

				// Warren's force
				// Vapor-liquid coexistence in many-body dissipative particle dynamics, P. B. Warren, Phys. Rev. E 68, 066702 – Published 18 December 2003
				if (r2 <= rd2){
					double wdij = (1.0 - dist/rd_cutoff);
					term2 = Bij*( p->dens + q->dens)*wdij;

					double wdij_pow_2 = wdij*wdij; 
					double wij = fifteen_by_twopi*wdij_pow_2;

					p->dens_new += wij;
					q->dens_new += wij;
				}

				double term1 = Aij*wR;
				double term3 = term1 + term2; 
				fCij.X = term3*capRij.X; 
				fCij.Y = term3*capRij.Y; 
				fCij.Z = term3*capRij.Z; 

				p->fC += fCij;
				q->fC += fCij*(-1.0); 

				/*
				// random force	
				double uniRand = d(rd); 
				double thetaij = std::sqrt(12.0)*(uniRand-0.5); 
				double magRand = sigma*wR*thetaij;
				fRij.X = magRand*capRij.X;
				fRij.Y = magRand*capRij.Y;
				fRij.Z = magRand*capRij.Z;

				Vec3D sumForce = fRij*inv_sqrt_dt;
				p->fR += sumForce;
				q->fR += -1.0*sumForce;

				// dissipative force -- not calculated here
				double rDotv = Vec3D::dot( capRij, Vij );
				double magDiss = -1.0*gamma*wR_pow_2*rDotv;
				fDij.X = magDiss*capRij.X;
				fDij.Y = magDiss*capRij.Y;
				fDij.Z = magDiss*capRij.Z;

				p->fD += fDij;
				q->fD += -1.0*fDij;
				 */
				// potential energy
				/*
				   double pair_pot_en = (aii/2.0)*(1.0 - (dist/rcutoff))*( 1.0 - (dist/rcutoff) ) ;
				   pot_en += pair_pot_en;
				 */
				// non-ideal comp pressure
				double nonIdealcomp = Vec3D::dot(minRij, fCij)*(1.0/(2.0*dim*volume));
				pressure += nonIdealcomp;


				//	}// calculate random, conservative and dissipative forces	
			}// computeForce() -- between 2 particles
		}

		// simple Leap-Frog Verlet
		void integrateEOS(){
			for (Particle& p : particles) {
				// store velocity (mid-step)
				Vec3D w_old = p.w;

				// update velocities (mid-step)
				p.w += (p.fC)*(dt/p.m);

				// update position (integral time step) using the velocities (mid-step)
				p.r += p.w*dt;				

				// calculate velocity (integral time step)
				p.v = 0.5*(w_old + p.w);

				// distribute velocities into velocity bins
				if ( (step > velHist_tStart) && (step % velHist_tDelta == 0) ) {
					int ivelX = ceil((velHist_velMax - p.v.X)/velHist_velDelta); 
					int ivelY = ceil((velHist_velMax - p.v.Y)/velHist_velDelta); 
					int ivelZ = ceil((velHist_velMax - p.v.Z)/velHist_velDelta); 

					velHistX[ivelX] += 1;
					velHistY[ivelY] += 1;
					velHistZ[ivelZ] += 1;

					tempSum += temp;
					tempCount += 1;
				}

				// calculate the kinetic energy
				kin_en += 0.5*p.m*(p.v.getLengthSquared());
			}
			// ideal component of pressure
			temp = 2*kin_en/dof;
			double idealComp = rho*1.0*temp; 
			pressure += idealComp;
		}

		// Integrating equations of motion using the velocity Verlet scheme
		void integrateEOM(){

			// calculate v(t + dt/2) and r(t + dt) usind v(t), r(t), fC(t), fD(t), fR(t)
			for (Particle&p : particles){

				// calculate v(t + dt/2)  using v(t), r(t), fC(t), fD(t), fR(t)
				p.v += half_dt*(p.fC + p.fD + p.fR);

				// calculate r(t + dt) using v(t), r(t), fC(t), fD(t), fR(t)
				p.r += p.v*dt;
			}

			// apply periodic boundary condition -- particle positions have changed
			pbc();

			// reset force to 0
			for (Particle& p : particles) {
				p.fC.setZero();
				p.fR.setZero();
				p.fD.setZero();

				if (step == 1){
					p.dens = 0.0;
				}
			}

			// createList() and calculate random, dissipative and conservative
			createList();
			dens_calculate();
			forceCalc_CRD();

			// final step 
			for (Particle&p : particles){

				// v(t + dt) using fC(t+dt), fR(t+dt), fD(t+dt/2)
				p.v += half_dt*(p.fC + p.fD + p.fR);
			}


			// fD = 0 for new calculation
			for (Particle& p : particles) {
				p.fD.setZero();
			}

			// fD using v(t + dt), r(t+dt)
			forceCalc_dissipative();

			for (Particle&p : particles){

				// calculate the kinetic energy
				vsqrSum += p.v.getLengthSquared();

				// calculate the total momentum
				momX += p.v.X;
				momY += p.v.Y;
				momZ += p.v.Z;

				// distribute velocities into velocity bins
				if ( (step > velHist_tStart) && (step % velHist_tDelta == 0) ) {
					int ivelX = ceil(( p.v.X - velHist_velMin )/velHist_velDelta) - 1; 
					int ivelY = ceil(( p.v.Y - velHist_velMin )/velHist_velDelta) - 1; 
					int ivelZ = ceil(( p.v.Z - velHist_velMin )/velHist_velDelta) - 1; 

					if( ( ivelX < 0 ) || ( ivelX >= velHist_bins  ) ) std::cout << "out of bounds ivelX" << std::endl;
					if( ( ivelY < 0 ) || ( ivelY >= velHist_bins  ) ) std::cout << "out of bounds ivelY" << std::endl;
					if( ( ivelZ < 0 ) || ( ivelZ >= velHist_bins  ) ) std::cout << "out of bounds ivelZ" << std::endl;

					velHistX[ivelX] += 1;
					velHistY[ivelY] += 1;
					velHistZ[ivelZ] += 1;

					// tempSum += temp;
					// tempCount += 1;
				}	

			}					

			// ideal component of pressure
			kin_en = 0.5*vsqrSum;
			temp = vsqrSum/dof;
			double idealComp = rho*1.0*temp; 
			pressure += idealComp;

			if ( (step > velHist_tStart) && (step % velHist_tDelta == 0) ) {

				tempSum += temp;
				tempCount += 1;
			}
		}

		void dens_calculate(){
			//loop over all contacts p=1..N-1, q=p+1..N to evaluate forces
			for (auto p = particles.begin();  p!=particles.end()-1; ++p){
				for (auto q = p+1;  q!=particles.end(); ++q) {

					Vec3D Rij = p->r - q->r;	
					Vec3D temp;
					temp.X = Vec3D::roundOff_x(Rij, box);
					temp.Y = Vec3D::roundOff_y(Rij, box);
					temp.Z = Vec3D::roundOff_z(Rij, box);
					Vec3D minRij = Rij - temp*box;
					double r2 = minRij.getLengthSquared();
					double dist = std::sqrt(r2);

					if ( r2 < rd2 ) { 

						double wdij = (1.0 - dist/rd_cutoff);
						double wdij_pow_2 = wdij*wdij; 
						double wij = fifteen_by_twopi*wdij_pow_2;

						//	std::cout << "dist = " << dist << ", wdij=" << wdij << ", 15/2*pi= " << fifteen_by_twopi << std::endl;

						p->dens += wij;
						q->dens += wij;
					}
				}
			}
		}

		/******************************************************************************************************/
		/**************************************** DISSIPATIVE FORCES *****************************************/
		/******************************************************************************************************/
		// calculating the dissipative forces
		void forceCalc_dissipative(){

			// only identifying neighbors
			// LL: loop over all contacts p=1..N, q=p+1..N to evaluate forces
			for (unsigned int indx=0; indx<Ncelx; ++indx)
				for (unsigned int indy=0; indy<Ncely; ++indy)
					for (unsigned int indz=0; indz<Ncelz; ++indz) {
						unsigned int ix = Ncelx*(Ncelx*indz + indy) + indx + 1 - 1;

						unsigned int Nbor_indx; 
						unsigned int Nbor_indy; 
						unsigned int Nbor_indz; 
						unsigned int Nbor_ix;  

						Cell& cell = ll[ix];
						// std::cout << "The cell number is: " << ix << ", with neighbours: ";

						if (cell.size()==0) continue;
						computeForces_dissipative(cell);		//compute forces between the particle pairs within a cell

						// compute forces between neighbours
						// neighbour 1
						Nbor_indx = MOD(indx + 1, Ncelx); 
						Nbor_indy = indy; 
						Nbor_indz = indz; 
						Nbor_ix = Ncelx*(Ncelx*Nbor_indz + Nbor_indy) + Nbor_indx + 1 - 1;
						// std::cout << Nbor_ix << ", ";				

						// compute forces between cell and neighbour 1
						computeForces_dissipative(cell, ll[Nbor_ix]);

						// neighbour 2
						Nbor_indx = (indx + 1)% Ncelx; 
						Nbor_indy = (indy + 1)% Ncely; 
						Nbor_indz = indz; 
						Nbor_ix = Ncelx*(Ncelx*Nbor_indz + Nbor_indy) + Nbor_indx + 1 - 1;
						// std::cout << Nbor_ix << ", ";				

						// compute forces between cell and neighbour 2
						computeForces_dissipative(cell, ll[Nbor_ix]);

						// neighbour 3
						Nbor_indx = indx; 
						Nbor_indy = (indy + 1)% Ncely; 
						Nbor_indz = indz; 
						Nbor_ix = Ncelx*(Ncelx*Nbor_indz + Nbor_indy) + Nbor_indx + 1 - 1;
						// std::cout << Nbor_ix << ", ";				

						// compute forces between cell and neighbour 3
						computeForces_dissipative(cell,ll[Nbor_ix]);

						// neighbour 4
						// Nbor_indx = (indx - 1)% Ncelx; 
						Nbor_indx = MOD(indx-1, Ncelx);
						Nbor_indy = (indy + 1)% Ncely; 
						Nbor_indz = indz; 
						Nbor_ix = Ncelx*(Ncelx*Nbor_indz + Nbor_indy) + Nbor_indx + 1 - 1;
						// std::cout << Nbor_ix << ", ";				

						// compute forces between cell and neighbour 4
						computeForces_dissipative(cell, ll[Nbor_ix]);

						// neighbour 5	
						Nbor_indx = (indx + 1)% Ncelx; 
						Nbor_indy = indy; 
						Nbor_indz = MOD(indz - 1, Ncelz); 
						Nbor_ix = Ncelx*(Ncelx*Nbor_indz + Nbor_indy) + Nbor_indx + 1 - 1;
						// std::cout << Nbor_ix << ", ";				

						// compute forces between cell and neighbour 5
						computeForces_dissipative(cell, ll[Nbor_ix]);

						// neighbour 6
						Nbor_indx = (indx + 1)% Ncelx; 
						Nbor_indy = (indy + 1)% Ncely; 
						Nbor_indz = MOD(indz - 1, Ncelz); 
						Nbor_ix = Ncelx*(Ncelx*Nbor_indz + Nbor_indy) + Nbor_indx + 1 - 1;
						// std::cout << Nbor_ix << ", ";				

						// compute forces between cell and neighbour 6
						computeForces_dissipative(cell, ll[Nbor_ix]);

						// neighbour 7
						Nbor_indx = indx; 
						Nbor_indy = (indy + 1)% Ncely; 
						Nbor_indz = MOD(indz - 1, Ncelz); 
						Nbor_ix = Ncelx*(Ncelx*Nbor_indz + Nbor_indy) + Nbor_indx + 1 - 1;
						// std::cout << Nbor_ix << ", ";				

						// compute forces between cell and neighbour 7
						computeForces_dissipative(cell, ll[Nbor_ix]);

						// neighbour 8
						Nbor_indx = MOD(indx - 1, Ncelx); 
						Nbor_indy = (indy + 1)% Ncely; 
						Nbor_indz = MOD(indz - 1, Ncelz); 
						Nbor_ix = Ncelx*(Ncelx*Nbor_indz + Nbor_indy) + Nbor_indx + 1 - 1;
						// std::cout << Nbor_ix << ", ";				

						// compute forces between cell and neighbour 8
						computeForces_dissipative(cell, ll[Nbor_ix]);

						// neihgbour 9
						Nbor_indx = (indx + 1)% Ncelx; 
						Nbor_indy = indy; 
						Nbor_indz = (indz + 1)% Ncelz; 
						Nbor_ix = Ncelx*(Ncelx*Nbor_indz + Nbor_indy) + Nbor_indx + 1 - 1;
						// std::cout << Nbor_ix << ", ";				

						// compute forces between cell and neighbour 9
						computeForces_dissipative(cell, ll[Nbor_ix]);

						// neighbour 10
						Nbor_indx = (indx + 1)% Ncelx; 
						Nbor_indy = (indy + 1)% Ncely; 
						Nbor_indz = (indz + 1)% Ncelz; 
						Nbor_ix = Ncelx*(Ncelx*Nbor_indz + Nbor_indy) + Nbor_indx + 1 - 1;
						// std::cout << Nbor_ix << ", ";				

						// compute forces between cell and neighbour 10
						computeForces_dissipative(cell, ll[Nbor_ix]);

						// neighbour 11
						Nbor_indx = indx; 
						Nbor_indy = (indy + 1)% Ncely; 
						Nbor_indz = (indz + 1)% Ncelz; 
						Nbor_ix = Ncelx*(Ncelx*Nbor_indz + Nbor_indy) + Nbor_indx + 1 - 1;
						// std::cout << Nbor_ix << ", ";				

						// compute forces between cell and neighbour 11
						computeForces_dissipative(cell, ll[Nbor_ix]);

						// neighbour 12
						Nbor_indx = MOD(indx - 1, Ncelx); 
						Nbor_indy = (indy + 1)% Ncely; 
						Nbor_indz = (indz + 1)% Ncelz; 
						Nbor_ix = Ncelx*(Ncelx*Nbor_indz + Nbor_indy) + Nbor_indx + 1 - 1;
						// std::cout << Nbor_ix << ", ";				

						// compute forces between cell and neighbour 12
						computeForces_dissipative(cell, ll[Nbor_ix]);

						// neighbour 13
						Nbor_indx = indx; 
						Nbor_indy = indy; 
						Nbor_indz = (indz + 1)% Ncelz; 
						Nbor_ix = Ncelx*(Ncelx*Nbor_indz + Nbor_indy) + Nbor_indx + 1 - 1;
						// std::cout << Nbor_ix << ", ";				

						// compute forces between cell and neighbour 13
						computeForces_dissipative(cell, ll[Nbor_ix]);

						// std::cout << std::endl;	
					}
		}

		void computeForces_dissipative(Cell& cell1, Cell& cell2) {
			for (Particle* p : cell1)
				for (Particle* q : cell2)
					computeForce_dissipative(p,q);
		}// computeForces_dissipative() -- between 2 cells	

		void computeForces_dissipative(Cell& cell) {
			for (auto p = cell.begin();  p!=cell.end(); ++p)
				for (auto q = p+1;  q!=cell.end(); ++q)
					computeForce_dissipative(*p,*q);
		}// computeForces_dissipative() -- entire cell	

		void computeForce_dissipative(Particle *p, Particle *q){

			Vec3D Rij = p->r - q->r;	
			Vec3D Vij = p->v - q->v;	
			Vec3D temp;
			temp.X = Vec3D::roundOff_x(Rij, box);
			temp.Y = Vec3D::roundOff_y(Rij, box);
			temp.Z = Vec3D::roundOff_z(Rij, box);
			Vec3D minRij = Rij - temp*box;
			double r2 = minRij.getLengthSquared();

			if ( r2 <= rc2 ){

				Vec3D fRij;
				Vec3D fDij;
				double dist = std::sqrt(r2);
				Vec3D capRij = minRij/dist;
				double rDotv = Vec3D::dot( capRij, Vij );
				double uniRand = d(rd); 
				double thetaij = std::sqrt(12.0)*(uniRand-0.5); 
				double wR = ( 1.0 - dist);
				double wR_pow_2 = wR*wR;

				// calculate local density at particle position
				// p->dens and q-> dens is calculated here to be used 
				// for the next iteration -- first step gives rubbish
				// weight function used -- Lucy weight function
				/*
				   double wR_pow_3 = wR*wR_pow_2;
				   double wij = five_by_pi*(1.0 + 3.0*dist)*wR_pow_3;
				   p->dens += wij; 
				   q->dens += wij;
				 */

				/*
				// Warren weight function
				if (r2 <= rd2){
				double inv_r3 = 1.0/(dist*dist*dist);
				double wdij = (1.0 - dist/rd_cutoff);
				double wdij_pow_2 = wdij*wdij; 
				double wij = fifteen_by_twopi*wdij_pow_2;

				p->dens += wij;
				q->dens += wij;
				}

				// dissipative force
				double magDiss = -1.0*gamma*wR_pow_2*rDotv;
				fDij.X = magDiss*capRij.X;
				fDij.Y = magDiss*capRij.Y;
				fDij.Z = magDiss*capRij.Z;

				// pairwise dissipative forces
				p->fD += fDij;
				q->fD += -1.0*fDij;
				 */

			}

		}// computeForce_dissipative() -- end of dissipative+random force between 2 particles

		// Periodic boundary conditions
		void pbc(){
			for (Particle& p : particles) {

				Vec3D temp;
				temp.X = Vec3D::roundOff_x(p.r, box);
				temp.Y = Vec3D::roundOff_y(p.r, box);
				temp.Z = Vec3D::roundOff_z(p.r, box);
				p.r  = p.r - temp*box;				
			}
		}// pbc()

		void createList(){

			//LL: remove old linked list
			for (Cell& cell : ll) cell.resize(0); 
			// ll.resize(0); // this strangely gave me a segmentation fault

			//LL: re-initialise linked list
			for (Particle& p : particles) {

				double Lx = -1.0*(box/2.0) - p.r.getComponent(0);
				double Ly = -1.0*(box/2.0) - p.r.getComponent(1);
				double Lz = -1.0*(box/2.0) - p.r.getComponent(2);

				unsigned int indx = -1.0*ceil(Lx/rn);
				unsigned int indy = -1.0*ceil(Ly/rn);
				unsigned int indz = -1.0*ceil(Lz/rn);

				unsigned int ix = Ncelx*(Ncelx*indz + indy) + indx + 1 - 1;
				// std::cout << "position: " << std::setw(10) << p.r << ", indices: "<< indx << ", " << indy << ", " <<indz << " and cell number: " << ix <<std::endl;
				ll[ix].push_back(&p);
				// std::cout << "successfully done for cell number: " << ix << std::endl; 

				/*
				// accessing ll			
				for (int i=0; i < ll.size(); ++i){
				Cell& cell = ll[i];
				}*/
			}
		}

		// Structure function g(r) calculation
		void grCalc(){


			// write the g(r) data to a file
			std::ofstream grWrite("./data/gr_data.dat");
			grWrite << "ri \t ro \t rad \t gr_count \t tot_part \t samples \t nHomo \t rho" << std::endl;

			for (int i=0; i < gR_nElem; ++i){

				double rad = gR_radDelta*( i + 0.5) + gR_radMin;		// radius in question	
				double ri = i*gR_radDelta + gR_radMin;				// radius at i^th bin
				double ro = (i+1)*gR_radDelta + gR_radMin;				// radius at (i+1)^th bin
				// double shellVol = (4/3)*M_PI*( pow(ro, 3.0) - pow(ri, 3.0) );	// volume of shell
				// double nHomo = shellVol*rho;					// number of particles if homogeneous

				// gR_nCount[i] /= (npart*gR_tSamples*nHomo);				// normalizing g(r)

				grWrite << ri << "\t" << ro << "\t" << rad << "\t" << gR_nCount[i] << "\t" << npart << "\t" << gR_tSamples << "\t" << rho << std::endl;
			}

			std::cout << " the total number of samples is: " << gR_tSamples	<< std::endl;
			std::cout << " the homogeneous density is: " << rho << std::endl;
			std::cout << " the number of particles is: " << npart << std::endl;

			// file close	
			grWrite.close();
		}//grCalc()

		// Velocity distribution calculation -- convert histogram to PDF
		void velHistCalc(){

			std::ofstream velDistdata("./data/velDist_data.dat"); 

			double trapzAreaX = 0.0;
			double trapzAreaY = 0.0;
			double trapzAreaZ = 0.0;

			for (int i=1; i <= velHist_bins-2; ++i){

				trapzAreaX += 2.0*velHistX[i];	
				trapzAreaY += 2.0*velHistY[i]; 	
				trapzAreaZ += 2.0*velHistZ[i];	
			}	

			// adding the contributions from the first and last element
			trapzAreaX += velHistX[0] + velHistX[velHist_bins - 1];
			trapzAreaY += velHistY[0] + velHistY[velHist_bins - 1];
			trapzAreaZ += velHistZ[0] + velHistZ[velHist_bins - 1];

			// multiplying the distance between the bins to get the final area
			trapzAreaX *= velHist_velDelta/2.0;
			trapzAreaY *= velHist_velDelta/2.0;
			trapzAreaZ *= velHist_velDelta/2.0;

			// converting histogram to PDF
			for (int i=0; i< velHist_bins; ++i){
				double bin_lower = velHist_velMin + i*velHist_velDelta;
				double bin_upper = velHist_velMin + (i+1)*velHist_velDelta;
				double vel = (bin_lower + bin_upper)*0.5;
				tempAv = tempSum/tempCount;

				velDistdata << vel << "\t" << tempAv << "\t" << velHistX[i] << "\t" << trapzAreaX << "\t" << velHistY[i] << "\t" << trapzAreaY << "\t" << velHistZ[i] << "\t" << trapzAreaZ << std::endl;	
			}

			velDistdata.close();
		}

		void paraWrite(std::ofstream& paraInfo){

			paraInfo << "set temperature (kbT)  	" << "\t \t" << std::setw(20) << std::setprecision(15) << kBT << std::endl;
			paraInfo << "droplet radius (rad)  	" << "\t \t" << std::setw(20) << std::setprecision(15) << sqrt(radSqr) << std::endl;
			paraInfo << "box length (box)   	" << "\t \t" << std::setw(20) << std::setprecision(15)<< box << std::endl;
			paraInfo << "cutoff attr (rcutoff)	" << "\t \t" << std::setw(20) << std::setprecision(15) << rcutoff << std::endl;
			paraInfo << "cutoff rep (rd_cutoff)	" << "\t \t" << std::setw(20) << std::setprecision(15) << rd_cutoff << std::endl;
			paraInfo << "dimensions (dim)		" << "\t \t" << std::setw(20) << std::setprecision(15) << dim << std::endl;
			paraInfo << "density (rho)		" << "\t \t" << std::setw(20) << std::setprecision(15) << rho << std::endl;
			paraInfo << "timestep (dt)		" << "\t \t" << std::setw(20) << std::setprecision(15) << dt << std::endl;
			paraInfo << "number of particles (npart)" << "\t \t" << std::setw(20) << std::setprecision(15) << npart << std::endl;
			paraInfo << "total run time (stepMax)   " << "\t \t" << std::setw(20) << std::setprecision(15) << stepMax << std::endl;
			paraInfo << "---------------------------" << std::endl;
			paraInfo << "Random & Dissipative Force " << std::endl;
			paraInfo << "---------------------------" << std::endl;
			paraInfo << "noise level (sigma)	" << "\t \t" << std::setw(20) << std::setprecision(15) << sigma << std::endl;
			paraInfo << "friction parameter (gamma) " << "\t \t" << std::setw(20) << std::setprecision(15) << gamma << std::endl;
			paraInfo << "---------------------------" << std::endl;
			paraInfo << "Conservative Force         " << std::endl;
			paraInfo << "---------------------------" << std::endl;
			paraInfo << "Attractive (Aij)		" << "\t \t" << std::setw(20) << std::setprecision(15) << Aij << std::endl;
			paraInfo << "Repulsive (Bij) 		" << "\t \t" << std::setw(20) << std::setprecision(15) << Bij << std::endl;
			paraInfo << "---------------------------" << std::endl;
			paraInfo << "---------------------------" << std::endl;
			paraInfo << "---------------------------" << std::endl;

		}

		// Reset variables
		void resetVar(){
			// energy reset to zero
			pot_en = 0.0;
			kin_en = 0.0;
			vsqrSum = 0.0;
			tot_en = 0.0;
			pressure = 0.0;
			momX = 0.0;
			momY = 0.0;
			momZ = 0.0;

			for (Particle& p : particles){
				p.dens = p.dens_new;
				p.dens_new = 0.0;
				p.fC.setZero();
				p.fR.setZero();
				p.fD.setZero();
			}
		}

		// File writing
		void fileWrite(std::ofstream& enStats, std::ofstream& eosStats, std::ofstream& momStats){

			if ( step % 10000 == 0){
				std::cout<< step << " steps out of " << stepMax << " completed " << std::endl;
			}

			//write output file in the .data format every 10th time step
			if (++counter>=saveCount) {
				//reset the counter, write time to terminal
				counter = 0;

				// particle positions in vtk file format
				vtkFileWritePosVel();

				//writing the energy balance
				enStats << std::setw(20) << std::setprecision(15) << pot_en << "\t" << std::setw(20) << std::setprecision(15) << kin_en << "\t" << std::setw(20) << std::setprecision(15) << tot_en << std::endl;
				eosStats << std::setw(20) << std::setprecision(15) << rho << "\t" << std::setw(20) << std::setprecision(15)  << temp << "\t" << std::setw(20) << std::setprecision(15) << pressure << std::endl;
				momStats << std::setw(20) << std::setprecision(15) << momX << "\t" << std::setw(20) << std::setprecision(15) << momY << "\t" << std::setw(20) << std::setprecision(15) << momZ << std::endl;
			}

		}

		int MOD(int a, int b){

			if (a >= 0)
				return(a%b);
			else
				return( (a%b) + b);
		}

		// Filewriting for the VTK format
		void vtkFileWritePosVel(){

			char filename[40];

			sprintf( filename, "./data/data1_%d.vtu", step);  
			std::ofstream file(filename);
			file << "<?xml version=\"1.0\"?>" << std::endl;
			file << "<VTKFile type=\"UnstructuredGrid\" version=\"0.1\" byte_order=\"LittleEndian\">" << std::endl;
			file << "<UnstructuredGrid>";
			file << "<Piece NumberOfPoints=\""<< particles.size() <<"\" NumberOfCells=\"0\">" << std::endl;
			file << "<Points>" << std::endl;
			// position data
			file << "<DataArray type=\"Float32\" Name=\"Position\" NumberOfComponents=\"3\" format=\"ascii\">"<< std::endl;
			for (Particle& p: particles)
			{
				file << p.r << std::endl;

			}
			file << "</DataArray>"<<std::endl;
			file << "</Points>" << std::endl;	

			// velocity data
			file << "<PointData Vectors=\"vector\">"<< std::endl;	
			file << "<DataArray type=\"Float32\" Name=\"Velocity\" NumberOfComponents=\"3\" format=\"ascii\">" << std::endl;
			for (Particle& p: particles)
			{
				file <<  p.v << std::endl;

			}
			file << "</DataArray>"<< std::endl;

			// radius data
			file << "<DataArray type=\"Float32\" Name=\"Radius\" format=\"ascii\">" << std::endl;
			for (Particle& p: particles)
			{
				file <<  p.a << std::endl;

			}
			file << "</DataArray>"<< std::endl;
			file << "</PointData>" << std::endl;	

			// Cells data
			file << "<Cells>"<< std::endl;	
			file << "<DataArray type=\"Int32\" Name=\"connectivity\" format=\"ascii\"></DataArray>" << std::endl;
			file << "<DataArray type=\"Int32\" Name=\"offsets\" format=\"ascii\"></DataArray>" << std::endl;
			file << "<DataArray type=\"UInt8\" Name=\"types\" format=\"ascii\"></DataArray>" << std::endl;
			file << "</Cells>" << std::endl;

			// Piece
			file << "</Piece>" << std::endl;
			file << "</UnstructuredGrid>" << std::endl;
			file << "</VTKFile>" << std::endl;

			file.close();

		}

};



/*
// compute dissipative and random force -- Lowe thermostat
void computeForce_dissipative(Particle* p, Particle* q) {

// std::mt19937 gen(rd());

Vec3D Rij = p->r - q->r;	
Vec3D Vij = p->v - q->v;	
Vec3D temp;
temp.X = Vec3D::roundOff_x(Rij, box);
temp.Y = Vec3D::roundOff_y(Rij, box);
temp.Z = Vec3D::roundOff_z(Rij, box);
Vec3D minRij = Rij - temp*box;
double r2 = minRij.getLengthSquared();
double dist = sqrt(r2);
Vec3D capRij = minRij/dist;

if ( r2 <= rc2 ) {

Vec3D Deltaij;

double zetaij = d(rd); 
double term1 = zetaij*sqrt(2.0*kBT/p->m);
double term2 = Vec3D::dot( Vij, capRij );
double term3 = term1 - term2;

Deltaij.X = 0.5*capRij.X*term3;
Deltaij.Y = 0.5*capRij.Y*term3;
Deltaij.Z = 0.5*capRij.Z*term3;

// std::cout << Deltaij << std::endl;

p->v += Deltaij;
q->v += Deltaij*(-1.0); 
}		

}// computeForce() -- between 2 particles
 */

#endif
