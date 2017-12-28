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
		double box;				// size of domain
		double epsilon;				// well-depth in Lennard-Jones po
		double sigma;				// distance at which potential is
		double aii;				// DPD conservative force parameter
		double rcutoff;				// cut-off distance
		double rc2;				// square of the cutoff distance
		double dt;				// time step
		double dim;				// dimension of system
		double dof;				// degrees of freedom

		int step;				// counter for number of steps 
		double stepMax;				// total number of steps
		double pot_en;				// system potential energy
		double kin_en;				// system kinetic energy
		double tot_en;				// system total energy
		double pressure;			// pressure
		double kBT;				// Boltzmann constant
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

		// cut-off correction to potential
		double sig6;
		double rc2i;
		double rc6i;
		double ecut;
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
		std::normal_distribution<double> d{0,1};

		/******************************************************************************************************/
		/**************************************** INITIALIZATION ROUTINE **************************************/
		/******************************************************************************************************/
		void init(){

			// defining instance of random number class
			// std::mt19937 gen(rd());

			// Set max and min dimensions of boxy
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

						/*
						   double rand_gen_velx = d(gen);
						   double rand_gen_vely = d(gen);
						   double rand_gen_velz = d(gen);*/
						// initializing particle mass, radius, position and mid-step-velocity
						particles.push_back({0.2,1.0,{xind, yind, zind},{rand_gen_velx, rand_gen_vely, rand_gen_velz}});

						// update zind
						zind += 1.0;

					}// end of zind
					yind += 1.0;
				}// end of yind			
				xind += 1.0;
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

			Vec3D velAvg={0.0, 0.0, 0.0};
			// Remove excess velocity
			for (Particle& p: particles){
				velAvg += p.v/particles.size();
			}

			for (Particle& p: particles){
				p.v -= velAvg;
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

			// parameters declaration
			temp = 0.0;
			tempSum = 0.0;
			tempAv = 0.0;
			tempCount = 0;
			volume = pow(box, 3.0);			// system volume
			npart = 1.0*particles.size();		// number of particles
			rho = npart/volume;			// density of system 
			dof = dim*(npart - 1);			// total degrees of freedom (momentum only, no energy conservation)

			// cut-off correction to potential
			sig6 = pow(sigma,6);
			rc2i = 1/rc2;
			rc6i = pow(rc2i,3);
			ecut = 4*epsilon*sig6*rc6i*(sig6*rc6i - 1.0);

			counter = 0;					// initialize time, counter for file writing
			std::ofstream enStats("./data/en_data.dat");	// initialize file stream for energy
			std::ofstream eosStats("./data/eos_data.dat");	// pressure and temperature data
			std::ofstream momStats("./data/mom_data.dat");	// pressure and temperature data

			//	energyMinimization();

			while (step<stepMax) {

				// std::cout << rn << "\t" << Ncelx << "\t" << Ncely << "\t" << Ncelz << std::endl;
				// force calculation
				// forceCalc();
				// forceCalc_conservative();

				// for (Particle&p : particles){
				//	std::cout << "position: " << p.r << " and force: " << p.f << std::endl;
				//}

				//for (Particle& p: particles)
				//	std::cout << "the force on particle is: "<< p.f << std::endl;

				// std::cout << step << std::endl;

				// Integrate equations of motion using Verlet leap_frog method
				integrateEOS();

				// total energy and g(r) sample calculation
				tot_en = kin_en + pot_en;
				if ( (step > gR_tStart) && (step % gR_tDelta == 0) ) grSample();
					
				// Apply periodic boundary conditions
				pbc();

				// filewriting 
				fileWrite(enStats, eosStats, momStats);

				// reset variables to zero
				resetVar();

				// increment time step
				step += 1;

			} //end time loop

			// post-processing
			grCalc();	 
			velHistCalc();
			enStats.close();
			eosStats.close();
			momStats.close();

		} //void solve()

		/******************************************************************************************************/
		/**************************************** FUNCTIONS ***************************************************/
		/******************************************************************************************************/
		// Brute force implementation of the Force Calculation 
		/*
		void forceCalc(){

			//loop over all contacts p=1..N-1, q=p+1..N to evaluate forces
			for (auto p = particles.begin();  p!=particles.end()-1; ++p){
				//for_loop_inner_counter = 0;
				for (auto q = p+1;  q!=particles.end(); ++q) {

					Vec3D Rij = p->r - q->r;	
					Vec3D temp;
					temp.X = Vec3D::roundOff_x(Rij, box);
					temp.Y = Vec3D::roundOff_y(Rij, box);
					temp.Z = Vec3D::roundOff_z(Rij, box);
					Vec3D minRij = Rij - temp*box;
					double r2 = minRij.getLengthSquared();
					double dist = sqrt(r2);

					if ( r2 < rc2 ) {

						Fij.X = aii*( 1.0 - (dist/rcutoff) )*( minRij.X / dist);
						Fij.Y = aii*( 1.0 - (dist/rcutoff) )*( minRij.Y / dist);
						Fij.Z = aii*( 1.0 - (dist/rcutoff) )*( minRij.Z / dist);

						p->f += Fij;
						q->f += Fij*(-1.0); 

						// potential energy
						// double pair_pot_en = epsilon*(sigma - dist)*(sigma - dist);
						double pair_pot_en = aii*( 1.0  - (dist/rcutoff) )*( 1.0  - (dist/rcutoff) );
						pot_en += pair_pot_en - ecut;

						// non-ideal comp pressure
						double nonIdealcomp = Vec3D::dot(minRij, Fij)*(1.0/(dim*volume));
						pressure += nonIdealcomp;
					}		

					// radial distribution function
					if ( (step > gR_tStart) && (step % gR_tDelta == 0) ) {

						if ( dist < box/2.0 ) { 
							int ig = round(dist/gR_radDelta) - 2;

							gR_nCount[ig] += 2;	
						}
					} 
				}
			}
		}*/

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
					double dist = sqrt(r2);

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
		void forceCalc_conservative(){

			// creating the list for force calculation
			createList();

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

		void computeForce(Particle* p, Particle* q) {

			Vec3D Rij = p->r - q->r;	
			Vec3D temp;
			temp.X = Vec3D::roundOff_x(Rij, box);
			temp.Y = Vec3D::roundOff_y(Rij, box);
			temp.Z = Vec3D::roundOff_z(Rij, box);
			Vec3D minRij = Rij - temp*box;
			double r2 = minRij.getLengthSquared();
			double dist = sqrt(r2);

			if ( r2 < rc2 ) {
				Vec3D Fij;

				Fij.X = aii*( 1.0 - (dist/rcutoff) )*( minRij.X / dist);
				Fij.Y = aii*( 1.0 - (dist/rcutoff) )*( minRij.Y / dist);
				Fij.Z = aii*( 1.0 - (dist/rcutoff) )*( minRij.Z / dist);

				p->f += Fij;
				q->f += Fij*(-1.0); 

				// potential energy
				// double pair_pot_en = epsilon*(sigma - dist)*(sigma - dist);
				// double pair_pot_en = aii*( 1.0  - (dist/rcutoff) )*( 1.0  - (dist/rcutoff) );
				double pair_pot_en = (aii/2.0)*(1.0 - (dist/rcutoff))*( 1.0 - (dist/rcutoff) ) ;
				pot_en += pair_pot_en - ecut;

				// non-ideal comp pressure
				double nonIdealcomp = Vec3D::dot(minRij, Fij)*(1.0/(dim*volume));
				pressure += nonIdealcomp;

			}		

		}// computeForce() -- between 2 particles

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

		// Integrating equations of motion
		void integrateEOS(){

			// create linked list -- switch on only if purely dissipative
			createList();

			// intermediate velocity calculation + position update
			for (Particle&p : particles){
				p.v += (0.5)*(1/p.m)*p.f*dt;
				p.r += p.v*dt;}

			// apply periodic boundary condition and reset force to 0
			pbc();
		
			// set all forces to zero
			for (Particle& p : particles) {
				p.f.setZero();
			}

			// update force for new positions
			// forceCalc_conservative();

			// intermediate velocity calculation 2
			for (Particle&p : particles)
				p.v += 0.5*(1/p.m)*p.f*dt;

			// Andersen momentum-conserving thermostat with a probability 
			double randThermalize = ((double) rand() / (RAND_MAX));
			if ( randThermalize < thermProb ) forceCalc_dissipative();

			// calculate physical quantities
			for (Particle&p : particles){

				// calculate the kinetic energy
				kin_en += 0.5*p.m*(p.v.getLengthSquared());

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

					tempSum += temp;
					tempCount += 1;
				}	
			}

			// ideal component of pressure
			temp = 2.0*kin_en/dof;
			double idealComp = rho*1.0*temp; 
			pressure += idealComp;
		}

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

		// Reset variables
		void resetVar(){
			// energy reset to zero
			pot_en = 0.0;
			kin_en = 0.0;
			tot_en = 0.0;
			pressure = 0.0;
			momX = 0.0;
			momY = 0.0;
			momZ = 0.0;

			// set all forces to zero
			for (Particle& p : particles) {
				p.f.setZero();
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
				enStats << pot_en << "\t" << kin_en << "\t" << tot_en << std::endl;
				eosStats << rho << "\t" << temp << "\t" << pressure << std::endl;
				momStats << momX << "\t" << momY << "\t" << momZ << std::endl;
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

#endif
