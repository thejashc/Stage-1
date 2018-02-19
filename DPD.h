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

// configuration of particles
#define SPHERICAL_DROPLET 0
#define CYLINDER_DROPLET 1
#define CRYSTAL	0
#define RESTART	0

//declare DPD solver
class DPD {
	public:
		#include "vars_list.h"

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
			npart = particles.size();		// number of particles
			rho = npart/volume;			// density of system 
			dof = dim*(npart - 1);			// total degrees of freedom (momentum only, no energy conservation)
			inv_sqrt_dt = 1.0/std::sqrt(dt);	// inverse of square root of the time step
			half_dt = 0.5*dt;			// 0.5*dt to be used in the integrateEOM()
			half_dt_sqr = half_dt*dt;		// 0.5*dt*dt to be used in the integrateEOM()
			rd4 = pow(rd_cutoff, 4.0);		// fourth power of rd_cutoff
			piThirty = M_PI/30.0;			// Pi/30.0 
			k1 = piThirty*Aij;			// constant k1
			k2 = piThirty*rd4*Bij;			// constant k2

			counter = 0;					// initialize time, counter for file writing
			std::ofstream enStats("./data/en_data.dat");	// initialize file stream for energy
			std::ofstream eosStats("./data/eos_data.dat");	// pressure and temperature data
			std::ofstream momStats("./data/mom_data.dat");	// pressure and temperature data
			std::ofstream writeConfig;

			/* // debug
			   for (Particle& p : particles)
			   std::cout << "Position: " << p.r << ", Density: " << p.dens <<", conservative force: " << p.fC << std::endl;*/

			// force calculation
			// createGridList();
			// forceCalc();
			// resetVar();

			createGridList();
			dens_calculation();

			// writing the parameters to a file
			std::ofstream paraInfo("parainfo.txt");
			paraWrite(paraInfo);
			paraInfo.close();

			while (step<stepMax) {

				// force calculation
				forceCalc();

				/*// debug 
				  for (Particle&p : particles)
				  std::cout << "position: " << p.r << " and force: " << p.fC << std::endl;*/

				// Integrate equations of motion (including pbc)
				integrateEOM();

				// calculate density
				for (Particle& p : particles) {
					p.dens = 0.0;
				}
				// dens_calculate();
				createGridList();
				dens_calculation();

				// total energy and g(r) sample calculation
				// potential energy
				for (Particle& p : particles) {
					pot_en += k1*p.rhoBar + k2*pow(p.dens, 2.0); 	// sum of self-energies of all particles
				}
				tot_en = kin_en + pot_en;
				if ( (step > gR_tStart) && (step % gR_tDelta == 0) ) grSample();

				// filewriting 
				fileWrite(enStats, eosStats, momStats);
				// std::cout << "filewrite done" << std::endl;

				// reset variables to zero
				resetVar();

				// increment time step
				step += 1;

				// for (Particle& p : particles)
				//	std::cout << "Position: " << p.r << ", Density: " << p.dens <<", conservative force: " << p.fC << std::endl;

			}//end time loop

			// write position velocity stats 
			writeConfig.open ( "./restart/posvelrestartfile.dat", std::ios::binary | std::ios::out );	// example binary file
			finalposvelWrite( writeConfig );	
			writeConfig.close();

			// post-processing
			grCalc();	 
			velHistCalc();
			enStats.close();
			eosStats.close();
			momStats.close();

		} //void solve()

		//--------------------------------------- INITIALIZATION ROUTINE --------------------------------------//
		void init(){

			// boxEdges
			boxEdge[x] = 10.0;
			boxEdge[y] = 10.0;
			boxEdge[z] = 10.0;

			// Half box size
			boxHalve[x] = boxEdge[x] / 2.0;
			boxHalve[y] = boxEdge[y] / 2.0;
			boxHalve[z] = boxEdge[z] / 2.0;

			// Reciprocal box size
			boxRecip[x] = 1.0 / boxEdge[x];
			boxRecip[y] = 1.0 / boxEdge[y];
			boxRecip[z] = 1.0 / boxEdge[z];

			// defining instance of random number class
			// std::mt19937 gen(rd());

			#if SPHERICAL_DROPLET 
				#include "sphDropInit.h"
			#elif CYLINDER_DROPLET 
				#include "cylDropInit.h"
			#elif CRYSTAL
				#include "crystalInit.h"
			#elif RESTART 
				#include "restartConfig.h"
			#endif

			// Initialize the gR_nCount array
			for (i=0; i < gR_nElem ; ++i){
				gR_nCount.push_back(0.0);
			}	

			// Initialize the velocity histogram array
			for (i=0; i < velHist_bins ; ++i){
				velHistX.push_back(0);
				velHistY.push_back(0);
				velHistZ.push_back(0);
			}	

			Vec3D velAvg={0.0, 0.0, 0.0};
			// Remove excess velocity
			for (Particle& p: particles){
				velAvg += p.w/particles.size();
			}

			for (Particle& p: particles){
				p.w -= velAvg;
			}


			// initializing NrCells[3], scale	
			for ( i = 0 ; i < 3 ; i++ )
			{
				NrCells[i] = int( boxEdge[i] / rcutoff ); // cellnr runs from 0 to NrCells-1
				scale[i] = NrCells[i] * boxRecip[i] ;
				if ( NrCells[i] < 3 ) { std::cout << "*** NrCells[" << i << "] = " << NrCells[i] << std::endl ; abort(); }

				// std::cout << "NrCells[" << i << "] = " << NrCells[i] << std::endl;
			}

			// initializing mini[3], maxi[3]
			mini[x] = 0; maxi[x] = NrCells[x] - 1;
			mini[y] = 0; maxi[y] = NrCells[y] - 1;
			mini[z] = 0; maxi[z] = NrCells[z] - 1;

			// defining grid matrix 
			grid.resize( NrCells[x] );
			for ( i = 0 ; i < NrCells[x] ; ++i ) {
				grid[i].resize( NrCells[y] );
				for (  j = 0 ; j < NrCells[y] ; ++j ){
					grid[i][j].resize( NrCells[z] );
					for ( k = 0 ; k < NrCells[z] ; ++k ){
						grid[i][j][k].resize( MaxPerCell ); 	
					} // k 
				}// j
			}// i

			// defining periodN 
			MaxNrCells = std::max( std::max( NrCells[x], NrCells[y] ), NrCells[z]);
			periodN.resize( MaxNrCells + 2 );
			periodR.resize( MaxNrCells + 2 );
			for ( i = 0 ; i < MaxNrCells + 2 ; ++ i ){
				periodN[ i ].resize( 3 );
				periodR[ i ].resize( 3 );
			}

			for ( j = 0 ; j < 3 ; j++ )
			{
				periodN[0][j] = NrCells[j] - 1;          // left neighbour of leftmost cell
				periodR[0][j] = -boxEdge[j];       // correction to add to particle j in rij = ri - rj
				for ( i = 1 ;  i < NrCells[j] + 1 ; i++ )
				{ 
					periodN[i][j] = i - 1; // same cell
					periodR[i][j] = 0.;
				} // i
				periodN[NrCells[j] + 1][j] = 0;          // right neigbour of rightmost cell
				periodR[NrCells[j] + 1][j] = +boxEdge[j];
			} // j

		}//init

		//--------------------------------------- Density Calculation --------------------------------------//
		void dens_calculation(){


			for ( mi[x] = 0 ; mi[x] < NrCells[x] ; ++mi[x] )
				for ( mi[y] = 0 ; mi[y] < NrCells[y] ; ++mi[y] )
					for ( mi[z] = 0 ; mi[z] < NrCells[z] ; ++mi[z] )
						for ( ii = 1 ; ii <= grid[mi[x]][mi[y]][mi[z]][0] ; ++ii ){
							i = grid[mi[x]][mi[y]][mi[z]][ii];
							// printf("i  %i %i %i %i %i \n",mix,miy,miz,ii,i);

							// particle j in same cell as i
							dR.setZero();
							for ( jj = ii + 1 ; jj <= grid[mi[x]][mi[y]][mi[z]][0] ; ++jj )
							{
								j = grid[mi[x]][mi[y]][mi[z]][jj];
								// std::cout << "j1 "<<  mi[x] << " " << mi[y] << " " << mi[z] << " " << jj << " " << j << std::endl;

								#include "dens_calculate.h"

							} // jj

							// particle j in neighbour cell to i
							for ( m = 0 ; m < 13 ; m++ )
							{
								mj[x]	     = periodN[ mi[x] + dm[m][x] + 1 ][x];
								mj[y]	     = periodN[ mi[y] + dm[m][y] + 1 ][y];
								mj[z]	     = periodN[ mi[z] + dm[m][z] + 1 ][z];
								dR.X	     = periodR[ mi[x] + dm[m][x] + 1 ][x];
								dR.Y	     = periodR[ mi[y] + dm[m][y] + 1 ][y];
								dR.Z	     = periodR[ mi[z] + dm[m][z] + 1 ][z];
								for ( jj = 1 ; jj <= grid[mj[x]][mj[y]][mj[z]][0] ; ++jj )
								{
									j = grid[mj[x]][mj[y]][mj[z]][jj];
									// std::cout << "j2 " << m << " " << mj[x] << " " << mj[y] << " " << mj[z] << " " << jj << " " << j << std::endl;

									#include "dens_calculate.h"

								} // jj
							} // m
						} // ii


		}// dens_calculation

		//--------------------------------------- Grid List creation--------------------------------------//
		void createGridList(){


			// initialize grid to 0
			for ( mi[x] = 0 ; mi[x] < NrCells[x] ; mi[x]++ )
				for ( mi[y] = 0 ; mi[y] < NrCells[y] ; mi[y]++ )
					for ( mi[z] = 0 ; mi[z] < NrCells[z] ; mi[z]++ )
						grid[mi[x]][mi[y]][mi[z]][0] = 0;

			for (i = 0; i < npart; ++i)
			{ 
				mi[x] = int( particles[i].r.getComponent(x) * scale[x] );
				mi[y] = int( particles[i].r.getComponent(y) * scale[y] );
				mi[z] = int( particles[i].r.getComponent(z) * scale[z] );
				// std::cout << "position of particle " << i << " =" << particles[i].r;
				// std::cout << ", mi[x], mi[y], mi[z] =  " << mi[x] << ", " << mi[y] << ", " << mi[z] << std::endl;
				if ( mi[x] < mini[x] || mi[x] > maxi[x]   // debug
						|| mi[y] < mini[y] || mi[y] > maxi[y] 
						|| mi[z] < mini[z] || mi[z] > maxi[z] )
				{ 
					// std::cout << "*** particle " << i << " outside box" << std::endl;
					std::cout << mini[x] << " < " << mi[x] << " < " << maxi[x] << std::endl;
					std::cout << mini[y] << " < " << mi[y] << " < " << maxi[y] << std::endl;
					std::cout << mini[z] << " < " << mi[z] << " < " << maxi[z] << std::endl;
					abort();
				}
				if ( grid[mi[x]][mi[y]][mi[z]][0] == MaxPerCell )
				{ 
					std::cout << "*** cell overfull" << std::endl;
					std::cout << mi[x] << "  " << mi[y] << "  " << mi[z] << std::endl;
					abort();
				}
				grid[mi[x]][mi[y]][mi[z]][0] ++ ;
				grid[mi[x]][mi[y]][mi[z]][ grid[mi[x]][mi[y]][mi[z]][0] ] = i;
			} // i
		}

		//--------------------------------------- Force Calculation --------------------------------------//
		void forceCalc(){

			for ( mi[x] = 0 ; mi[x] < NrCells[x] ; ++mi[x] )
				for ( mi[y] = 0 ; mi[y] < NrCells[y] ; ++mi[y] )
					for ( mi[z] = 0 ; mi[z] < NrCells[z] ; ++mi[z] )
						for ( ii = 1 ; ii <= grid[mi[x]][mi[y]][mi[z]][0] ; ++ii ){
							i = grid[mi[x]][mi[y]][mi[z]][ii];
							// printf("i  %i %i %i %i %i \n",mix,miy,miz,ii,i);

							// particle j in same cell as i
							dR.setZero();
							for ( jj = ii + 1 ; jj <= grid[mi[x]][mi[y]][mi[z]][0] ; ++jj )
							{
								j = grid[mi[x]][mi[y]][mi[z]][jj];
								// std::cout << "j1 "<<  mi[x] << " " << mi[y] << " " << mi[z] << " " << jj << " " << j << std::endl;

								#include "pairforce.h"

							} // jj

							// particle j in neighbour cell to i
							for ( m = 0 ; m < 13 ; m++ )
							{
								mj[x]	     = periodN[ mi[x] + dm[m][x] + 1 ][x];
								mj[y]	     = periodN[ mi[y] + dm[m][y] + 1 ][y];
								mj[z]	     = periodN[ mi[z] + dm[m][z] + 1 ][z];
								dR.X	     = periodR[ mi[x] + dm[m][x] + 1 ][x];
								dR.Y	     = periodR[ mi[y] + dm[m][y] + 1 ][y];
								dR.Z	     = periodR[ mi[z] + dm[m][z] + 1 ][z];
								for ( jj = 1 ; jj <= grid[mj[x]][mj[y]][mj[z]][0] ; ++jj )
								{
									j = grid[mj[x]][mj[y]][mj[z]][jj];
									// std::cout << "j2 " << m << " " << mj[x] << " " << mj[y] << " " << mj[z] << " " << jj << " " << j << std::endl;

									#include "pairforce.h"

								} // jj
							} // m
						} // ii
		}

		//--------------------------------------- Integrate Equations of motion --------------------------------------//
		void integrateEOM(){
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
					ivelX = ceil((velHist_velMax - p.v.X)/velHist_velDelta); 
					ivelY = ceil((velHist_velMax - p.v.Y)/velHist_velDelta); 
					ivelZ = ceil((velHist_velMax - p.v.Z)/velHist_velDelta); 

					velHistX[ivelX] += 1;
					velHistY[ivelY] += 1;
					velHistZ[ivelZ] += 1;

					tempSum += temp;
					tempCount += 1;
				}

				// implement periodic boundary condition 
				#include "pbc.h"

				// calculate the kinetic energy
				kin_en += 0.5*p.m*(p.v.getLengthSquared());
			}
			// ideal component of pressure
			temp = 2*kin_en/dof;
			idealComp = rho*1.0*temp; 
			pressure += idealComp;
		}
		//--------------------------------------- Resetting variables--------------------------------------//
		void resetVar(){
			// energy reset to zero
			pot_en = 0.0;
			kin_en = 0.0;
			tot_en = 0.0;
			vsqrSum = 0.0;
			pressure = 0.0;
			momX = 0.0;
			momY = 0.0;
			momZ = 0.0;

			for (Particle& p : particles){
				// p.dens = p.dens_new;
				// p.dens_new = 0.0;
				p.rhoBar = 0.0;
				p.fC.setZero();
				p.fR.setZero();
				p.fD.setZero();
			}
		}

		//--------------------------------------- g(r) sampling --------------------------------------//
		void grSample(){

			//loop over all contacts p=1..N-1, q=p+1..N to evaluate forces
			for (auto p = particles.begin();  p!=particles.end()-1; ++p){
				for (auto q = p+1;  q!=particles.end(); ++q) {

					Rij = p->r - q->r;	
					tempVec.X = Vec3D::roundOff_x(Rij, box);
					tempVec.Y = Vec3D::roundOff_y(Rij, box);
					tempVec.Z = Vec3D::roundOff_z(Rij, box);
					minRij.X = Rij.X - tempVec.X*boxEdge[x];
					minRij.Y = Rij.Y - tempVec.Y*boxEdge[y];
					minRij.Z = Rij.Z - tempVec.Z*boxEdge[z];
					r2 = minRij.getLengthSquared();
					dist = std::sqrt(r2);

					if ( dist < box/2.0 ) { 
						ig = ceil(dist/gR_radDelta)-1;

						if ( (ig < 0) || (ig >= gR_nElem)) {std::cout << " out of bounds " << std::endl;} 

						gR_nCount[ig] += 2.0;	
					}
				}
			}
		}

		//--------------------------------------- g(r) calculation --------------------------------------//
		// Structure function g(r) calculation
		void grCalc(){

			// write the g(r) data to a file
			std::ofstream grWrite("./data/gr_data.dat");
			grWrite << "ri \t ro \t rad \t gr_count \t tot_part \t samples \t nHomo \t rho" << std::endl;

			for (i=0; i < gR_nElem; ++i){

				rad = gR_radDelta*( i + 0.5) + gR_radMin;		// radius in question	
				ri = i*gR_radDelta + gR_radMin;				// radius at i^th bin
				ro = (i+1)*gR_radDelta + gR_radMin;				// radius at (i+1)^th bin
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

		//--------------------------------------- velocity histogram calculation --------------------------------------//
		void velHistCalc(){

			std::ofstream velDistdata("./data/velDist_data.dat"); 
			velDistdata << "velBin" << "\t" << "AvgTemp" << "\t" << "velHistX" << "\t" << "trapzAreaX" << "\t" << "velHistY" << "\t" << "trapzAreaY" << "\t" << "velHistZ[i]" << "\t" << "trapzAreaZ" << std::endl;	

			trapzAreaX = 0.0;
			trapzAreaY = 0.0;
			trapzAreaZ = 0.0;

			for (i=1; i <= velHist_bins-2; ++i){

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
			for (i=0; i< velHist_bins; ++i){
				bin_lower = velHist_velMin + i*velHist_velDelta;
				bin_upper = velHist_velMin + (i+1)*velHist_velDelta;
				vel = (bin_lower + bin_upper)*0.5;
				tempAv = tempSum/tempCount;

				velDistdata << vel << "\t" << tempAv << "\t" << velHistX[i] << "\t" << trapzAreaX << "\t" << velHistY[i] << "\t" << trapzAreaY << "\t" << velHistZ[i] << "\t" << trapzAreaZ << std::endl;	
			}

			velDistdata.close();
		}

		//--------------------------------------- Parameter file writing--------------------------------------//
		void paraWrite(std::ofstream& paraInfo){

			paraInfo << "set temperature (kbT)  	" << "\t \t" << std::setw(20) << std::setprecision(15) << kBT << std::endl;
			// paraInfo << "droplet radius (rad)  	" << "\t \t" << std::setw(20) << std::setprecision(15) << sqrt(radSqr) << std::endl;
			paraInfo << "box length (box)   	" << "\t \t" << std::setw(20) << std::setprecision(15)<< box << std::endl;
			paraInfo << "cutoff attr (rcutoff)	" << "\t \t" << std::setw(20) << std::setprecision(15) << rcutoff << std::endl;
			paraInfo << "cutoff rep (rd_cutoff)	" << "\t \t" << std::setw(20) << std::setprecision(15) << rd_cutoff << std::endl;
			paraInfo << "dimensions (dim)		" << "\t \t" << std::setw(20) << std::setprecision(15) << dim << std::endl;
			paraInfo << "density (rho)		" << "\t \t" << std::setw(20) << std::setprecision(15) << rho << std::endl;
			paraInfo << "timestep (dt)		" << "\t \t" << std::setw(20) << std::setprecision(15) << dt << std::endl;
			paraInfo << "number of particles (npart)" << "\t \t" << std::setw(20) << std::setprecision(15) << npart << std::endl;
			paraInfo << "total run time (stepMax)   " << "\t \t" << std::setw(20) << std::setprecision(15) << stepMax << std::endl;
			paraInfo << "Data frequency (saveCount) " << "\t \t" << std::setw(20) << std::setprecision(15) << saveCount << std::endl;
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

		//--------------------------------------- Velocity, Momentum and Pressure file writing--------------------------------------//
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

		//--------------------------------------- VTK file writing routine--------------------------------------//
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

		//-------------------- Final velocity and positions ----------------------//
		void finalposvelWrite( std::ofstream& writeConfig ){

			writeConfig.write( reinterpret_cast< const char * >( &npart ), sizeof( npart ) );

			for (Particle& p : particles){
				writeConfig.write( reinterpret_cast< const char * >( &p.r.X ), sizeof( p.r.X ) );
				//writeConfig.write( &tab, sizeof( tab ) );
				writeConfig.write( reinterpret_cast< const char * >( &p.r.Y ), sizeof( p.r.Y ) );
				//writeConfig.write( &tab, sizeof( tab ) );
				writeConfig.write( reinterpret_cast< const char * >( &p.r.Z ), sizeof( p.r.Z ) );
				//writeConfig.write( &tab, sizeof( tab ) );
				writeConfig.write( reinterpret_cast< const char * >( &p.w.X ), sizeof( p.w.X ) );
				//writeConfig.write( &tab, sizeof( tab ) );
				writeConfig.write( reinterpret_cast< const char * >( &p.w.Y ), sizeof( p.w.Y ) );
				//writeConfig.write( &tab, sizeof( tab ) );
				writeConfig.write( reinterpret_cast< const char * >( &p.w.Z ), sizeof( p.w.Z ) );
				//myFile.write( &newline, sizeof( newline ) );
			}

		}
};

#endif
