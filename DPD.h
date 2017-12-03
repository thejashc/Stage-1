#ifndef _DPD_
#define _DPD_

#include <vector>
#include <fstream>
#include "Particle.h"

//declare DPD solver
class DPD {
	public:
		//global parameters
		double box;				// size of domain
		double epsilon;				// well-depth in Lennard-Jones po
		double sigma;				// distance at which potential is
		double rcutoff;				// cut-off distance
		double rc2;				// square of the cutoff distance
		double dt;				// time step

		double step;				// counter for number of steps 
		double stepMax;				// total number of steps

		unsigned int saveCount = 1;	     	//number of timestep between saves
		std::vector<Particle> particles;     	//vector of particles

		/*
		// function to initialize position
		void initialize_position(){

		for (Particle& p : particles) {
		// update position (integral time step) using the velocities (mid-step)
		setZero(p.r);				
		}
		}*/

		//solve contains the time stepping algorithm and the output routines 
		void solve () {
			//initialise time, and counter/ofstream for data output
			double t=0;
			unsigned int counter = 0;
			std::ofstream file("data");
			while (step<stepMax) {

				//loop over all contacts p=1..N, q=p+1..N to evaluate forces
				for (auto p = particles.begin();  p!=particles.end(); ++p)
					for (auto q = p+1;  q!=particles.end(); ++q) {
						// double distance = sqrt((p->r-q->r).lengthSquared());
						// double overlap = p->a+q->a-distance;
						// if (overlap>0) {

						Vec2D Rij = p->r - q->r;	
						Vec2D temp;
						temp.X = Vec2D::roundOff_x(Rij, box);
						temp.Y = Vec2D::roundOff_y(Rij, box);
						Vec2D minRij = Rij - temp*box;
						double r2 = minRij.getLengthSquared();

						if ( r2 < rc2 ) {
							double sig6 = pow(sigma,6);
							double r2i = 1/r2;
							double r6i = pow(r2i,3);
							double ff = 48*epsilon*sig6*r2i*r6i*(r6i - 0.5);
							p->f += ff*minRij;
							q->f += -1.0*ff*minRij; 
						}		
						else{
							p->f += 0;
							q->f += 0;
						}
					}

					// Integrate equations of motion using Verlet leap_frog method
					for (Particle& p : particles) {
						// store velocity (mid-step)
						Vec2D w_old = p.w;

						// update velocities (mid-step)
						p.w += p.f*(dt/p.m);

						// update position (integral time step) using the velocities (mid-step)
						p.r += p.w*dt;				

						// calculate velocity (integral time step)
						p.v = 0.5*(w_old + p.w);

					}

					// Apply periodic boundary conditions
					for (Particle& p : particles) {
	
						Vec2D temp;
						temp.X = Vec2D::roundOff_x(p.r, box);
						temp.Y = Vec2D::roundOff_y(p.r, box);
						p.r  = p.r - temp*box;				

					}

					// increment time
					step += 1;

					// print particle positions and velocities
					for (Particle& p : particles) {
						// update position (integral time step) using the velocities (mid-step)
						file << p.r << "\n";				
					}

					} //end time loop
				file.close();
			} //end solve
		};

#endif
