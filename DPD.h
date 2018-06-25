#ifndef _DPD_CG
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
#define RANDOM_DISSIPATIVE               1
#define SPHERICAL_DROPLET                0
#define SPHERICAL_CAP                    0
#define CYLINDER_DROPLET                 0
#define PLANAR_SLAB                      0
#define CRYSTAL	                         0
#define RESTART	                         0

// WALL flags
#define WALL_ON                          1
#define LOWER_WALL_ON                    0
#define UPPER_WALL_ON                    0
#define FCC_WALL                         0
#define ROUGH_WALL                       0

#define CAPILLARY_TUBE			 1

// POISEUILLE flow
#define BODY_FORCE                       0

// FILE_WRITE
#define STYLE_VMD                        1
#define STYLE_MERCURY_DPM                0

// CORRELATION FUNCTIONS
#define SACF                             0
#define SACF_TEST                        0

// LEES-EDWARDS BOUNDARY CONDITION
#define LEES_EDWARDS_BC                  0

// DENSITY CALCULATION
#define DENS_EXACT			 0

//declare DPD solver
class DPD {
	public:
		#include "vars_list.h"

		/******************************************************************************************************/
		/**************************************** SOLVE ROUTINE ***********************************************/
		/******************************************************************************************************/
		//solve contains the time stepping algorithm and the output routines 
		void solve () {
	
			simProg.open ( "./simProg.txt", std::ios::out );
			simProg << " Proccess ID : " << getpid() << std::endl;
			// initialize position, velocity, hoc, ll

			// parameters declaration
			pTensCounter            = 0;
			#if SACF
				normalizeCorr_count     = 0;
			#endif
			step 			= 1;
			init();
			temp 			= 0.0;
			tempSum 		= 0.0;
		    	#if WALL_ON
				wallTemp 		= 0.;
			#endif
			tempAv 			= 0.0;
			tempCount 		= 0;
			volume 			= boxEdge[x] * boxEdge[y] * boxEdge[z];		// system volume
			npart 			= particles.size();				// number of particles
			rho 			= npart/volume;					// density of system 
			dof 			= dim*(npart - 1);				// total degrees of freedom (momentum only, no energy conservation)
			inv_sqrt_dt 		= 1.0/std::sqrt(dt);				// inverse of square root of the time step
			half_dt 		= 0.5*dt;					// 0.5*dt to be used in the integrateEOM()
			half_dt_sqr 		= half_dt*dt;					// 0.5*dt*dt to be used in the integrateEOM()

			rd4 			= pow(rd_cutoff, 4.0);				// fourth power of rd_cutoff
			rc4 			= pow(rcutoff, 4.0);				// fourth power of rc_cutoff
			K1 			= piThirty * rc4 * All;				// constant k1
			K2 			= piThirty * rd4 * Bll;				// constant k2

			counter 		= 0;						// initialize time, counter for file writing
			pcounter 		= 0;						// initialize time, counter for file writing

			#if !(RESTART)
				createGridList();
				dens_calculation();
				forceCalc();
				
				resetVar();
			#endif

			#if RESTART	
				std::ofstream enStats 		( "./data/en_data.dat"	, std::ios_base::app);	// Kinetic, Potential and total energy
				std::ofstream eosStats		( "./data/eos_data.dat"	, std::ios_base::app);	// Mean Pressure and temperature data
				std::ofstream pTensStats	( "./data/pTens.dat"	, std::ios_base::app);	// Pressure tensor data
				std::ofstream momStats		( "./data/mom_data.dat"	, std::ios_base::app);	// pressure and temperature data
			#else
				std::ofstream enStats 		( "./data/en_data.dat"	);	// Kinetic, Potential and total energy
				std::ofstream eosStats		( "./data/eos_data.dat"	);	// Mean Pressure and temperature data
				std::ofstream pTensStats	( "./data/pTens.dat"	);	// Pressure tensor data
				std::ofstream momStats		( "./data/mom_data.dat"	);	// pressure and temperature data
			#endif // RESTART

			// write parameters and initial configuration
			vtkFileWritePosVel();
			std::ofstream paraInfo( "param.out" );
			paraWrite(paraInfo);
			paraInfo.close();

			#if WALL_ON
				if ( rd_cutoff > rcWcutoff ){
					simProg<< " The cutoff for density calculation of fluid near wall needs to be checked " << std::endl; 
					abort();
				}
			#endif

			simProg << " ********************* STARTING SIMULATION ************************* " << std::endl;	
			while (step<= stepMax) {

				createGridList();
				#if DENS_EXACT
					dens_calculation();
				#endif
				forceCalc();
	
				integrateEOM();

				for ( i=0 ; i <npart ; ++i ) {
					pot_en += K1 * particles[i].rhoBar + K2 * pow( particles[i].dens, 2.0 ); 	// sum of self-energies of all particles
				}
				tot_en = kin_en + pot_en;

				if ( (step > gR_tStart) && (step % gR_tDelta == 0) ) grSample();

				counter += 1;						// filewriting
				pcounter += 1;
				fileWrite(enStats, eosStats, momStats, pTensStats);

				resetVar();						// reset variables to zero
				
				// for (Particle& p : particles)
				//	simProg << "Position: " << p.r << ", Density: " << p.dens <<", conservative force: " << p.fC << std::endl;

				if ( step % rstrtFwrtFreq == 0 ){
					// write position velocity stats 
					std::ofstream writeConfig;
					writeConfig.open ( "./restart/posvelrestartfile.dat", std::ios::binary | std::ios::out );	// example binary file
					finalposvelWrite( writeConfig );	
					writeConfig.close();
				}
				
				#if SACF 
					if ( step % 100000 == 0 ) { writeCorr(); }
				#endif

				step += 1;						// increment time step

				// if ( step % 1 == 0 )
				//	simProg << " step= " << step << std::endl;

			}//end time loop

			// post-processing
			// grCalc();	 
			// velHistCalc();


			// enStats.close();	// close file-streams
			// eosStats.close();
			// momStats.close();
			
			simProg << " ********************* ENDING SIMULATION ************************* " << std::endl;	
			simProg.close();

		} //void solve()

		//--------------------------------------- INITIALIZATION ROUTINE --------------------------------------//
		void init(){

			#include "paramIn.h"

			#if WALL_ON
				#include "defineWall.h"

				#if SPHERICAL_CAP
					#include "sphericalCap.h"			
				#elif CYLINDER_DROPLET
					#include "cylDropInit.h"
				#elif PLANAR_SLAB
					#include "planarSlabInit.h" 
				#elif CAPILLARY_TUBE
					#include "capillaryTube.h"
					#include "reservoir.h"
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

			// initializing NrCells[3], scale	
			for ( i = 0 ; i < 3 ; i++ )
			{
				NrCells[i] = int( boxEdge[i] / rcutoff ); // cellnr runs from 0 to NrCells-1
				scale[i] = NrCells[i] * boxRecip[i] ;
				if ( NrCells[i] < 3 ) { simProg << "*** NrCells[" << i << "] = " << NrCells[i] << std::endl ; abort(); }

				// simProg << "NrCells[" << i << "] = " << NrCells[i] << std::endl;
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
				periodN[0][j] = NrCells[j] - 1;    // left neighbour of leftmost cell
				periodR[0][j] = -boxEdge[j];       // correction to add to particle j in rij = ri - rj
				for ( i = 1 ;  i < NrCells[j] + 1 ; i++ )
				{ 
					periodN[i][j] = i - 1; 	   // same cell
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

			pNonIdealKin[0][0] = 0.0;
			pNonIdealKin[0][1] = 0.0;
			pNonIdealKin[0][2] = 0.0;
			pNonIdealKin[1][0] = 0.0;
			pNonIdealKin[1][1] = 0.0;
			pNonIdealKin[1][2] = 0.0;
			pNonIdealKin[2][0] = 0.0;
			pNonIdealKin[2][1] = 0.0;
			pNonIdealKin[2][2] = 0.0;

			pDissipative[0][0] = 0.0;
			pDissipative[0][1] = 0.0;
			pDissipative[0][2] = 0.0;
			pDissipative[1][0] = 0.0;
			pDissipative[1][1] = 0.0;
			pDissipative[1][2] = 0.0;
			pDissipative[2][0] = 0.0;
			pDissipative[2][1] = 0.0;
			pDissipative[2][2] = 0.0;

			pRandom[0][0] = 0.0;
			pRandom[0][1] = 0.0;
			pRandom[0][2] = 0.0;
			pRandom[1][0] = 0.0;
			pRandom[1][1] = 0.0;
			pRandom[1][2] = 0.0;
			pRandom[2][0] = 0.0;
			pRandom[2][1] = 0.0;
			pRandom[2][2] = 0.0;

			// find indices of all liquid particles
			fluidCount = 0;
			#if WALL_ON
				solidCount = 0;
			#endif
			for ( i = 0; i < particles.size(); ++i ){
				if ( particles[i].type == 1 ){
					fluid_index.push_back(i);
					fluidCount++;
				}
				#if WALL_ON
				else{
					solid_index.push_back(i);
					solidCount++;
				}
				#endif
			}

			simProg << fluidCount << " fluid particles indexed" << std::endl;
			simProg << solidCount << " solid particles indexed" << std::endl;

			#if WALL_ON
				for ( i = solid_index[0] ; i <= solid_index[solidCount-1] ; ++i )
					particles[i].r0 = particles[i].r;

				#if BODY_FORCE
					for ( i = fluid_index[0] ; i <= fluid_index[fluidCount - 1] ; ++i ){
						particles[i].fBody.X = fBodyX;
						particles[i].fBody.Y = 0.;
						particles[i].fBody.Z = 0.;
					}
				#endif	// BODY_FORCE
			#endif // WALL_ON

			simProg << solidCount << " solid particles attached to initial positions by a spring" << std::endl;
			
			// Remove excess velocity and set particle density to 0
			Vec3D velAvg={0.0, 0.0, 0.0};
			if ( fluidCount != 0 ){
				for ( i=fluid_index[0] ; i <= fluid_index[fluidCount - 1] ; ++i ){
					velAvg 			+= particles[i].w/fluidCount;
					particles[i].dens 	 = 0.0;
					particles[i].dens_new	 = 0.0;
					particles[i].fC.setZero();
				}
	
				for ( i=fluid_index[0] ; i <= fluid_index[fluidCount - 1] ; ++i )
					particles[i].w -= velAvg;
	
				simProg << "Excess average velocity from fluid particles removed" << std::endl;
				simProg << "Densities of the fluid particles set to 0" << std::endl;
			}
			else{

				simProg << "No fluid particles intialized" << std::endl;
			}

			#if WALL_ON
				velAvg={0.0, 0.0, 0.0};
				// Remove excess velocity and set particle density to 0
				for ( i = solid_index[0] ; i <= solid_index[solidCount - 1] ; ++i ){
					velAvg 			+= particles[i].w/solidCount;
					particles[i].dens 	 = 0.0;
					particles[i].dens_new	 = 0.0;
				}

				for ( i = solid_index[0] ; i <= solid_index[solidCount - 1] ; ++i )
					particles[i].w -= velAvg;
				
				simProg << "Excess average velocity from solid particles removed" << std::endl;
				simProg << "Densities of the solid particles set to 0" << std::endl;
			#endif

			#if LEES_EDWARDS_BC
				dissipativeWork = 0.0;
				randomWork      = 0.0;
			#endif

			#if SACF
			// multi-tau correlation matrix definition
			fCorr.resize(n_vars * n_vars);		// fcor stores the calculated correlation at a given level
			nCorr.resize(n_vars * n_vars);		// ncor stores the number of samples used for averaging the correlation
			
			fCorrAv.resize(n_vars * n_vars);	// used for file-writing purpose at intermediate times

			normalizeCorr.resize(n_vars * n_vars);		// normalized values for every stress vector element
			normalizeCorrAv.resize(n_vars * n_vars);	// used for file-writing purpose at intermediate times

			aCorr.resize(n_vars);		// acor stores the data used to calculate correlation
			pointCorr.resize(n_vars);	// pointcor stores the array-indes at which the last data was stored

			// fCorr matrix initialize
			for ( i=0 ; i<n_vars*n_vars ; ++i ) {

				fCorr[i].resize( corLevels );
				nCorr[i].resize( corLevels );
				fCorrAv[i].resize( corLevels );

				for (  j=0 ; j<corLevels ; ++j ){
					fCorr[i][j].resize( pCorr2 );
					nCorr[i][j].resize( pCorr2 );
					fCorrAv[i][j].resize( pCorr2 );
				}// j
			}// i

			// aCorr, pointCorr, nCorr matrix initialize
			for ( i=0 ; i<n_vars ; ++i ) {

				aCorr[i].resize( corLevels );
				pointCorr[i].resize( corLevels );

				for (  j=0 ; j<corLevels ; ++j ){
					aCorr[i][j].resize( pCorr );
				}// j
			}// i

			// initialize aCorr -- WARNING : assign large initial negative 
			// (or positive) values to ensure that the data read is larger(smaller)
			// than this initial value 
			for ( i=0 ; i<n_vars ; ++i )
				for ( j=0 ; j<corLevels ; ++j )
					for( k=0 ; k<pCorr ; ++k )
						aCorr[i][j][k] = -10e20;

			// initialize fCorr
			for ( i=0 ; i<n_vars*n_vars ; ++i )
				for ( j=0 ; j<corLevels ; ++j )
					for( k=0 ; k<pCorr2 ; ++k )
						fCorr[i][j][k] = 0.;

			// initialize norr
			for ( i=0 ; i<n_vars*n_vars ; ++i )
				for ( j=0 ; j<corLevels ; ++j )
					for( k=0 ; k<pCorr2 ; ++k )
						nCorr[i][j][k] = 0;

			// initialize pointCorr
			for ( i=0 ; i<n_vars ; ++i )
				for ( j=0 ; j<corLevels ; ++j )
					pointCorr[i][j] = -1;

			// initialize normalizeCorr
			for ( i=0 ; i<n_vars*n_vars ; ++i ){
					normalizeCorr[i] = 0.;
					normalizeCorrAv[i] = 0.;}
					

			// initialize sacpunt
			sacpunt = 0;

			// finding size of arrays
			// simProg << "aCorr [" << aCorr.size() << "][" << aCorr[0].size() << "][" << aCorr[0][0].size() << "]" << std::endl;
			// simProg << "fCorr [" << fCorr.size() << "][" << fCorr[0].size() << "][" << fCorr[0][0].size() << "]" << std::endl;
			// simProg << "nCorr [" << nCorr.size() << "][" << nCorr[0].size() << "][" << nCorr[0][0].size() << "]" << std::endl;
			// simProg << "pointCorr [" << pointCorr.size() << "][" << pointCorr[0].size() << "]" << std::endl;
			#endif

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
								// simProg << "j1 "<<  mi[x] << " " << mi[y] << " " << mi[z] << " " << jj << " " << j << std::endl;

								#include "dens_calculate.h"

							} // jj

							#if LEES_EDWARDS_BC 
							strain = std::fmod( strainRate * dt * ( step - 1 ) , boxEdge[x] );	// strain in units of boxEdge[x]
							//strain = std::fmod( gammaDot , boxEdge[x] );	// strain in units of boxEdge[x]

							if ( mi[y] == NrCells[y] - 1 ){	// calculating neighbor-cells for cells with mi[y] = NrCells[y] - 1 ( top-layer cells )
								
								// std::cout << "dmCell1, dmCell2, dmCell3, dmCell4 = " << dmCell1 << ", " << dmCell2 << ", " << dmCell3 << "," << dmCell4 << std::endl;
								#include "dm_LEbc_init.h"

								// std::cout << "dm_LEbc initialized for cell mi[x], mi[y], mi[z]" << mi[x] << ", "<< mi[y] << ", " << mi[z] << std::endl;
								// std::cout << "Cell mi[x], mi[y], mi[z]" << mi[x] << ", "<< mi[y] << ", " << mi[z] << std::endl;

								for ( m = 0 ; m < 16 ; m++ )
								{
									i1 = moduloAB( mi[x] - dm_LEbc[m][x] + 1, NrCells[x] );	// correction for cell in direction of shear ( x )
									i2 = mi[y] + dm_LEbc[m][y] + 1;
									i3 = mi[z] + dm_LEbc[m][z] + 1;
									// std::cout << i1  <<", " << i2 <<", " << i3 << std::endl;

									mj[x]        = periodN[ i1 ][ x ];
									mj[y]	     = periodN[ i2 ][ y ];
									mj[z]	     = periodN[ i3 ][ z ];

									dR.X	     = strain * ( dm_LEbc[m][y] == 1 );			// correction for particle-position: should correct only for +1 change in y 
									dR.Y	     = 0.; 
									dR.Z	     = 0.;

									// std::cout <<  m << " " << mj[x] << " " << mj[y] << " " << mj[z] << " " << dR.X << " " << dR.Y << " " << dR.Z << std::endl;
									for ( jj = 1 ; jj <= grid[mj[x]][mj[y]][mj[z]][0] ; ++jj )
									{
										j = grid[mj[x]][mj[y]][mj[z]][jj];
										// simProg << "j2 " << m << " " << mj[x] << " " << mj[y] << " " << mj[z] << " " << jj << " " << j << std::endl;

										#include "dens_calculate.h"

									} // jj

								} // m
							}
							else{
								for ( m = 0 ; m < 13 ; m++ )
								{
									mj[x]        = periodN[ mi[x] + dm[m][x] + 1 ][x];
									mj[y]	     = periodN[ mi[y] + dm[m][y] + 1 ][y];
									mj[z]	     = periodN[ mi[z] + dm[m][z] + 1 ][z];

									/*
									dR.X         = periodR[ mi[x] + dm[m][x] + 1 ][x]; // correction for position
									dR.Y	     = periodR[ mi[y] + dm[m][y] + 1 ][y];
									dR.Z	     = periodR[ mi[z] + dm[m][z] + 1 ][z];*/

									dR.setZero();

									for ( jj = 1 ; jj <= grid[mj[x]][mj[y]][mj[z]][0] ; ++jj )
									{
										j = grid[mj[x]][mj[y]][mj[z]][jj];
										// simProg << "j2 " << m << " " << mj[x] << " " << mj[y] << " " << mj[z] << " " << jj << " " << j << std::endl;
										#include "dens_calculate.h"

									} // jj
								} // m

							}
							#else
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
									// simProg << "j2 " << m << " " << mj[x] << " " << mj[y] << " " << mj[z] << " " << jj << " " << j << std::endl;

									#include "dens_calculate.h"

								} // jj
							} // m

							#endif // LEES_EDWARDS_BC

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

				// simProg << "position of particle " << i << " =" << particles[i].r;
				// simProg << ", mi[x], mi[y], mi[z] =  " << mi[x] << ", " << mi[y] << ", " << mi[z] << std::endl;
				if ( mi[x] < mini[x] || mi[x] > maxi[x] ||  // debug
			             mi[y] < mini[y] || mi[y] > maxi[y] ||
				     mi[z] < mini[z] || mi[z] > maxi[z] )
				{ 
					simProg << "*** particle " << i << " of type " << particles[i].type  << " is outside box in step: " << step << std::endl;
					simProg << "*** position: " << particles[i].r << std::endl;
					simProg << mini[x] << " < " << mi[x] << " < " << maxi[x] << std::endl;
					simProg << mini[y] << " < " << mi[y] << " < " << maxi[y] << std::endl;
					simProg << mini[z] << " < " << mi[z] << " < " << maxi[z] << std::endl;
					abort();
				}
				if ( grid[mi[x]][mi[y]][mi[z]][0] == MaxPerCell )
				{ 
					simProg << "*** cell overfull" << std::endl;
					simProg << mi[x] << "  " << mi[y] << "  " << mi[z] << std::endl;
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
							velCorr.setZero();

							for ( jj = ii + 1 ; jj <= grid[mi[x]][mi[y]][mi[z]][0] ; ++jj )
							{
								j = grid[mi[x]][mi[y]][mi[z]][jj];
								// simProg << "j1 "<<  mi[x] << " " << mi[y] << " " << mi[z] << " " << jj << " " << j << std::endl;

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

								// simProg << "i, mi[x], mi[y], mi[z] = " <<  i << " " << mi[x] << " " << mi[y] << " " <<  mi[z] ;
								// simProg << ", j, mj[x], mj[y], mj[z] = " << j << " " << mj[x] << " " << mj[y] << " " <<  mj[z] ;
								// simProg << "i, pos= " <<  i << " " << particles[i].r;
								// simProg << ",j, pos= " << j << " " << particles[j].r;
								// simProg << ", fCij = " << fCij  << ", fC = " << particles[i].fC << std::endl; 

							} // jj

							// particle j in neighbour cell to i
							#if LEES_EDWARDS_BC
							strain = std::fmod( strainRate * dt * ( step - 1 ) , boxEdge[x] );	// strain in units of boxEdge[x]
							// strain = std::fmod( gammaDot , boxEdge[x] );	// strain in units of boxEdge[x]

							if (  mi[y] == NrCells[y] - 1 ){	// calculating neighbor-cells for cells with mi[y] = NrCells[y] - 1 ( top-layer cells )
								
								// std::cout << "dmCell1, dmCell2, dmCell3, dmCell4 = " << dmCell1 << ", " << dmCell2 << ", " << dmCell3 << "," << dmCell4 << std::endl;
								#include "dm_LEbc_init.h"

								// std::cout << "dm_LEbc initialized for cell mi[x], mi[y], mi[z]" << mi[x] << ", "<< mi[y] << ", " << mi[z] << std::endl;
								// std::cout << "Cell mi[x], mi[y], mi[z]" << mi[x] << ", "<< mi[y] << ", " << mi[z] << std::endl;

								for ( m = 0 ; m < 16 ; m++ )
								{
									i1 = moduloAB( mi[x] - dm_LEbc[m][x] + 1, NrCells[x] );	// correction for cell in direction of shear ( x )
									i2 = mi[y] + dm_LEbc[m][y] + 1;
									i3 = mi[z] + dm_LEbc[m][z] + 1;
									// std::cout << i1  <<", " << i2 <<", " << i3 << std::endl;

									mj[x]        = periodN[ i1 ][ x ];
									mj[y]	     = periodN[ i2 ][ y ];
									mj[z]	     = periodN[ i3 ][ z ];

									dR.X	     = strain * ( dm_LEbc[m][y] == 1 );			// correction for particle-position: should correct only for +1 change in y 
									dR.Y	     = 0.; 
									dR.Z	     = 0.;

									/* BUG: velCorr was a bug. Now corrected */
									velCorr.X    = strainRate * ( dm_LEbc[m][y] == 1 );             // correction for particle-velocity: should correct only for +1 change in y
									velCorr.Y    = 0.;
									velCorr.Z    = 0.;

									// std::cout <<  m << " " << mj[x] << " " << mj[y] << " " << mj[z] << " " << dR.X << " " << dR.Y << " " << dR.Z << std::endl;
									for ( jj = 1 ; jj <= grid[mj[x]][mj[y]][mj[z]][0] ; ++jj )
									{
										j = grid[mj[x]][mj[y]][mj[z]][jj];
										// simProg << "j2 " << m << " " << mj[x] << " " << mj[y] << " " << mj[z] << " " << jj << " " << j << std::endl;

										#include "pairforceLL.h"

									} // jj

								} // m

							} // ( mi[y] == NrCells[y] - 1 )
							else {
								dR.setZero();
								velCorr.setZero();

								for ( m = 0 ; m < 13 ; m++ )
								{
									mj[x]        = periodN[ mi[x] + dm[m][x] + 1 ][x];
									mj[y]	     = periodN[ mi[y] + dm[m][y] + 1 ][y];
									mj[z]	     = periodN[ mi[z] + dm[m][z] + 1 ][z];

									/*
									dR.X         = periodR[ mi[x] + dm[m][x] + 1 ][x]; // correction for position
									dR.Y	     = periodR[ mi[y] + dm[m][y] + 1 ][y];
									dR.Z	     = periodR[ mi[z] + dm[m][z] + 1 ][z];*/


									for ( jj = 1 ; jj <= grid[mj[x]][mj[y]][mj[z]][0] ; ++jj )
									{
										j = grid[mj[x]][mj[y]][mj[z]][jj];
										// simProg << "j2 " << m << " " << mj[x] << " " << mj[y] << " " << mj[z] << " " << jj << " " << j << std::endl;

											#include "pairforceLL.h"
									} // jj
								} // m
							
							}	

							#else
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
									// simProg << "j2 " << m << " " << mj[x] << " " << mj[y] << " " << mj[z] << " " << jj << " " << j << std::endl;

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
							#endif

						} // ii
		}

		//--------------------------------------- Integrate Equations of motion --------------------------------------//
		void integrateEOM(){

			#include "fluidParticles.h"

			#include "solidParticles.h"

			// calculation of  pressure tensor 
			#include "pTensCalc.h"

			// calculate auto-correlation
			#if SACF
			if ( step > 5e4 ){
				#include "ACF.h"
			}
			#endif

		} // run over all fluid particles
		//--------------------------------------- Resetting variables--------------------------------------//
		void resetVar(){
			// energy reset to zero
			pot_en		= 0.0;
			kin_en		= 0.0;
			tot_en		= 0.0;
			vx2		= 0.0;
			vy2		= 0.0;
			vz2		= 0.0;
			v2		= 0.0;
			vxvy		= 0.0;
			pressure	= 0.0;
			momX		= 0.0;
			momY		= 0.0;
			momZ		= 0.0;
			#if LEES_EDWARDS_BC
			dissipativeWork = 0.0;
			randomWork	= 0.0;
			#endif
	
			// simProg << " fi[0], fi[fluidCount - 1], fluidCount " << fluid_index[0] << " " << fluid_index[1] << " " << fluidCount << std::endl;	
			for ( i = 0; i < npart ; ++i )
			{

			
				#if DENS_EXACT	
					particles[i].dens = 0.0;
				#else
					particles[i].dens = particles[i].dens_new;
					particles[i].dens_new = 0.;
				#endif
				particles[i].rhoBar = 0.0;
				particles[i].fC.setZero();
		
				#if RANDOM_DISSIPATIVE 
					particles[i].fR.setZero();
					particles[i].fD.setZero();
				#endif

				#if WALL_ON
					particles[i].fext.setZero();
					particles[i].fCW.setZero();
					particles[i].fHarmonic.setZero();
				#endif 

			} // set density equal to zero for solid type particles

			// reset temporary ideal and non-ideal component of pressure tensor
			// to 0 at every time step. 
			pNonIdeal_temp[0][0] = 0.;
			pNonIdeal_temp[0][1] = 0.;
			pNonIdeal_temp[0][2] = 0.;
			pNonIdeal_temp[1][0] = 0.;
			pNonIdeal_temp[1][1] = 0.;
			pNonIdeal_temp[1][2] = 0.;
			pNonIdeal_temp[2][0] = 0.;
			pNonIdeal_temp[2][1] = 0.;
			pNonIdeal_temp[2][2] = 0.;
	
			pNonIdealKin_temp[0][0] = 0.;
			pNonIdealKin_temp[0][1] = 0.;
			pNonIdealKin_temp[0][2] = 0.;
			pNonIdealKin_temp[1][0] = 0.;
			pNonIdealKin_temp[1][1] = 0.;
			pNonIdealKin_temp[1][2] = 0.;
			pNonIdealKin_temp[2][0] = 0.;
			pNonIdealKin_temp[2][1] = 0.;
			pNonIdealKin_temp[2][2] = 0.;

			#if RANDOM_DISSIPATIVE
			// Dissipative
			pDissipative_temp[0][0] = 0.;
			pDissipative_temp[0][1] = 0.;
			pDissipative_temp[0][2] = 0.;
			pDissipative_temp[1][0] = 0.;
			pDissipative_temp[1][1] = 0.;
			pDissipative_temp[1][2] = 0.;
			pDissipative_temp[2][0] = 0.;
			pDissipative_temp[2][1] = 0.;
			pDissipative_temp[2][2] = 0.;

			// Random
			pRandom_temp[0][0] = 0.;
			pRandom_temp[0][1] = 0.;
			pRandom_temp[0][2] = 0.;
			pRandom_temp[1][0] = 0.;
			pRandom_temp[1][1] = 0.;
			pRandom_temp[1][2] = 0.;
			pRandom_temp[2][0] = 0.;
			pRandom_temp[2][1] = 0.;
			pRandom_temp[2][2] = 0.;
			#endif
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

						if ( (ig < 0) || (ig >= gR_nElem)) {simProg << " out of bounds " << std::endl;} 

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

			simProg << " the total number of samples is: " << gR_tSamples	<< std::endl;
			simProg << " the homogeneous density is: " << rho << std::endl;
			simProg << " the number of particles is: " << npart << std::endl;

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

		//---------------------------------------- Auto-correlation function calculation------------------------------//
		#if SACF
		void recursive_addCorr(double f, unsigned int nf, unsigned int k){

			// shift-pointer and put in f
			point               = ( pointCorr[nf][k] + 1 ) % pCorr;
			pointCorr[nf][k]    = point;
			aCorr[nf][k][point] = f;

			//std::cout << "point = " << point  << "\t f = " << f << "\t nf= " << nf << "\t k = " << k << std::endl;

			// correlate data
			// WARNING: ensure that none of the data is smaller than -9e9
			// If this is the case, the aCorr[nf][k][j] is discounted, leading to 
			// wrong correlations. For example: if " > -1" leads to erroneous results
			for ( i=0 ; i<pCorr2 ; ++i ){

				j = ( point + pCorr - i - pCorr2 ) % pCorr;			// see report for explanation

				n_base = n_vars * nf;
				if ( aCorr[nf][k][j] > -9e20 ){					// nf+0 -- SxyC(t) * SxyC(0) / SxyR(t) * SxyR(0) / SxyD(t) * SxyD(0)
					fCorr[n_base][k][i] += f*aCorr[nf][k][j];
					nCorr[n_base][k][i] += 1; 
				}
			
				nf1 = (nf+1) % n_vars;	
				if ( aCorr[nf1][k][j] > -9e20 ){				// nf+1 -- SxyC(t) * SxyR(0) / SxyR(t) * SxyD(0) / SxyD(t) * SxyC(0)
					fCorr[n_base + 1][k][i] += f*aCorr[nf1][k][j];
					nCorr[n_base + 1][k][i] += 1; 
				}
								
				nf2 = (nf+2) % n_vars;	
				if ( aCorr[nf2][k][j] > -9e20 ){				// nf+2 -- SxyC(t) * SxyD(0) / SxyR(t) * SxyC(0) / SxyD(t) * SxyR(0)
					fCorr[n_base + 2][k][i] += f*aCorr[nf2][k][j];
					nCorr[n_base + 2][k][i] += 1; 
				}

				nf3 = (nf+3) % n_vars;	
				if ( aCorr[nf3][k][j] > -9e20 ){				// nf+2 -- SxyC(t) * SxyD(0) / SxyR(t) * SxyC(0) / SxyD(t) * SxyR(0)
					fCorr[n_base + 3][k][i] += f*aCorr[nf3][k][j];
					nCorr[n_base + 3][k][i] += 1; 
				}

			} // do the correlation only when the array is filled up
			
			// putting in 0 missing values -- from 1 to pCorr2-1
			if ( k == 1 ){
				for ( i=0 ; i<pCorr2-1 ; ++i ){
		
					j = ( point + pCorr - i - 1 ) % pCorr;
				
					n_base = n_vars * nf;
					if ( aCorr[nf][1][j] > -9e20 ){
						fCorr[n_base][0][i] += aCorr[nf][1][point] * aCorr[nf][1][j];
						nCorr[n_base][0][i] += 1;
					}

					nf1 = (nf+1) % n_vars;	
					if ( aCorr[nf1][1][j] > -9e20 ){			
						fCorr[n_base + 1][0][i] += aCorr[nf][1][point] * aCorr[nf1][k][j];
						nCorr[n_base + 1][0][i] += 1; 
					}
									
					nf2 = (nf+2) % n_vars;	
					if ( aCorr[nf2][1][j] > -9e20 ){				
						fCorr[n_base + 2][0][i] += aCorr[nf][1][point] * aCorr[nf2][k][j];
						nCorr[n_base + 2][0][i] += 1; 
					}
					
					nf3 = (nf+3) % n_vars;	
					if ( aCorr[nf3][1][j] > -9e20 ){				
						fCorr[n_base + 3][0][i] += aCorr[nf][1][point] * aCorr[nf3][k][j];
						nCorr[n_base + 3][0][i] += 1; 
					}
				}
			}
	
			// shift to next-level : WARNING: Observe that this is hard-coded to handle only mCorr = 2
			// ideally: (1. / mCorr ) * ( aCorr[nf][k][point] + aCorr[nf][k][point - 1] + ... aCorr[nf][k][point - mCorr + 1] ) -- average over m terms
			if ( ( (point + 1) % mCorr == 0 ) && k < (corLevels - 1) )
				recursive_addCorr( (1. / mCorr) * ( aCorr[nf][k][point] + aCorr[nf][k][point - 1] ), nf, k+1);
			else 
				return;
		}
		//--------------------------------------- velocity histogram calculation --------------------------------------//
		void writeCorr(){

			std::ofstream corrdata("./data/correlationData.dat"); 
			#if SACF_TEST
			std::ofstream corrdata("./cbp7_li_ForLi/thejas/mycorrelationData_p_16.dat"); 
			#endif
			
			// std::cout << "before avg fcorr[0][0][0]= " << fCorr[0][0][0] << std::endl;
	
			// data averaging
			for ( i1=0 ; i1<n_vars*n_vars ; ++i1 ){
				for( j1=0; j1<corLevels ; ++j1 ){
					for( k1=0; k1<pCorr2 ; ++k1 ){
						fCorrAv[i1][j1][k1] = fCorr[i1][j1][k1] / nCorr[i1][j1][k1];
						// fCorr[i1][j1][k1] /= nCorr[i1][j1][k1];
					} // blocks
				} // levels
			} // variables

			for ( i1=0 ; i1<n_vars*n_vars ; ++i1 )
				normalizeCorrAv[i1] = normalizeCorr[i1] / normalizeCorr_count;
				// normalizeCorr[i1] /= normalizeCorr_count;

			// writing data -- writing level 0 and other levels separately
			// Level 0
			for( k1=0 ; k1<pCorr2-1 ; ++k1 ){

				corrdata << k1+1 << "\t\t";
				for( i1=0 ; i1<n_vars*n_vars ; ++i1){
					corrdata << fCorrAv[i1][0][k1] << "\t\t" << normalizeCorrAv[i1] << "\t\t"; 
				}
				corrdata << std::endl;
			}
	
			// Level1 to corLevels-1
			for( j1=1 ; j1<corLevels ; ++j1 ){
				for( k1=0 ; k1<pCorr2 ; ++k1 ){
					
					corrdata << ( pCorr2 + k1 ) * pow(2. , j1 - 1) << "\t\t";
					for ( i1=0 ; i1<n_vars*n_vars ; ++i1 ){
						corrdata << fCorrAv[i1][j1][k1] << "\t\t" << normalizeCorrAv[i1] << "\t\t";
					}
					corrdata << std::endl;	
				}

			} 
			corrdata.close();
		}
		#endif

		//--------------------------------------- Parameter file writing--------------------------------------//
		void paraWrite(std::ofstream& paraInfo){

			paraInfo << "---------------------------" << std::endl;
			paraInfo << "Simulation Box Parameters" << std::endl;
			paraInfo << "---------------------------" << std::endl;
			paraInfo << "Box length x (boxEdge[x])                  :           " << boxEdge[x] << std::endl;
			paraInfo << "Box length y (boxEdge[y])                  :           " << boxEdge[y] << std::endl;
			paraInfo << "Box length z (boxEdge[z])                  :           " << boxEdge[z] << std::endl;
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
			paraInfo << "Pressure Data frequency (psaveCount)       :           " << psaveCount << std::endl;
			
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
			paraInfo << "Set temperature (kbT)                      :           " << kBT << std::endl;
			paraInfo << "Noise level (sigma)                        :           " << sigma << std::endl;
			paraInfo << "Friction parameter (gamma)                 :           " << gamma << std::endl;
			#endif 
			
			#if WALL_ON
			paraInfo << "---------------------------" << std::endl;
			paraInfo << "Wall parameters      " << std::endl;
			paraInfo << "---------------------------" << std::endl;
			paraInfo << "Number of solid particles                  :           " << npart - fluidCount << std::endl;
			paraInfo << "Wall density (initWallRho)                 :           " << initWallRho << std::endl;
			paraInfo << "Solid-Solid Attraction Strength   (Ass)    :           " << Ass << std::endl;
			paraInfo << "Solid-Solid Repulsion  Strength   (Bss)    :           " << Bss << std::endl;
			paraInfo << "Solid-Liquid Attraction Strength   (Asl)   :           " << Asl << std::endl;
			paraInfo << "Solid-Liquid Repulsion  Strength   (Bsl)   :           " << Bsl << std::endl;
			paraInfo << "Soft Repulsive force Strength   (Brep)     :           " << Brep << std::endl;
			paraInfo << "Penetration tolerance (wallPenetration)    :           " << wallPenetration << std::endl;
			paraInfo << "Wall Attraction cutoff (rcWallcutoff )     :           " << rcWcutoff << std::endl;
			paraInfo << "Wall Repulsion  cutoff (rdWall_cutoff )    :           " << rdWcutoff << std::endl;
			paraInfo << "Spring constant for wall ( kWall )         :           " << kWall << std::endl;
			#if BODY_FORCE
			paraInfo << "Particle x body force ( fBodyX )           :           " << fBodyX << std::endl;
			#endif
			#else
			#if BODY_FORCE
			paraInfo << "Particle x body force ( fBodyX )           :           " << fBodyX << std::endl;
			#endif
			#endif // WALL_ON
			
			paraInfo << "---------------------------" << std::endl;
			paraInfo << "Conservative Force         " << std::endl;
			paraInfo << "---------------------------" << std::endl;
			paraInfo << "Liquid-Liquid Attraction Strength (All)    :           " << All << std::endl;
			paraInfo << "Liquid-Liquid Repulsion Strength (Bll)     :           " << Bll << std::endl;

			#if LEES_EDWARDS_BC
			paraInfo << "---------------------------" << std::endl;
			paraInfo << "Lees-Edwards BC            " << std::endl;
			paraInfo << "---------------------------" << std::endl;
			paraInfo << "Homogeneous shear-rate (gammaDot)          :            " << gammaDot << std::endl;
			paraInfo << "Strain-rate (strainRate)                   :            " << strainRate << std::endl;
			#endif

			#if SACF
			paraInfo << "-------------------------------" << std::endl;
			paraInfo << "Stress-Autocorrelation function" << std::endl;
			paraInfo << "-------------------------------" << std::endl;
			paraInfo << "Number of variables (nvars )               :            " << n_vars << std::endl;
			paraInfo << "Levels of correlation (corLevels)          :            " << corLevels << std::endl;
			paraInfo << "Blocks in a level (pCorr)                  :            " << pCorr << std::endl;
			paraInfo << "Average length in a level (mCorr)          :            " << mCorr << std::endl;
			#endif
			#if CAPILLARY_TUBE
			paraInfo << "-------------------------------" << std::endl;
			paraInfo << "Capillary imbibition           " << std::endl;
			paraInfo << "-------------------------------" << std::endl;
			paraInfo << "Buffer length left of capillary (bufferLen):            " << bufferLen << std::endl;	
			paraInfo << "Capillary tube length ( capLen )           :            " << capLen << std::endl;
			paraInfo << "Capillary radius ( capRad )                :            " << capRad << std::endl;
			paraInfo << "Wall width adj. to capillary (capWallWdth) :            " << capWallWdth << std::endl;
			paraInfo << "Initial width of reservoir (resWdth)       :            " << resWdth << std::endl;
			#endif
			paraInfo << "---------------------------" << std::endl;
			paraInfo << "---------------------------" << std::endl;
			paraInfo << "---------------------------" << std::endl;


			/******************************* FLAGS ***************************************/
			flagList.open ( "./flagList.txt", std::ios::out );
			#if RANDOM_DISSIPATIVE           
			flagList << "RANDOM_DISSIPATIVE FLAG                :            " << "ON" << std::endl;  
			#endif
			#if SPHERICAL_DROPLET                
			flagList << "SPHERICAL_DROPLET FLAG                 :            " << "ON" << std::endl;  
			#endif
			#if SPHERICAL_CAP                    
			flagList << "SPHERICAL_CAP FLAG                     :            " << "ON" << std::endl;  
			#endif
			#if CYLINDER_DROPLET                 
			flagList << "CYLINDER_DROPLET FLAG                  :            " << "ON" << std::endl;  
			#endif
			#if PLANAR_SLAB                      
			flagList << "PLANAR_SLAB FLAG                       :            " << "ON" << std::endl;  
			#endif
			#if CRYSTAL	                         
			flagList << "CRYSTAL FLAG                           :            " << "ON" << std::endl;  
			#endif
			#if RESTART	                          
			flagList << "RESTART FLAG                           :            " << "ON" << std::endl;  
			#endif

			// WALL flags
			#if WALL_ON                          
			flagList << "WALL_ON FLAG                           :            " << "ON" << std::endl;  
			#if LOWER_WALL_ON                    
			flagList << "LOWER_WALL_ON FLAG                     :            " << "ON" << std::endl;  
			#endif
			#if UPPER_WALL_ON                    
			flagList << "UPPER_WALL_ON FLAG                     :            " << "ON" << std::endl;  
			#endif
			#if FCC_WALL                         
			flagList << "FCC_WALL FLAG                          :            " << "ON" << std::endl;  
			#endif
			#if ROUGH_WALL                       
			flagList << "ROUGH_WALL FLAG                        :            " << "ON" << std::endl;  
			#endif
			#endif

			// POISEUILLE flow
			#if BODY_FORCE                       
			flagList << "BODY_FORCE FLAG                        :            " << "ON" << std::endl;  
			#endif

			// FILE_WRITE
			#if STYLE_VMD                        
			flagList << "STYLE_VMD FLAG                         :            " << "ON" << std::endl;  
			#endif
			#if STYLE_MERCURY_DPM                
			flagList << "STYLE_MERCURY_DPM FLAG                 :            " << "ON" << std::endl;  
			#endif

			// CORRELATION FUNCTIONS
			#if SACF                              
			flagList << "SACF FLAG                              :            " << "ON" << std::endl;  
			#endif
			#if SACF_TEST                        
			flagList << "SACF_TEST FLAG                         :            " << "ON" << std::endl;  
			#endif

			// LEES-EDWARDS BOUNDARY CONDITION
			#if LEES_EDWARDS_BC                  
			flagList << "LEES_EDWARDS_BC FLAG                   :            " << "ON" << std::endl;  
			#endif

			// DENSITY CALCULATION
			#if DENS_EXACT			 
			flagList << "DENS_EXACT FLAG                        :            " << "ON" << std::endl;  
			#endif
			
			// CAPILLARY IMBIBITION
			#if CAPILLARY_TUBE		 
			flagList << "CAPILLARY TUBE FLAG                    :            " << "ON" << std::endl;  
			#endif

		}

		//--------------------------------------- Velocity, Momentum and Pressure file writing--------------------------------------//
		void fileWrite( std::ofstream& enStats, std::ofstream& eosStats, std::ofstream& momStats, std::ofstream& pTensStats ){

			if ( step % saveCount == 0){
				simProg << step << " steps out of " << stepMax << " completed " << std::endl;
			}

			// separate module for pressure -- requires better averaging
			if ( pcounter >= psaveCount ) {
				
				// average and reset the pTensor
				#include "pTensAverage.h"

				// Pressure tensor	
				pTensStats 	<<  pIdeal[0][0]		 << " " 
					        <<  pIdeal[0][1]		 << " " 
	        				<<  pIdeal[0][2]		 << " "
		        			<<  pIdeal[1][0]		 << " "
			        		<<  pIdeal[1][1]		 << " "
				        	<<  pIdeal[1][2]		 << " "
				        	<<  pIdeal[2][0]		 << " "
	        				<<  pIdeal[2][1]		 << " "
		        			<<  pIdeal[2][2]		 << " "

			        		<<  pNonIdeal[0][0]		 << " " 
				        	<<  pNonIdeal[0][1]		 << " " 
        					<<  pNonIdeal[0][2]		 << " "
	        				<<  pNonIdeal[1][0]		 << " "
		        			<<  pNonIdeal[1][1]		 << " "
		        			<<  pNonIdeal[1][2]		 << " "
			        		<<  pNonIdeal[2][0]		 << " "
		        			<<  pNonIdeal[2][1]		 << " "
			        		<<  pNonIdeal[2][2]		 << " " 

				        	<<  pNonIdealKin[0][0]		 << " " 
				        	<<  pNonIdealKin[0][1]		 << " " 
        					<<  pNonIdealKin[0][2]		 << " "
	        				<<  pNonIdealKin[1][0]		 << " "
		        			<<  pNonIdealKin[1][1]		 << " "
		        			<<  pNonIdealKin[1][2]		 << " "
			        		<<  pNonIdealKin[2][0]		 << " "
		        			<<  pNonIdealKin[2][1]		 << " "
			        		<<  pNonIdealKin[2][2]		 << " " 

			        		<<  pDissipative[0][0]	         << " " 
				        	<<  pDissipative[0][1]	         << " " 
        					<<  pDissipative[0][2]	         << " "
	        				<<  pDissipative[1][0]	         << " "
		        			<<  pDissipative[1][1]	         << " "
		        			<<  pDissipative[1][2]	         << " "
			        		<<  pDissipative[2][0]	         << " "
		        			<<  pDissipative[2][1]	         << " "
			        		<<  pDissipative[2][2]	         << " " 

			        		<<  pRandom[0][0]                << " " 
				        	<<  pRandom[0][1]                << " " 
        					<<  pRandom[0][2]                << " "
	        				<<  pRandom[1][0]                << " "
		        			<<  pRandom[1][1]                << " "
		        			<<  pRandom[1][2]                << " "
			        		<<  pRandom[2][0]                << " "
		        			<<  pRandom[2][1]                << " "
			        		<<  pRandom[2][2]                << std::endl;
						
				#include "pTensReset.h"

				pcounter = 0;	
			}

			//write output file in the .data format
			if (counter>=saveCount) {				

				// particle positions in vtk file format
				vtkFileWritePosVel();

				// Energy
						#if LEES_EDWARDS_BC
				enStats 	<< std::setw(20) << std::setprecision(15) << pot_en << "\t" 
					        << std::setw(20) << std::setprecision(15) << kin_en << "\t" 
				        	<< std::setw(20) << std::setprecision(15) << tot_en << "\t"
				        	<< std::setw(20) << std::setprecision(15) << dissipativeWork << "\t"
				        	<< std::setw(20) << std::setprecision(15) << randomWork << std::endl;
						#else
				enStats 	<< std::setw(20) << std::setprecision(15) << pot_en << "\t" 
					        << std::setw(20) << std::setprecision(15) << kin_en << "\t" 
				        	<< std::setw(20) << std::setprecision(15) << tot_en << std::endl;
						#endif

				// Density, Temperature, Average Pressure
				eosStats 	<< std::setw(20) << std::setprecision(15) << rho 	<< "\t" 
					        << std::setw(20) << std::setprecision(15) << temp	<< "\t" 
					        #if WALL_ON
					        << std::setw(20) << std::setprecision(15) << wallTemp	<< "\t" 
					        #endif
				        	<< std::setw(20) << std::setprecision(15) << pressure 	<< std::endl;

				// Momentum
				momStats 	<< std::setw(20) << std::setprecision(15) << momX << "\t" 
				        	<< std::setw(20) << std::setprecision(15) << momY << "\t" 
				        	<< std::setw(20) << std::setprecision(15) << momZ << std::endl;


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

					#if WALL_ON
					/*
					// COM calculation
					if ( step > nZ_tStart ) {
						sprintf( filename, "./data/segPlane_%d.dat", step );  
						std::ofstream segPlaneStats( filename );
					
						// writing the center of mass		
						for ( segPlane_ind = 0; segPlane_ind < segPlane_bins; ++segPlane_ind ){
							segPlane_xCOM[segPlane_ind] /= segPlane_count[segPlane_ind];
							segPlane_zCOM[segPlane_ind] /= segPlane_count[segPlane_ind];

							segPlaneStats << segPlane_xCOM[segPlane_ind] << "\t" << segPlane_zCOM[segPlane_ind] << std::endl;

							// reset values
							segPlane_count[segPlane_ind] = 0;
							segPlane_xCOM[segPlane_ind] = 0.0;
							segPlane_zCOM[segPlane_ind] = 0.0;
						}

					}
					*/

					/*
					if ( step > nZ_tStart){
						sprintf( filename, "./data/nzStats_%d.dat", step );  
						std::ofstream nZStats( filename );

						for ( nZ_indz = 0; nZ_indz < nZ_zbins; ++nZ_indz ){
							for ( nZ_indx = 0; nZ_indx < nZ_xbins; ++nZ_indx ){

								nZStats << ( nZ[nZ_indz][nZ_indx] / counter ) << "\t";
							}
							nZStats << "\n" << std::endl;
						}
					}
					*/
					#endif

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

			/*
			file << "<?xml version=\"1.0\"?>" << std::endl;
			file << "<VTKFile type=\"UnstructuredGrid\" version=\"0.1\" byte_order=\"LittleEndian\">" << std::endl;
			file << "<UnstructuredGrid>";
			file << "<Piece NumberOfPoints=\""<< particles.size() <<"\" NumberOfCells=\"0\">" << std::endl;
			file << "<Points>" << std::endl;
			// position data -- particles 
			file << "<DataArray type=\"Float32\" Name=\"Position\" NumberOfComponents=\"3\" format=\"ascii\">"<< std::endl;
			*/

			#if STYLE_VMD 
			char filename[40];
			char filename1[40];

			// sprintf( filename, "./data/data1_%d.vtu", step);  
			sprintf( filename, "./data/XYZ%d.xyz", step);  
			sprintf( filename1, "./data/velocity%d.dat", step);  
			
			std::ofstream file(filename);
			std::ofstream file1(filename1);

			file << particles.size() << std::endl;
			file << "#X Y Z co-ordinates" << std::endl;
			file1 << particles.size() << std::endl;
			file1 << "#vx vy vz" << std::endl;


			if ( fluidCount != 0 ) {	
				for ( i = fluid_index[0] ; i <= fluid_index[fluidCount-1] ; ++i ) {
					file  << "H" << "\t" << particles[i].r << std::endl;
					file1 << "H" << "\t" << particles[i].v << std::endl;
				}
			}
				#if WALL_ON	
				for ( i = solid_index[0] ; i <= solid_index[solidCount-1] ; ++i ) {
					file  << "O" << "\t" << particles[i].r << std::endl;
					file1 << "O" << "\t" << particles[i].v << std::endl;
				}
				#endif
			#endif

			#if STYLE_MERCURY_DPM
			char filename[40];
			char filename1[40];

			// sprintf( filename, "./data/data1_%d.vtu", step);  
			sprintf( filename, "./data/fluid_XYZ%d.data", step);  
			
			std::ofstream file(filename);
			std::ofstream file1(filename1);

			file << particles.size() << ", " << step << " 0, 0, 0, " << boxEdge[x] << ", " << boxEdge[y] << ", " << boxEdge[z] << std::endl;
			for ( i = fluid_index[0] ; i <= fluid_index[fluidCount-1] ; ++i )
				file  <<  particles[i].r << " " << particles[i].v << " " << particles[i].a << " 0 0 0 0 0 0 " << particles[i].type << std::endl;

			#endif
			/*
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
			*/
			file.close();

		}

		//-------------------- Final velocity and positions ----------------------//
		void finalposvelWrite( std::ofstream& writeConfig ){

			writeConfig.write( reinterpret_cast< const char * >( &npart ), sizeof( npart ) );
			writeConfig.write( reinterpret_cast< const char * >( &step ), sizeof( step ) );
			writeConfig.write( reinterpret_cast< const char * >( &seed ), sizeof( std::default_random_engine ) );

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

		//------------------------------ Mod function ------------------------------//
		int moduloAB( int A, int B){ 
		
			int result = std::fmod( A, B);

			result = ( result < 0 ) ? ( result + B ) : result; 

			return( result );
		}
};

#endif
