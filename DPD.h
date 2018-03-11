#ifndef _DPD_
#define _DPD_

#include <vector>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <cstring>
#include <cstdlib>
#include "Particle.h"
#include <unistd.h>
#include <ctime>
#include <random> 

// configuration of particles
#define RANDOM_DISSIPATIVE 1
#define SPHERICAL_DROPLET 0
#define SPHERICAL_CAP 0
#define CYLINDER_DROPLET 0
#define PLANAR_SLAB 1
#define CRYSTAL	0
#define RESTART	0
#define WALL_ON 1
#define LOWER_WALL_ON 1
#define UPPER_WALL_ON 1

//declare DPD solver
class DPD {
	public:
		#include "vars_list.h"

		/******************************************************************************************************/
		/**************************************** SOLVE ROUTINE ***********************************************/
		/******************************************************************************************************/
		//solve contains the time stepping algorithm and the output routines 
		void solve () {

			std::cout << " Proccess ID : " << getpid() << std::endl;
			// initialize position, velocity, hoc, ll
			init();

			// parameters declaration
			step = 1;
			temp = 0.0;
			tempSum = 0.0;
			tempAv = 0.0;
			tempCount = 0;
			volume = boxEdge[x] * boxEdge[y] * boxEdge[z];		// system volume
			npart = particles.size();				// number of particles
			rho = npart/volume;					// density of system 
			dof = dim*(npart - 1);					// total degrees of freedom (momentum only, no energy conservation)
			inv_sqrt_dt = 1.0/std::sqrt(dt);			// inverse of square root of the time step
			half_dt = 0.5*dt;					// 0.5*dt to be used in the integrateEOM()
			half_dt_sqr = half_dt*dt;				// 0.5*dt*dt to be used in the integrateEOM()
			rd4 = pow(rd_cutoff, 4.0);				// fourth power of rd_cutoff
			piThirty = M_PI/30.0;					// Pi/30.0 
			k1 = piThirty*All;					// constant k1
			k2 = piThirty*rd4*Bll;					// constant k2

			counter = 0;						// initialize time, counter for file writing
			std::ofstream enStats 	( "./data/en_data.dat"	);	// Kinetic, Potential and total energy
			std::ofstream eosStats	( "./data/eos_data.dat"	);	// Mean Pressure and temperature data
			std::ofstream pTensStats( "./data/pTens.dat"	);	// Pressure tensor data
			std::ofstream momStats	( "./data/mom_data.dat"	);	// pressure and temperature data
			std::ofstream writeConfig;

			/*
			createGridList();
			dens_calculation();*/ // everytime force calculation

			createGridList();
			forceCalc();
	
			resetVar();

			
			// write parameters and initial configuration
			vtkFileWritePosVel();
			std::ofstream paraInfo( "param.out" );
			paraWrite(paraInfo);
			paraInfo.close();

			#if WALL_ON
			if ( rd_cutoff > rcWcutoff ){
				std::cout << " The cutoff for density calculation of fluid near wall needs to be checked " << std::endl; 
				abort();
			}
			#endif

			std::cout << " ********************* STARTING SIMULATION ************************* " << std::endl;	

			while (step<stepMax) {

				// force calculation
				createGridList();
				forceCalc();

				// for (i = fluid_index[0] ; i <= fluid_index[fluidCount-1] ; ++i)
				//	  std::cout << "i= " << i << ", position: " <<  particles[i].r << ", type: " << particles[i].type << ", wall force: " << particles[i].fCW << std::endl;

				// Integrate equations of motion (including pbc)
				integrateEOM();

				// calculate density
				/*
				// for (Particle& p : particles) {
				// 	p.dens = 0.0;
				// }
				// dens_calculate();
				//createGridList();
				// dens_calculation();*/ // Everytime density calculation

				// total energy and g(r) sample calculation
				// potential energy
				for (Particle& p : particles) {
					pot_en += k1*p.rhoBar + k2*pow(p.dens, 2.0); 	// sum of self-energies of all particles
				}
				tot_en = kin_en + pot_en;
				if ( (step > gR_tStart) && (step % gR_tDelta == 0) ) grSample();

				// filewriting 
				fileWrite(enStats, eosStats, momStats, pTensStats);
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
			
			std::cout << " ********************* ENDING SIMULATION ************************* " << std::endl;	

		} //void solve()

		//--------------------------------------- INITIALIZATION ROUTINE --------------------------------------//
		void init(){


			#include "paramIn.h"

			// defining instance of random number class
			// std::mt19937 gen(rd());

			#if WALL_ON
				#include "defineWall.h"

				#if SPHERICAL_CAP
					#include "sphericalCap.h"			
				#elif CYLINDER_DROPLET
					#include "cylDropInit.h"
				#elif PLANAR_SLAB
					#include "planarSlabInit.h" 
				#endif
			#else
				#if SPHERICAL_DROPLET 
					#include "sphDropInit.h"
				#elif PLANAR_SLAB
					#include "planarSlabInit.h"
				#elif CYLINDER_DROPLET 
					#include "cylDropInit.h"
				#elif CRYSTAL
					#include "crystalInit.h"
				#elif RESTART 
					#include "restartConfig.h"
				#endif
			#endif // WALL_ON

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

			#if PLANAR_SLAB
				// Initialize the rhoZ array
				for ( i=0; i < rhoZ_bins ; ++i ){
					rhoZ.push_back(0.0);
				}	
			#elif CYLINDER_DROPLET
				// Initialize the rhor array
				for ( i=0; i < rhor_bins ; ++i ){
					rhor.push_back(0.0);
				}	
			#endif

			Vec3D velAvg={0.0, 0.0, 0.0};
			// Remove excess velocity and set particle density to 0
			for (Particle& p: particles){
				velAvg 		+= p.w/particles.size();
				p.dens 		 = 0.0;
				p.dens_new	 = 0.0;
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

		
			// Initialize Ideal, Non-Ideal and complete Pressure tensor
			pIdeal[0][0] = 0.0;
			pIdeal[0][1] = 0.0;
			pIdeal[0][2] = 0.0;
			pIdeal[1][0] = 0.0;
			pIdeal[1][1] = 0.0;
			pIdeal[1][2] = 0.0;
			pIdeal[2][0] = 0.0;
			pIdeal[2][1] = 0.0;
			pIdeal[2][2] = 0.0;

			pNonIdeal[0][0] = 0.0;
			pNonIdeal[0][1] = 0.0;
			pNonIdeal[0][2] = 0.0;
			pNonIdeal[1][0] = 0.0;
			pNonIdeal[1][1] = 0.0;
			pNonIdeal[1][2] = 0.0;
			pNonIdeal[2][0] = 0.0;
			pNonIdeal[2][1] = 0.0;
			pNonIdeal[2][2] = 0.0;

			pTensor[0][0] = 0.0;
			pTensor[0][1] = 0.0;
			pTensor[0][2] = 0.0;
			pTensor[1][0] = 0.0;
			pTensor[1][1] = 0.0;
			pTensor[1][2] = 0.0;
			pTensor[2][0] = 0.0;
			pTensor[2][1] = 0.0;
			pTensor[2][2] = 0.0;

			// find indices of all liquid particles
			fluidCount = 0;
			for ( i = 0; i < particles.size(); ++i ){
				if ( particles[i].type == 1 ){
					fluid_index.push_back(i);
					fluidCount++;
				}
				else
					solid_index.push_back(i);

			}

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
					std::cout << "*** particle " << i << " outside box in step: " << step << std::endl;
					std::cout << "*** position: " << particles[i].r << std::endl;
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


								#if WALL_ON
								if ( particles[i].type == 1 && particles[j].type == 1 ){
										#include "pairforceLL.h"
								} // liquid liquid interaction
								else if ( particles[i].type == 0 && particles[j].type == 0 ){
										#include "pairforceSS.h"
								} // solid solid interaction
								else{
										#include "pairforceSL.h"
								} // solid liquid interaction
								#else
									#include "pairforceLL.h"
								#endif
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


									#if WALL_ON
									if ( particles[i].type == 1 && particles[j].type == 1 ){
											#include "pairforceLL.h"
									} // liquid liquid interaction
									else if ( particles[i].type == 0 && particles[j].type == 0 ){
										#include "pairforceSS.h"
									}// solid solid interaction
									else{
										#include "pairforceSL.h"
									} // solid liquid interaction
									#else 
										#include "pairforceLL.h"
									#endif
								} // jj
							} // m
						} // ii
		}

		//--------------------------------------- Integrate Equations of motion --------------------------------------//
		void integrateEOM(){
			for ( i = fluid_index[0] ; i <= fluid_index[fluidCount-1] ; ++i ) {
				// store velocity (mid-step)
				Vec3D w_old = particles[i].w;

				// update velocities (mid-step)
				#if RANDOM_DISSIPATIVE
					#if WALL_ON
						particles[i].w += ( particles[i].fC + particles[i].fCW +  particles[i].fD + particles[i].fR )*(dt/particles[i].m);
					#else	
						particles[i].w += ( particles[i].fC + particles[i].fD + particles[i].fR )*(dt/particles[i].m);
					#endif
				#else 
					#if WALL_ON
						particles[i].w += ( particles[i].fC + particles[i].fCW )*(dt/particles[i].m);
					#else
						particles[i].w += ( particles[i].fC )*(dt/particles[i].m);
					#endif
				#endif 

				// update position (integral time step) using the velocities (mid-step)
				particles[i].r += particles[i].w*dt;				

				// calculate velocity (integral time step)
				particles[i].v = 0.5*( w_old + particles[i].w );

				if ( particles[i].r.Z < 1.89 || particles[i].r.Z > 8. )
					std::cout << " Particle " << i << " is outside " << particles[i].r << " at step " << step << std::endl;

				// implement periodic boundary condition 
				#include "pbc.h"

				// distribute velocities into velocity bins
				if ( (step > velHist_tStart) && (step % velHist_tDelta == 0) ) {
					ivelX = ceil( ( velHist_velMax - particles[i].v.X )/ velHist_velDelta ); 
					ivelY = ceil( ( velHist_velMax - particles[i].v.Y )/ velHist_velDelta ); 
					ivelZ = ceil( ( velHist_velMax - particles[i].v.Z )/ velHist_velDelta ); 

					velHistX[ivelX] += 1;
					velHistY[ivelY] += 1;
					velHistZ[ivelZ] += 1;
				}

				#if PLANAR_SLAB
				// calculate density profile
				iRhoZ = ceil ( ( particles[i].r.getComponent(z) - rhoZ_Zmin ) / rhoZ_Zdelta ) - 1;		

				if ( iRhoZ < 0 || iRhoZ > rhoZ_bins  ) { std::cout << " rhoZ calculation -- planar slab particle out of bounds" << std::endl; abort(); } 						
				rhoZ[ iRhoZ ] += 1;

				#elif CYLINDER_DROPLET
				// calculate radial density profile
				radPos 	= std::sqrt( pow( particles[i].r.X - xCOM, 2.0 ) + pow( particles[i].r.Y - yCOM, 2.0 ) );
				iRhor 	= round ( ( radPos - rhor_rmin ) / rhor_rdelta );

				if ( iRhor < 0 || iRhor > rhor_bins  ) { std::cout << " cylindrical particle out of bounds" << std::endl; abort(); } 						

				rhor[ iRhor ] += 1;
				#endif

				// calculate the kinetic energy
				vx2 += particles[i].v.X * particles[i].v.X;
				vy2 += particles[i].v.Y * particles[i].v.Y;
				vz2 += particles[i].v.Z * particles[i].v.Z;
			}

			// temperature calculation	
			v2 = ( vx2 + vy2 + vz2 ) / ( 3. * npart );
			kin_en = 0.5 * v2;		// unit mass assumption
			temp = 2.*kin_en;

			tempSum += temp;
			tempCount += 1;

			// calculation of  pressure tensor 
			#include "pTensCalc.h"
		} // run over all fluid particles
		//--------------------------------------- Resetting variables--------------------------------------//
		void resetVar(){
			// energy reset to zero
			pot_en = 0.0;
			kin_en = 0.0;
			tot_en = 0.0;
			vx2 = 0.0;
			vy2 = 0.0;
			vz2 = 0.0;
			v2 = 0.0;
			pressure = 0.0;
			momX = 0.0;
			momY = 0.0;
			momZ = 0.0;
	
			// std::cout << " fi[0], fi[fluidCount - 1], fluidCount " << fluid_index[0] << " " << fluid_index[1] << " " << fluidCount << std::endl;	
			for ( i = 0; i < npart ; ++i )
			{
				particles[i].dens = particles[i].dens_new;
				particles[i].dens_new = 0.0;
				particles[i].rhoBar = 0.0;
				particles[i].fC.setZero();
				particles[i].fR.setZero();
				particles[i].fD.setZero();

				#if WALL_ON
				particles[i].fCW.setZero();
				#endif 

			} // set density equal to zero for solid type particles

			// reset ideal and non-ideal component of pressure tensor
			/*
			   pIdeal[0][0] = 0.0;
			   pIdeal[0][1] = 0.0;
			   pIdeal[0][2] = 0.0;
			   pIdeal[1][0] = 0.0;
			   pIdeal[1][1] = 0.0;
			   pIdeal[1][2] = 0.0;
			   pIdeal[2][0] = 0.0;
			   pIdeal[2][1] = 0.0;
			   pIdeal[2][2] = 0.0;

			   pNonIdeal[0][0] = 0.0;
			   pNonIdeal[0][1] = 0.0;
			   pNonIdeal[0][2] = 0.0;
			   pNonIdeal[1][0] = 0.0;
			   pNonIdeal[1][1] = 0.0;
			   pNonIdeal[1][2] = 0.0;
			   pNonIdeal[2][0] = 0.0;
			   pNonIdeal[2][1] = 0.0;
			   pNonIdeal[2][2] = 0.0;
			 */
		}

		//--------------------------------------- g(r) sampling --------------------------------------//
		void grSample(){

			//loop over all contacts p=1..N-1, q=p+1..N to evaluate forces
			for ( i = fluid_index[0]; i <= fluid_index[fluidCount - 2] ; ++i ){
				for ( j = i + 1; j <= fluid_index[fluidCount - 1] ; ++j ){

					Rij = particles[i].r - particles[j].r;	
					tempVec.X = Vec3D::roundOff_x( Rij, boxEdge[x] );
					tempVec.Y = Vec3D::roundOff_y( Rij, boxEdge[y] );
					tempVec.Z = Vec3D::roundOff_z( Rij, boxEdge[z] );
					minRij.X = Rij.X - tempVec.X*boxEdge[x];
					minRij.Y = Rij.Y - tempVec.Y*boxEdge[y];
					minRij.Z = Rij.Z - tempVec.Z*boxEdge[z];
					r2 = minRij.getLengthSquared();
					dist = std::sqrt(r2);

					if ( dist < boxEdge[x] / 2.0 ) { 
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

			paraInfo << "---------------------------" << std::endl;
			paraInfo << "Simulation Box Parameters" << std::endl;
			paraInfo << "---------------------------" << std::endl;
			paraInfo << "Set temperature (kbT)                      :           " << kBT << std::endl;
			paraInfo << "Box length x (box)                         :           " << boxEdge[x] << std::endl;
			paraInfo << "Box length y (box)                         :           " << boxEdge[y] << std::endl;
			paraInfo << "Box length z (box)                         :           " << boxEdge[z] << std::endl;
			paraInfo << "Initial fluid density (initRho)            :           " << initRho << std::endl;
			paraInfo << "Cutoff attr (rcutoff)                      :           " << rcutoff << std::endl;
			paraInfo << "Cutoff rep (rd_cutoff)                     :           " << rd_cutoff << std::endl;
			paraInfo << "Dimensions (dim)                           :           " << dim << std::endl;
			paraInfo << "Density (rho)                              :           " << rho << std::endl;
			paraInfo << "Timestep (dt)                              :           " << dt << std::endl;
			paraInfo << "Number of fluid particles (fluidCount)     :           " << fluidCount << std::endl;
		
			paraInfo << "---------------------------" << std::endl;
			paraInfo << "Run & Data saving" << std::endl;
			paraInfo << "---------------------------" << std::endl;
			paraInfo << "Total run time (stepMax)                   :           " << stepMax << std::endl;
			paraInfo << "Data frequency (saveCount)                 :           " << saveCount << std::endl;
			
			#if SPHERICAL_DROPLET 
			paraInfo << "---------------------------" << std::endl;
			paraInfo << "Spherical Drop Parameters" << std::endl;
			paraInfo << "---------------------------" << std::endl;
			paraInfo << "Initial box dimension (dropBox)            :           " << dropBox << std::endl;
			#elif CYLINDER_DROPLET 
			paraInfo << "---------------------------" << std::endl;
			paraInfo << "Cylinder Parameters" << std::endl;
			paraInfo << "---------------------------" << std::endl;
			paraInfo << "Cylinder radius (cylRad)                   :           " << cylRad << std::endl;
			paraInfo << "Cylinder Height (cylHeight)                :           " << cylHeight << std::endl;
			paraInfo << "Cylinder center x-coord (cylCenterX)       :           " << cylCenterX << std::endl;
			paraInfo << "Cylinder center y-coord (cylCenterY)       :           " << cylCenterY << std::endl;
			#elif PLANAR_SLAB
			paraInfo << "---------------------------" << std::endl;
			paraInfo << "Planar Slab Parameters" << std::endl;
			paraInfo << "---------------------------" << std::endl;
			#if !(WALL_ON)
			paraInfo << "Width of planar slab (slabWidth)           :           " << slabWidth << std::endl;
			#endif
			#endif
			
			#if RANDOM_DISSIPATIVE
			paraInfo << "---------------------------" << std::endl;
			paraInfo << "Random & Dissipative Force " << std::endl;
			paraInfo << "---------------------------" << std::endl;
			paraInfo << "Noise level (sigma)                        :           " << sigma << std::endl;
			paraInfo << "Friction parameter (gamma)                 :           " << gamma << std::endl;
			#endif 
			
			#if WALL_ON
			paraInfo << "---------------------------" << std::endl;
			paraInfo << "Wall parameters      " << std::endl;
			paraInfo << "---------------------------" << std::endl;
			paraInfo << "Number of solid particles                  :           " << npart - fluidCount << std::endl;
			paraInfo << "Wall density (initWallRho)                 :           " << initWallRho << std::endl;
			paraInfo << "Solid-Liquid Attraction Strength   (Asl)   :           " << Asl << std::endl;
			paraInfo << "Solid-Liquid Repulsion  Strength   (Bsl)   :           " << Bsl << std::endl;
			paraInfo << "Wall Attraction cutoff (rcWallcutoff )     :           " << rcWcutoff << std::endl;
			paraInfo << "Wall Repulsion  cutoff (rdWall_cutoff )    :           " << rdWcutoff << std::endl;
			#endif 
			
			paraInfo << "---------------------------" << std::endl;
			paraInfo << "Conservative Force         " << std::endl;
			paraInfo << "---------------------------" << std::endl;
			paraInfo << "Liquid-Liquid Attraction Strength (All)    :           " << All << std::endl;
			paraInfo << "Liquid-Liquid Repulsion Strength (Bll)     :           " << Bll << std::endl;
			paraInfo << "---------------------------" << std::endl;
			paraInfo << "---------------------------" << std::endl;
			paraInfo << "---------------------------" << std::endl;

		}

		//--------------------------------------- Velocity, Momentum and Pressure file writing--------------------------------------//
		void fileWrite( std::ofstream& enStats, std::ofstream& eosStats, std::ofstream& momStats, std::ofstream& pTensStats ){

			if ( step % 10000 == 0){
				std::cout<< step << " steps out of " << stepMax << " completed " << std::endl;
			}

			//write output file in the .data format
			if (++counter>=saveCount) {				

				// particle positions in vtk file format
				vtkFileWritePosVel();

				// Energy
				enStats 	<< std::setw(20) << std::setprecision(15) << pot_en << "\t" 
					<< std::setw(20) << std::setprecision(15) << kin_en << "\t" 
					<< std::setw(20) << std::setprecision(15) << tot_en << std::endl;

				// Density, Temperature, Average Pressure
				eosStats 	<< std::setw(20) << std::setprecision(15) << rho 	<< "\t" 
					<< std::setw(20) << std::setprecision(15) << temp	<< "\t" 
					<< std::setw(20) << std::setprecision(15) << pressure 	<< std::endl;

				// Momentum
				momStats 	<< std::setw(20) << std::setprecision(15) << momX << "\t" 
					<< std::setw(20) << std::setprecision(15) << momY << "\t" 
					<< std::setw(20) << std::setprecision(15) << momZ << std::endl;

				// average and reset the pTensor
				#include "pTensAverage.h"

				/*	
				// Pressure tensor	
				pTensStats 	<< std::setw(20) << std::setprecision(15) << pTensor[0][0] << "\t" 
				<< std::setw(20) << std::setprecision(15) << pTensor[0][1] << "\t" 
				<< std::setw(20) << std::setprecision(15) << pTensor[0][2] << "\t"
				<< std::setw(20) << std::setprecision(15) << pTensor[1][0] << "\t"
				<< std::setw(20) << std::setprecision(15) << pTensor[1][1] << "\t"
				<< std::setw(20) << std::setprecision(15) << pTensor[1][2] << "\t"
				<< std::setw(20) << std::setprecision(15) << pTensor[2][0] << "\t"
				<< std::setw(20) << std::setprecision(15) << pTensor[2][1] << "\t"
				<< std::setw(20) << std::setprecision(15) << pTensor[2][2] << std::endl;
				 */

				// Pressure tensor	
				pTensStats 	<<  pIdeal[0][0] << ", " 
					<<  pIdeal[0][1] << ", " 
					<<  pIdeal[0][2] << ", "
					<<  pIdeal[1][0] << ", "
					<<  pIdeal[1][1] << ", "
					<<  pIdeal[1][2] << ", "
					<<  pIdeal[2][0] << ", "
					<<  pIdeal[2][1] << ", "
					<<  pIdeal[2][2] << ", "
					<<  pNonIdeal[0][0] << ", " 
					<<  pNonIdeal[0][1] << ", " 
					<<  pNonIdeal[0][2] << ", "
					<<  pNonIdeal[1][0] << ", "
					<<  pNonIdeal[1][1] << ", "
					<<  pNonIdeal[1][2] << ", "
					<<  pNonIdeal[2][0] << ", "
					<<  pNonIdeal[2][1] << ", "
					<<  pNonIdeal[2][2] << std::endl;

				#include "pTensReset.h"

				// density profile rhoZ
				#if PLANAR_SLAB 

				sprintf( filename, "./data/rhoZ_%d.dat", step );  
				std::ofstream rhoZStats( filename );

				for ( iRhoZ = 0; iRhoZ < rhoZ_bins ; ++iRhoZ )  {

					bin_lower = rhoZ_Zmin + ( iRhoZ ) * rhoZ_Zdelta;
					bin_upper = rhoZ_Zmin + ( iRhoZ + 1 ) * rhoZ_Zdelta;
					Zpos = ( bin_lower + bin_upper )*0.5;
					vol = rhoZ_Zdelta * boxEdge[x] * boxEdge[y];

					rhoZStats << Zpos << "\t" << vol << "\t" << rhoZ[iRhoZ] << "\t" << counter << std::endl;

					// reset value of rhoZ vector
					rhoZ[iRhoZ] = 0.0;
				}

				#elif CYLINDER_DROPLET

				sprintf( filename, "./data/rhor_%d.dat", step );  
				std::ofstream rhorStats( filename );

				for ( iRhor = 0; iRhor < rhor_bins ; ++iRhor )  {

					bin_lower = rhor_rmin + ( iRhor ) * rhor_rdelta;
					bin_upper = rhor_rmin + ( iRhor + 1 ) * rhor_rdelta;
					radPos = ( bin_lower + bin_upper )*0.5;
					vol = M_PI * ( pow( bin_upper, 2.0 ) - pow( bin_lower, 2.0 ) ) * cylHeight;

					rhorStats << radPos << "\t" << vol << "\t" << rhor[iRhor] << "\t" << counter << std::endl;

					// reset value of rhor vector
					rhor[iRhor] = 0.0;
				}
				#endif	


				//reset the counter, write time to terminal
				counter = 0;
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
			// position data -- particles 
			file << "<DataArray type=\"Float32\" Name=\"Position\" NumberOfComponents=\"3\" format=\"ascii\">"<< std::endl;
			for (Particle& p: particles)
			{
				file << p.r << std::endl;

			}
			file << "</DataArray>"<<std::endl;
			file << "</Points>" << std::endl;	

			// particle species
			/*
			   file << "<DataArray type=\"UInt8\" Name=\"Colors\" NumberOfComponents=\"3\" format=\"ascii\">"<< std::endl;
			   for (Particle& p: particles)
			   {
			   file << "255" << ", 0" << ", 0" << std::endl;

			   }
			   file << "</DataArray>"<<std::endl;
			 */

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
