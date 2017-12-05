#ifndef _DPD_
#define _DPD_

#include <vector>
#include <fstream>
#include <cstdlib>
#include "Particle.h"
#include <ctime>

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

		int step;				// counter for number of steps 
		double stepMax;				// total number of steps
		double pot_en;				// system potential energy
		double kin_en;				// system kinetic energy
		double tot_en;				// system total energy

		unsigned int saveCount = 500;	     	//number of timestep between saves
		std::vector<Particle> particles;     	//vector of particles

		/******************************************************************************************************/
		/**************************************** SOLVE ROUTINE ***********************************************/
		/******************************************************************************************************/
		//solve contains the time stepping algorithm and the output routines 
		void solve () {
			// cut-off correction to potential
			double sig6 = pow(sigma,6);
			double rc2i = 1/rc2;
			double rc6i = pow(rc2i,3);
			double ecut = 4*epsilon*sig6*rc6i*(sig6*rc6i - 1.0);

			//initialise time, and counter/ofstream for data output
			double t=0;
			unsigned int counter = 0;

			std::ofstream file1("./data/en_data.dat");

			while (step<stepMax) {

				// force calculation
				// forceCalc();
				//loop over all contacts p=1..N-1, q=p+1..N to evaluate forces
				for (auto p = particles.begin();  p!=particles.end()-1; ++p){
					// for_loop_inner_counter = 0;
					for (auto q = p+1;  q!=particles.end(); ++q) {

						Vec3D Rij = p->r - q->r;	
						Vec3D temp;
						temp.X = Vec3D::roundOff_x(Rij, box);
						temp.Y = Vec3D::roundOff_y(Rij, box);
						temp.Z = Vec3D::roundOff_z(Rij, box);
						Vec3D minRij = Rij - temp*box;
						double r2 = minRij.getLengthSquared();

						if ( r2 < rc2 ) {
							double r2i = 1/r2;
							double r6i = pow(r2i,3);
							double ff = 48.0*epsilon*sig6*r2i*r6i*(r6i - 0.5);
							p->f += ff*minRij;
							q->f += -1.0*ff*minRij; 

							// calculate the potential energy
							double pair_pot_en = 4.0*epsilon*sig6*r6i*(sig6*r6i - 1.0);
							pot_en += pair_pot_en - ecut;
						}		


					}

				}

				// Integrate equations of motion using Verlet leap_frog method
				// integrateEOS();
				for (Particle& p : particles) {
					// store velocity (mid-step)
					Vec3D w_old = p.w;

					// update velocities (mid-step)
					p.w += p.f*(dt/p.m);

					// update position (integral time step) using the velocities (mid-step)
					p.r += p.w*dt;				

					// calculate velocity (integral time step)
					p.v = 0.5*(w_old + p.w);

					// calculate the kinetic energy
					kin_en += 0.5*p.m*(p.v.getLengthSquared());

				}


				// total energy
				tot_en = kin_en + pot_en;

				// Apply periodic boundary conditions
				// pbc();
				for (Particle& p : particles) {

					Vec3D temp;
					temp.X = Vec3D::roundOff_x(p.r, box);
					temp.Y = Vec3D::roundOff_y(p.r, box);
					temp.Z = Vec3D::roundOff_z(p.r, box);
					p.r  = p.r - temp*box;				

				}					

				//write output file in the .data format every 10th time step
				if (++counter>=saveCount) {
					//reset the counter, write time to terminal
					counter = 0;

					if ( step % 10000 == 0){
						std::cout<< step << " steps out of " << stepMax << " completed " << std::endl;
					}

					// particle positions in vtk file format
					vtkFileWrite();

					//writing the energy balance
					file1<< pot_en << " " << kin_en << " " << tot_en << std::endl;
				}

				// reset energy and forces
				// resetVar();
				// energy reset to zero
				pot_en = 0.0;
				kin_en = 0.0;
				tot_en = 0.0;

				// set all forces to zero
				for (Particle& p : particles) {
					p.f.setZero();
				}

				// increment time
				step += 1;

			} //end time loop
			file1.close();
		} //end solve


		/******************************************************************************************************/
		/**************************************** FUNCTIONS ***************************************************/
		/******************************************************************************************************/
		/*
		// Force Calculation
		void forceCalc(){
			//loop over all contacts p=1..N-1, q=p+1..N to evaluate forces
			for (auto p = particles.begin();  p!=particles.end()-1; ++p){
				// for_loop_inner_counter = 0;
				for (auto q = p+1;  q!=particles.end(); ++q) {

					Vec3D Rij = p->r - q->r;	
					Vec3D temp;
					temp.X = Vec3D::roundOff_x(Rij, box);
					temp.Y = Vec3D::roundOff_y(Rij, box);
					Vec3D minRij = Rij - temp*box;
					double r2 = minRij.getLengthSquared();

					if ( r2 < rc2 ) {
						double r2i = 1/r2;
						double r6i = pow(r2i,3);
						double ff = 48.0*epsilon*sig6*r2i*r6i*(r6i - 0.5);
						p->f += ff*minRij;
						q->f += -1.0*ff*minRij; 

						// calculate the potential energy
						double pair_pot_en = 4.0*epsilon*sig6*r6i*(sig6*r6i - 1.0);
						pot_en += pair_pot_en - ecut;
					}		


				}

			}

		}

		// Integrating equations of motion -- Verlet Leap Frog scheme
		void integrateEOS(){
			for (Particle& p : particles) {
				// store velocity (mid-step)
				Vec3D w_old = p.w;

				// update velocities (mid-step)
				p.w += p.f*(dt/p.m);

				// update position (integral time step) using the velocities (mid-step)
				p.r += p.w*dt;				

				// calculate velocity (integral time step)
				p.v = 0.5*(w_old + p.w);

				// calculate the kinetic energy
				kin_en += 0.5*p.m*(p.v.getLengthSquared());

			}

		}

		// Periodic boundary conditions
		void pbc(){
			for (Particle& p : particles) {

				Vec3D temp;
				temp.X = Vec3D::roundOff_x(p.r, box);
				temp.Y = Vec3D::roundOff_y(p.r, box);
				p.r  = p.r - temp*box;				

			}					

		}

		// Reset variables
		void resetVar(){
			// energy reset to zero
			pot_en = 0.0;
			kin_en = 0.0;
			tot_en = 0.0;

			// set all forces to zero
			for (Particle& p : particles) {
				p.f.setZero();
			}
		}*/

		// Filewriting for the VTK format
		void vtkFileWrite(){

			char filename[40];

			sprintf( filename, "./data/data_%d.vtu", step);  
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
