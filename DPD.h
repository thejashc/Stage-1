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
#include <chrono>

// configuration of particles
#define RANDOM_DISSIPATIVE		1
#define RESTART				    0

// WALL flags
#define WALL_ON				    1
#define SPRING_CONNECTED_SLD    1
#define BCKGRND_CONNECTED_SLD   1

#define CAPILLARY_CYLINDER		1
#define CAPILLARY_SQUARE		0
#define HARD_SPHERES            1

// DENSITY CALCULATION
#define DENS_EXACT			    0    

//declare DPD solver
class DPD {
	public:
		#include "vars_list.h"

		/******************************************************************************************************/
		/**************************************** SOLVE ROUTINE ***********************************************/
		/******************************************************************************************************/
		//solve contains the time stepping algorithm and the output routines 
		int solve () {

            auto start = std::chrono::high_resolution_clock::now();

			simProg.open ( "./simProg.txt", std::ios::out );
			simProg << " Proccess ID : " << getpid() << std::endl;
			// initialize position, velocity, hoc, ll

			// parameters declaration
			pTensCounter    = 0;
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
			half_dt 		= 0.5*dt;					// 0.5*dt to be used in the integrateEOM()
			half_dt_sqr     = half_dt*dt;					// 0.5*dt*dt to be used in the integrateEOM()

			rd4 			= pow(rd_cutoff, 4.0);				// fourth power of rd_cutoff
			rc4 			= pow(rcutoff, 4.0);				// fourth power of rc_cutoff
			K1 			    = piThirty * rc4 * Aatt[1][1];				// constant k1
			K2 			    = piThirty * rd4 * Brep[1][1];				// constant k2

			counter 		= 0;						// initialize time, counter for file writing
			pcounter 		= 0;						// initialize time, counter for file writing

			#if !(RESTART)
                i = 0;
                while( i < npart ){
                    #include "pbcNew.h"

                    i++;
                }
				createGridList();

                #if WALL_ON 
                    #if SPRING_CONNECTED_SLD
                        #include "psfWrite.h"
                        springNetworkBox();
                    #endif
                #endif 
    
				dens_calculation();
				forceCalc();
				resetVar();
            #else
                i = 0;
                while( i < npart ){
                    #include "pbcNew.h"
                    i++;
                }
				createGridList();       // density calculation is taken care of

                #if WALL_ON
                    #if SPRING_CONNECTED_SLD
                        springNetworkBox();
                    #endif
                #endif 
			#endif

			#if RESTART	
				std::ofstream enStats 		( "./data/en_data.dat"	, std::ios_base::app);	// Kinetic, Potential and total energy
				std::ofstream eosStats		( "./data/eos_data.dat"	, std::ios_base::app);	// Mean Pressure and temperature data
				std::ofstream pTensStats	( "./data/pTens.dat"	, std::ios_base::app);	// Pressure tensor data
				std::ofstream momStats		( "./data/mom_data.dat"	, std::ios_base::app);	// pressure and temperature data
			#else
                std::ofstream enStats       ( "./data/en_data.dat"  , std::ios_base::app);  // Kinetic, Potential and total energy
                std::ofstream eosStats      ( "./data/eos_data.dat" , std::ios_base::app);  // Mean Pressure and temperature data
                std::ofstream pTensStats    ( "./data/pTens.dat"    , std::ios_base::app);  // Pressure tensor data
                std::ofstream momStats      ( "./data/mom_data.dat" , std::ios_base::app);  // pressure and temperature data
                #if HARD_SPHERES
                    std::ofstream colloidStats   ("./data/colloid_com.dat", std::ios_base::app );
                #endif
			#endif // RESTART

			// write parameters and initial configuration
			//asciiFileWritePosVel();
			binFileWritePosVel();
			std::ofstream paraInfo( "param.out" );
			paraWrite(paraInfo);
			paraInfo.close();

			simProg << " ********************* STARTING SIMULATION ************************* " << std::endl;	
			while (step<= stepMax) {

				createGridList();
				#if DENS_EXACT
					dens_calculation();
				#endif

				forceCalc();
                // forceCalc_bruteForce();

				integrateEOM();
               
                /*
				for ( i=0 ; i <npart ; ++i ) {
					pot_en += K1 * particles[i].rhoBar + K2 * pow( particles[i].dens, 2.0 ); 	// sum of self-energies of all particles
				}
                */
				tot_en = kin_en + pot_en;

				counter += 1;						// filewriting
				fileWrite(enStats, eosStats, momStats, pTensStats, colloidStats);
               

				resetVar();						// reset variables to zero
				
				if ( step % rstrtFwrtFreq == 0 ){
					// write position velocity stats 
					std::ofstream writeConfig;
                    char fName[100];
                    sprintf(fName, "./restart/posvelrestartfile.dat");
					writeConfig.open ( fName, std::ios::binary | std::ios::out );	// example binary file
					finalposvelWrite( writeConfig );	
					writeConfig.close();
				}

				step += 1;						// increment time step

                if( step % 1000 == 0 ){

                    double tMax = 672.;
                    auto stop = std::chrono::high_resolution_clock::now();
                    auto duration = std::chrono::duration_cast<std::chrono::hours>(stop-start);
                    //auto duration = stop-start; // even this command works
        
                    simProg << "The run time is = " << duration.count() << " hours \n";

                    if ( duration.count() > tMax ){
                        simProg << "The simulation has exceeded the max. time of 504 hours and will now be stopped. \n";
                        simProg << " ********************* ENDING SIMULATION ************************* ";	
                        simProg.close();

                        return(0);
                    }
                }

			}//end time loop

			// enStats.close();	// close file-streams
			// eosStats.close();
			// momStats.close();
			
			simProg << " ********************* ENDING SIMULATION *************************";	
			simProg.close();

		} //void solve()
		//--------------------------------------- INITIALIZATION ROUTINE --------------------------------------//
		void init(){
            
			#include "paramIn.h"
            #include "geometryIn.h"

			#if WALL_ON

				#if CAPILLARY_CYLINDER
                    // GLASSY WALL
                    pCount=0;
                    bckgIdxStart = pCount;
                    #include "glassyCylinder.h"
                    bckgIdxEnd = pCount - 1;
                #endif

                #if HARD_SPHERES
                    ngbrIdxStart = particles.size();
                    #include "fluidInCylinder.h"
                    ngbrIdxEnd=particles.size()-1;
                #endif
                
				#if RESTART 
					#include "restartConfig.h"
				#endif

			#else

				#if RESTART 
					#include "restartConfig.h"
				#endif

			#endif // WALL_ON

            #include "cellGridInit.h"

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
            solidCount = 0;

            simProg << "*************************************************** \n" ;
            simProg << "Counting total particles \n" << std::endl;
            //fluidEndIndex = 0;
           // solidEndIndex = 0;

			for ( i = 0; i < particles.size(); ++i ){
                if ( particles[i].type == 1 || particles[i].type == 2 || particles[i].type == 5 ){
                    fluid_index.push_back(i);
                    fluidCount++;
                }
				else if( particles[i].type == 0 || particles[i].type == 3 || particles[i].type == 4 ){
					solid_index.push_back(i);
					solidCount++;
				}

                #if HARD_SPHERES
                    particles[i].rUnfolded = particles[i].r;
                #endif
			}

            simProg << fluidCount << " fluid particles indexed \n" << std::endl;
            simProg << solidCount << " solid particles indexed \n" << std::endl;
            simProg << fluidCount + solidCount << " total particles indexed \n";

			#if WALL_ON

                #if SPRING_CONNECTED_SLD
                simProg << "*************************************************** \n" << std::endl;
                simProg << "Spring Connected Solid Structures \n" << std::endl;
				idx = 0;
                ngbrIdxParticles=0;
                i = solid_index[idx];

                // simProg << " i = "  << i << ", ngbrIdxStart= " << ngbrIdxStart << " , ngbrIdxEnd " << ngbrIdxEnd << std::endl;

                    while( idx < solidCount ){

                        if ( i >= ngbrIdxStart && i <= ngbrIdxEnd ){
                            particles[i].bondIndex[0] = 0;
               
                            // assign spring constants to particles
                            #if PISTON

                            if( i >= pistonStartIndex && i <= pistonEndIndex )
                               particles[i].springConstant = kWallNgbr2;
                            else
                               particles[i].springConstant = kWallNgbr1;

                            #else
                               particles[i].springConstant=kWallNgbr1; 
                            #endif

                            ngbrIdxParticles++;
                        }

                        idx++;
                        i = solid_index[idx];
                    }
                    #if PISTON
                    simProg << "\n" << "Particles within indices " << pistonStartIndex << " and " << pistonEndIndex << " assigned spring constant " << kWallNgbr2;
                    #endif
                    simProg << "\n" << "Particles not belonging to piston assigned the spring constant " << kWallNgbr1;
                    simProg << "\n" << ngbrIdxParticles << " solid particles from " << ngbrIdxStart << " and " << ngbrIdxEnd  << " have their bond indices set to 0 \n"<< std::endl;

                    #if HARD_SPHERES
                        nPartColloid=ngbrIdxParticles;
                    #endif

                #endif

                #if BCKGRND_CONNECTED_SLD
                simProg << "*************************************************** \n" << std::endl;
                simProg << "Background Connected Solid Structures \n"<<std::endl;
				idx = 0;
                bckgIdxParticles=0;
                i = solid_index[idx];

                    #if RESTART
                        while( idx < solidCount ){

                            if ( i >= bckgIdxStart && i <= bckgIdxEnd ){
                                bckgIdxParticles++;
                            }
                            idx++;
                            i = solid_index[idx];
                        }
                    #else
                        while( idx < solidCount ){

                            if ( i >= bckgIdxStart && i <= bckgIdxEnd ){

                                particles[i].r0 = particles[i].r;
                                bckgIdxParticles++;
                            }

                            idx++;
                            i = solid_index[idx];
                        }
                    #endif
                    simProg << "\n" << bckgIdxParticles << " solid particles from " << bckgIdxStart << " and " << bckgIdxEnd  << " have their positions attached to their initial position \n"<< std::endl;
                #endif

			#endif // WALL_ON
			
            // defining the matrix for Brep and Aatt
            simProg << " ********************************************************************* \n";
            simProg << " Defining the matrix of repulsion parameters and attraction parameters \n";

            Brep[0][0] = Bs1_s1;        Aatt[0][0] = As1_s1;        // Solid 1
            Brep[0][1] = Bs1_l1;        Aatt[0][1] = As1_l1;     
            Brep[0][2] = Bs1_l2;        Aatt[0][2] = As1_l2;
            Brep[0][3] = Bs1_s2;        Aatt[0][3] = As1_s2;
            Brep[0][4] = Bs1_s3;        Aatt[0][4] = As1_s3;
            Brep[0][5] = Bs1_l3;        Aatt[0][5] = As1_l3;
                                 
            Brep[1][0] = Brep[0][1];    Aatt[1][0] = Aatt[0][1];    // Liquid 1
            Brep[1][1] = Bl1_l1;        Aatt[1][1] = Al1_l1;
            Brep[1][2] = Bl1_l2;        Aatt[1][2] = Al1_l2;
            Brep[1][3] = Bl1_s2;        Aatt[1][3] = Al1_s2;     
            Brep[1][4] = Bl1_s3;        Aatt[1][4] = Al1_s3;
            Brep[1][5] = Bl1_l3;        Aatt[1][5] = Al1_l3;
                                 
            Brep[2][0] = Brep[0][2];    Aatt[2][0] = Aatt[0][2];    // Liquid 2
            Brep[2][1] = Brep[1][2];    Aatt[2][1] = Aatt[1][2];
            Brep[2][2] = Bl2_l2;        Aatt[2][2] = Al2_l2;
            Brep[2][3] = Bl2_s2;        Aatt[2][3] = Al2_s2;   
            Brep[2][4] = Bl2_s3;        Aatt[2][4] = Al2_s3;
            Brep[2][5] = Bl2_l3;        Aatt[2][5] = Al2_l3;
                                 
            Brep[3][0] = Brep[0][3];    Aatt[3][0] = Aatt[0][3];    // Solid 2
            Brep[3][1] = Brep[1][3];    Aatt[3][1] = Aatt[1][3];
            Brep[3][2] = Brep[2][3];    Aatt[3][2] = Aatt[2][3];
            Brep[3][3] = Bs2_s2;        Aatt[3][3] = As2_s2;
            Brep[3][4] = Bs2_s3;        Aatt[3][4] = As2_s3;
            Brep[3][5] = Bs2_l3;        Aatt[3][5] = As2_l3;
                                 
            Brep[4][0] = Brep[0][4];    Aatt[4][0] = Aatt[0][4];    // Solid 3
            Brep[4][1] = Brep[1][4];    Aatt[4][1] = Aatt[1][4];
            Brep[4][2] = Brep[2][4];    Aatt[4][2] = Aatt[2][4];
            Brep[4][3] = Brep[3][4];    Aatt[4][3] = Aatt[3][4];
            Brep[4][4] = Bs3_s3;        Aatt[4][4] = As3_s3;
            Brep[4][5] = Bs3_l3;        Aatt[4][5] = As3_l3;

            Brep[5][0] = Brep[0][5];    Aatt[5][0] = Aatt[0][5];    // Liquid 3
            Brep[5][1] = Brep[1][5];    Aatt[5][1] = Aatt[1][5];
            Brep[5][2] = Brep[2][5];    Aatt[5][2] = Aatt[2][5];
            Brep[5][3] = Brep[3][5];    Aatt[5][3] = Aatt[3][5];
            Brep[5][4] = Brep[4][5];    Aatt[5][4] = Aatt[4][5];
            Brep[5][5] = Bl3_l3;        Aatt[5][5] = Al3_l3;
                
            simProg << " Finished defining the matrix of repulsion parameters and attraction parameters \n";
            simProg << " ********************************************************************* \n";

			// determine the number of types of particles present
            #if RANDOM_DISSIPATIVE
                simProg << " ********************************************************************* \n";
                simProg << " Defining the matrix of noise and friction parameters \n";

                sigma.resize( 6 );      // 3 solid types + 3 liquid types
                gamma.resize( 6 ); 

                for ( i=0; i < 6; ++i ){
                    sigma[i].resize( 6 ); 
                    gamma[i].resize( 6 ); 
                }    

                // defining the elements of the sigma and gamma array
               sigma[0][0]  = noise_s1_s1;                  // S1-S1 
               sigma[0][1]  = noise_s1_l1;                  // S1-L1
               sigma[0][2]  = noise_s1_l2;                  // S1-L2
               sigma[0][3]  = noise_s1_s2;                  // S1-S2
               sigma[0][4]  = noise_s1_s3;                  // S1-S3
               sigma[0][5]  = noise_s1_l3;                  // S1-L3

               sigma[1][0]  = sigma[0][1];                  // L1-S1 
               sigma[1][1]  = noise_l1_l1;                  // L1-L1    
               sigma[1][2]  = noise_l1_l2;                  // L1-L2 
               sigma[1][3]  = noise_l1_s2;                  // L1-S2 = L1-S1
               sigma[1][4]  = noise_l1_s3;                  // L1-S3
               sigma[1][5]  = noise_l1_l3;                  // L1-L3

               sigma[2][0]  = sigma[0][2];                  // L2-S1
               sigma[2][1]  = sigma[1][2];                  // L2-L1 
               sigma[2][2]  = noise_l2_l2;                  // L2-L2        
               sigma[2][3]  = noise_l2_s2;                  // L2-S2 
               sigma[2][4]  = noise_l2_s3;                  // L2-S3
               sigma[2][5]  = noise_l2_l3;                  // L2-L3

               sigma[3][0]  = sigma[0][3];                  // S2-S1 
               sigma[3][1]  = sigma[1][3];                  // S2-L1
               sigma[3][2]  = sigma[2][3];                  // S2-L2
               sigma[3][3]  = noise_s2_s2;                  // S2-S2
               sigma[3][4]  = noise_s2_s3;                  // S2-S3
               sigma[3][5]  = noise_s2_l3;                  // S2-L3

               sigma[4][0]  = sigma[0][4];                  // S3-S1 
               sigma[4][1]  = sigma[1][4];                  // S3-L1
               sigma[4][2]  = sigma[2][4];                  // S3-L2
               sigma[4][3]  = sigma[3][4];                  // S3-S2
               sigma[4][4]  = noise_s3_s3;                  // S3-S3
               sigma[4][5]  = noise_s3_l3;                  // S3-L3

               sigma[5][0]  = sigma[0][5];                  // L3-S1 
               sigma[5][1]  = sigma[1][5];                  // L3-L1
               sigma[5][2]  = sigma[2][5];                  // L3-L2
               sigma[5][3]  = sigma[3][5];                  // L3-S2
               sigma[5][4]  = sigma[4][5];                  // L3-S3
               sigma[5][5]  = noise_l3_l3;                  // L3-L3

               gamma[0][0]  = friction_s1_s1;               // S1-S1 
               gamma[0][1]  = friction_s1_l1;               // S1-L1
               gamma[0][2]  = friction_s1_l2;               // S1-L2
               gamma[0][3]  = friction_s1_s2;               // S1-S2
               gamma[0][4]  = friction_s1_s3;               // S1-S3
               gamma[0][5]  = friction_s1_l3;               // S1-L3

               gamma[1][0]  = gamma[0][1];                  // L1-S1
               gamma[1][1]  = friction_l1_l1;               // L1-L1     
               gamma[1][2]  = friction_l1_l2;               // L1-L2     
               gamma[1][3]  = friction_l1_s2;               // L1-S2     
               gamma[1][4]  = friction_l1_s3;               // L1-S3
               gamma[1][5]  = friction_l1_l3;               // L1-L3

               gamma[2][0]  = gamma[0][2];                  // L2-S1
               gamma[2][1]  = gamma[1][2];                  // L2-L1
               gamma[2][2]  = friction_l2_l2;               // L2-L2    
               gamma[2][3]  = friction_l2_s2;               // L2-S2    
               gamma[2][4]  = friction_l2_s3;               // L2-S3
               gamma[2][5]  = friction_l2_l3;               // L2-L3

               gamma[3][0]  = gamma[0][3];                  // S2-S1 
               gamma[3][1]  = gamma[1][3];                  // S2-L1
               gamma[3][2]  = gamma[2][3];                  // S2-L2
               gamma[3][3]  = friction_s2_s2;               // S2-S2
               gamma[3][4]  = friction_s2_s3;               // S2-S3
               gamma[3][5]  = friction_s2_l3;               // S2-L3

               gamma[4][0]  = gamma[0][4];                  // S3-S1 
               gamma[4][1]  = gamma[1][4];                  // S3-L1
               gamma[4][2]  = gamma[2][4];                  // S3-L2
               gamma[4][3]  = gamma[3][4];                  // S3-S2
               gamma[4][4]  = friction_s3_s3;               // S3-S3
               gamma[4][5]  = friction_s3_l3;               // S3-L3

               gamma[5][0]  = gamma[0][5];                  // L3-S1 
               gamma[5][1]  = gamma[1][5];                  // L3-L1
               gamma[5][2]  = gamma[2][5];                  // L3-L2
               gamma[5][3]  = gamma[3][5];                  // L3-S2
               gamma[5][4]  = gamma[4][5];                  // L3-S3
               gamma[5][5]  = friction_l3_l3;               // L3-L3

               simProg << " ********************************************************************* \n";
               simProg << " Finished defining the matrix of noise and friction parameters \n";
            #endif // RANDOM_DISSIPATIVE
            
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
					simProg << "*** position: " << particles[i].r << "\n";
					simProg << mini[x] << " < " << mi[x] << " < " << maxi[x] << std::endl;
					simProg << mini[y] << " < " << mi[y] << " < " << maxi[y] << std::endl;
					simProg << mini[z] << " < " << mi[z] << " < " << maxi[z] << std::endl;
					abort();
				}
				if ( grid[mi[x]][mi[y]][mi[z]][0] == MaxPerCell )
				{ 
					simProg << "*** cell overfull at time step = " << step << " with " << grid[mi[x]][mi[y]][mi[z]][0] << " particles in cell" << std::endl;
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
                                #include "pairforce.h"

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
                                        
										#include "pairforce.h"

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

											#include "pairforce.h"
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

                                    #include "pairforce.h"

								} // jj
							} // m
							#endif

						} // ii

		}

		//--------------------------------------- Integrate Equations of motion --------------------------------------//
		void integrateEOM(){

            #if SPRING_CONNECTED_SLD && BCKGRND_CONNECTED_SLD
                #include "springNetworkForceNew.h"  
            #elif SPRING_CONNECTED_SLD && !(BCKGRND_CONNECTED_SLD)
                #include "springNetworkForceOnly.h"  
            #elif !(SPRING_CONNECTED_SLD) && BCKGRND_CONNECTED_SLD
                #include "backgroundForceOnly.h"  
            #endif

          i = 0;
          while ( i < npart ){

                // calculate total center of mass momentum
                totCOM += particles[i].w; 

                // store velocity (mid-step)
                particles[i].r_old = particles[i].r;        // position at t: r(t)
                particles[i].w_old = particles[i].w;        // velocity at t-dt/2 : v(t - dt/2)
            
                particles[i].w += ( particles[i].fC       + 
                                    particles[i].fD       + 
                                    particles[i].fR       + 
                                    particles[i].fHarmonic  + 
                                    particles[i].fext )*( dt/particles[i].m );      // evaluating velocity at t+dt/2 : v(t+dt/2)

                particles[i].r += particles[i].w*dt;                                // evaluating position at t+dt: r(t+dt)	

                // implement periodic boundary condition 
                #include "pbcNew.h"
                //#include "pbcNewReflecting.h"
                //#include "pbcXOnly.h"

                // calculate velocity (integral time step)
                particles[i].v = 0.5*( particles[i].w_old + particles[i].w );       // calculate v(t) = v(t-dt/2) + v(t+dt/2)

                momX += particles[i].w.X;
                momY += particles[i].w.Y;
                momZ += particles[i].w.Z;

                #if HARD_SPHERES 
                    /* WARNING : THIS WORKS ( MAKES SENSE ) ONLY FOR A SINGLE COLLOIDAL PARTICLE IN  A BATH OF FLUID ONLY*/
                    /* WARNING : The condition used to recognize the colloid is extremely simplified.
                     *           Best method is to identify the co-ordinates of the colloid and add up the
                     *           trajectories */
                    if ( particles[i].type == 4  ){
                        particles[i].rUnfolded += particles[i].w * dt;
                        colloid_com_pos += (particles[i].rUnfolded/nPartColloid);
                    }
                #endif

                // update count for fluid particles
                i++;
          }

		} // run over all fluid particles
		//--------------------------------------- Resetting variables--------------------------------------//
		void resetVar(){
			// energy reset to zero
			pot_en		= 0.;
			kin_en		= 0.;
			tot_en		= 0.;
			vx2		    = 0.;
			vy2		    = 0.;
			vz2		    = 0.;
			v2		    = 0.;
			vxvy		= 0.;
			pressure	= 0.;
			momX		= 0.;
			momY		= 0.;
			momZ		= 0.;

            totCOM.X      = 0.;
            totCOM.Y      = 0.;
            totCOM.Z      = 0.;

            #if HARD_SPHERES
                colloid_com_pos.X = 0.;
                colloid_com_pos.Y = 0.;
                colloid_com_pos.Z = 0.;
            #endif

            fWCA.setZero();

			for ( i = 0; i < npart ; ++i )		// traversing over all particles
			{
				#if DENS_EXACT	
					particles[i].dens = 0.0;
				#else
					particles[i].dens = particles[i].dens_new;
					particles[i].dens_new = 0.;
				#endif

				particles[i].rhoBar = 0.0;
				particles[i].fC.setZero();
                particles[i].fR.setZero();
                particles[i].fD.setZero();
                particles[i].fext.setZero();
                particles[i].fHarmonic.setZero();

                // springPotEn = 0;

			} // set density equal to zero for solid type particles

		}
		//--------------------------------------- Parameter file writing--------------------------------------//
		void paraWrite(std::ofstream& paraInfo){

			paraInfo << "---------------------------" << "\n";
			paraInfo << "Simulation Box Parameters" << "\n";
			paraInfo << "---------------------------" << "\n";
			paraInfo << "Box length x (boxEdge[x])                  :           " << boxEdge[x] << "\n";
			paraInfo << "Box length y (boxEdge[y])                  :           " << boxEdge[y] << "\n";
			paraInfo << "Box length z (boxEdge[z])                  :           " << boxEdge[z] << "\n";
			paraInfo << "Initial fluid density (initRho)            :           " << initRho << "\n";
			paraInfo << "Cutoff attr (rcutoff)                      :           " << rcutoff << "\n";
			paraInfo << "Cutoff rep (rd_cutoff)                     :           " << rd_cutoff << "\n";
			paraInfo << "Density (rho)                              :           " << rho << "\n";
			paraInfo << "Timestep (dt)                              :           " << dt << "\n";
			paraInfo << "Number of fluid particles (fluidCount)     :           " << fluidCount << "\n";
		
			paraInfo << "---------------------------" << "\n";
			paraInfo << "Run & Data saving" << "\n";
			paraInfo << "---------------------------" << "\n";
			paraInfo << "Total run time (stepMax)                   :           " << stepMax << "\n";
			paraInfo << "Data frequency (saveCount)                 :           " << saveCount << "\n";
			paraInfo << "Pressure Data frequency (psaveCount)       :           " << psaveCount << "\n";
			
			#if SPHERICAL_DROPLET 
				paraInfo << "---------------------------" << "\n";
				paraInfo << "Spherical Drop Parameters" << "\n";
				paraInfo << "---------------------------" << "\n";
				paraInfo << "Radius of the spherical droplet (sphDropRad)            :           " << sphDropRad << "\n";
			#elif CYLINDER_DROPLET 
				paraInfo << "---------------------------" << "\n";
				paraInfo << "Cylinder Parameters" << "\n";
				paraInfo << "---------------------------" << "\n";
				paraInfo << "Cylinder radius (cylRad)                   :           " << cylRad << "\n";
				paraInfo << "Cylinder Height (cylHeight)                :           " << cylHeight << "\n";
				paraInfo << "Cylinder center x-coord (cylCenterX)       :           " << cylCenterX << "\n";
				paraInfo << "Cylinder center y-coord (cylCenterY)       :           " << cylCenterY << "\n";
			#elif PLANAR_SLAB
				paraInfo << "---------------------------" << "\n";
				paraInfo << "Planar Slab Parameters" << "\n";
				paraInfo << "---------------------------" << "\n";
			#if !(WALL_ON)
				paraInfo << "Width of planar slab (slabWidth)           :           " << slabWidth << "\n";
			#endif
			#endif
			
			#if RANDOM_DISSIPATIVE
				paraInfo << "---------------------------------------------------" << "\n";
                paraInfo << "--------- Dynamic interaction parameters ----------" << "\n";
				paraInfo << "---------------------------------------------------" << "\n";
				paraInfo << "Set temperature (kbT)                      :           " << kBT << "\n";

                paraInfo << "noise_S1_S1                  :           " << sigma[0][0] << "\n";
                paraInfo << "noise_S1_L1                  :           " << sigma[0][1] << "\n";
                paraInfo << "noise_S1_L2                  :           " << sigma[0][2] << "\n";
                paraInfo << "noise_S1_S2                  :           " << sigma[0][3] << "\n";
                paraInfo << "noise_S1_S3                  :           " << sigma[0][4] << "\n";
                paraInfo << "noise_S1_L3                  :           " << sigma[0][5] << "\n\n";

                paraInfo << "noise_L1_L1                  :           " << sigma[1][1] << "\n";
                paraInfo << "noise_L1_L2                  :           " << sigma[1][2] << "\n";
                paraInfo << "noise_L1_S2                  :           " << sigma[1][3] << "\n";
                paraInfo << "noise_L1_S3                  :           " << sigma[1][4] << "\n";
                paraInfo << "noise_L1_L3                  :           " << sigma[1][5] << "\n\n";

                paraInfo << "noise_L2_L2                  :           " << sigma[2][2] << "\n";
                paraInfo << "noise_L2_S2                  :           " << sigma[2][3] << "\n";
                paraInfo << "noise_L2_S3                  :           " << sigma[2][4] << "\n";
                paraInfo << "noise_L2_L3                  :           " << sigma[2][5] << "\n\n";

                paraInfo << "noise_S2_S2                  :           " << sigma[3][3] << "\n";
                paraInfo << "noise_S2_S3                  :           " << sigma[3][4] << "\n";
                paraInfo << "noise_S2_L3                  :           " << sigma[3][5] << "\n\n";

                paraInfo << "noise_S3_S3                  :           " << sigma[4][4] << "\n";
                paraInfo << "noise_S3_L3                  :           " << sigma[4][5] << "\n\n";

                paraInfo << "noise_L3_L3                  :           " << sigma[5][5] << "\n\n";

                paraInfo << "friction_S1_S1               :           " << gamma[0][0] << "\n";
                paraInfo << "friction_S1_L1               :           " << gamma[0][1] << "\n";
                paraInfo << "friction_S1_L2               :           " << gamma[0][2] << "\n";
                paraInfo << "friction_S1_S2               :           " << gamma[0][3] << "\n";
                paraInfo << "friction_S1_S3               :           " << gamma[0][4] << "\n";
                paraInfo << "friction_S1_S3               :           " << gamma[0][5] << "\n\n";

                paraInfo << "friction_L1_L1               :           " << gamma[1][1] << "\n";
                paraInfo << "friction_L1_L2               :           " << gamma[1][2] << "\n";
                paraInfo << "friction_L1_S2               :           " << gamma[1][3] << "\n";
                paraInfo << "friction_L1_S3               :           " << gamma[1][4] << "\n";
                paraInfo << "friction_L1_L3               :           " << gamma[1][5] << "\n\n";

                paraInfo << "friction_L2_L2               :           " << gamma[2][2] << "\n";
                paraInfo << "friction_L2_S2               :           " << gamma[2][3] << "\n";
                paraInfo << "friction_L2_S3               :           " << gamma[2][4] << "\n";
                paraInfo << "friction_L2_L3               :           " << gamma[2][5] << "\n\n";

                paraInfo << "friction_S2_S2               :           " << gamma[3][3] << "\n";
                paraInfo << "friction_S2_S3               :           " << gamma[3][4] << "\n";
                paraInfo << "friction_S2_L3               :           " << gamma[3][5] << "\n\n";

                paraInfo << "friction_S3_S3               :           " << gamma[4][4] << "\n";
                paraInfo << "friction_S3_L3               :           " << gamma[4][5] << "\n\n";

                paraInfo << "friction_L3_L3               :           " << gamma[5][5] << "\n\n";
			#endif 
			
            paraInfo << "---------------------------------------------------" << "\n";
            paraInfo << "-------Thermo-dynamic interaction parameters-------" << "\n";
            paraInfo << "---------------------------------------------------" << "\n";
			paraInfo << "                        Aij" << "\n";
			paraInfo << "---------------------------------------------------" << "\n";

            paraInfo << "Aatt_S1_S1                    :           " << Aatt[0][0] << "\n";
            paraInfo << "Aatt_S1_L1                    :           " << Aatt[0][1] << "\n";
            paraInfo << "Aatt_S1_L2                    :           " << Aatt[0][2] << "\n";
            paraInfo << "Aatt_S1_S2                    :           " << Aatt[0][3] << "\n";
            paraInfo << "Aatt_S1_S3                    :           " << Aatt[0][4] << "\n";
            paraInfo << "Aatt_S1_L3                    :           " << Aatt[0][5] << "\n\n";

            paraInfo << "Aatt_L1_L1                    :           " << Aatt[1][1] << "\n";
            paraInfo << "Aatt_L1_L2                    :           " << Aatt[1][2] << "\n";
            paraInfo << "Aatt_L1_S2                    :           " << Aatt[1][3] << "\n";
            paraInfo << "Aatt_L1_S3                    :           " << Aatt[1][4] << "\n";
            paraInfo << "Aatt_L1_L3                    :           " << Aatt[1][5] << "\n\n";

            paraInfo << "Aatt_L2_L2                    :           " << Aatt[2][2] << "\n";
            paraInfo << "Aatt_L2_S2                    :           " << Aatt[2][3] << "\n";
            paraInfo << "Aatt_L2_S3                    :           " << Aatt[2][4] << "\n";
            paraInfo << "Aatt_L2_L3                    :           " << Aatt[2][5] << "\n\n";

            paraInfo << "Aatt_S2_S2                    :           " << Aatt[3][3] << "\n";
            paraInfo << "Aatt_S2_S3                    :           " << Aatt[3][4] << "\n";
            paraInfo << "Aatt_S2_L3                    :           " << Aatt[3][5] << "\n\n";

            paraInfo << "Aatt_S3_S3                    :           " << Aatt[4][4] << "\n";
            paraInfo << "Aatt_S3_L3                    :           " << Aatt[4][5] << "\n\n";

            paraInfo << "Aatt_L3_L3                    :           " << Aatt[5][5] << "\n\n";

            paraInfo << "---------------------------------------------------" << "\n";
			paraInfo << "                        Bij" << "\n";
			paraInfo << "---------------------------------------------------" << "\n";
            paraInfo << "Brep_S1_S1                    :           " << Brep[0][0] << "\n";
            paraInfo << "Brep_S1_L1                    :           " << Brep[0][1] << "\n";
            paraInfo << "Brep_S1_L2                    :           " << Brep[0][2] << "\n";
            paraInfo << "Brep_S1_S2                    :           " << Brep[0][3] << "\n";
            paraInfo << "Brep_S1_S3                    :           " << Brep[0][4] << "\n";
            paraInfo << "Brep_S1_L3                    :           " << Brep[0][5] << "\n\n";

            paraInfo << "Brep_L1_L1                    :           " << Brep[1][1] << "\n";
            paraInfo << "Brep_L1_L2                    :           " << Brep[1][2] << "\n";
            paraInfo << "Brep_L1_S2                    :           " << Brep[1][3] << "\n";
            paraInfo << "Brep_L1_S3                    :           " << Brep[1][4] << "\n";
            paraInfo << "Brep_L1_L3                    :           " << Brep[1][5] << "\n\n";

            paraInfo << "Brep_L2_L2                    :           " << Brep[2][2] << "\n";
            paraInfo << "Brep_L2_S2                    :           " << Brep[2][3] << "\n";
            paraInfo << "Brep_L2_S3                    :           " << Brep[2][4] << "\n";
            paraInfo << "Brep_L2_L3                    :           " << Brep[2][5] << "\n\n";

            paraInfo << "Brep_S2_S2                    :           " << Brep[3][3] << "\n";
            paraInfo << "Brep_S2_S3                    :           " << Brep[3][4] << "\n";
            paraInfo << "Brep_S2_L3                    :           " << Brep[3][5] << "\n\n";

            paraInfo << "Brep_S3_S3                    :           " << Brep[4][4] << "\n";
            paraInfo << "Brep_S3_L3                    :           " << Brep[4][5] << "\n\n";

            paraInfo << "Brep_L3_L3                    :           " << Brep[5][5] << "\n\n";

            paraInfo << "---------------------------------------------------" << "\n";
			paraInfo << "                        Wall parameters" << "\n";
			paraInfo << "---------------------------------------------------" << "\n";
            paraInfo << "Number of solid particles (solidCount)                     :           " << solidCount << "\n";
            paraInfo << "Wall density (initWallRho)                                 :           " << initWallRho << "\n";
            paraInfo << "Penetration tolerance (wallPenetration)                    :           " << wallPenetration << "\n";
            paraInfo << "Background Spring constant for wall ( kWallBckg )          :           " << kWallBckg << "\n";
            paraInfo << "Neighbour Spring constant for wall ( kWallNgbr1 )           :           " << kWallNgbr1 << "\n";
            paraInfo << "Neighbour Spring constant for wall ( kWallNgbr2 )           :           " << kWallNgbr2 << "\n";

			#if LEES_EDWARDS_BC
				paraInfo << "---------------------------" << "\n";
				paraInfo << "Lees-Edwards BC            " << "\n";
				paraInfo << "---------------------------" << "\n";
				paraInfo << "Homogeneous shear-rate (gammaDot)          :            " << gammaDot << "\n";
				paraInfo << "Strain-rate (strainRate)                   :            " << strainRate << "\n";
			#endif

			#if SACF
				paraInfo << "-------------------------------" << "\n";
				paraInfo << "Stress-Autocorrelation function" << "\n";
				paraInfo << "-------------------------------" << "\n";
				paraInfo << "Number of variables (nvars )               :            " << n_vars << "\n";
				paraInfo << "Levels of correlation (corLevels)          :            " << corLevels << "\n";
				paraInfo << "Blocks in a level (pCorr)                  :            " << pCorr << "\n";
				paraInfo << "Average length in a level (mCorr)          :            " << mCorr << "\n";
			#endif
			#if CAPILLARY_CYLINDER
				paraInfo << "-------------------------------" << "\n";
				paraInfo << "Capillary cylinder             " << "\n";
				paraInfo << "-------------------------------" << "\n";
				paraInfo << "Buffer length left of capillary (bufferLen):            " << bufferLen << "\n";	
				paraInfo << "Capillary tube length ( capLen )           :            " << capLen << "\n";
				paraInfo << "Capillary radius ( capRad )                :            " << capRad << "\n";
				paraInfo << "Wall width adj. to capillary (capWallWdth) :            " << capWallWdth << "\n";
				paraInfo << "Initial width of reservoir (resWdth)       :            " << resWdth << "\n";
			#elif CAPILLARY_SQUARE
				paraInfo << "-------------------------------" << "\n";
				paraInfo << "Capillary Square               " << "\n";
				paraInfo << "-------------------------------" << "\n";
				paraInfo << "Buffer length left of capillary (bufferLen):            " << bufferLen << "\n";	
				paraInfo << "Capillary tube length ( capLen )           :            " << capLen << "\n";
				paraInfo << "Inner-edge-length of capillary ( sqEdge )  :            " << sqEdge << "\n";
				paraInfo << "Wall width adj. to capillary (capWallWdth) :            " << capWallWdth << "\n";
				paraInfo << "Initial width of reservoir (resWdth)       :            " << resWdth << "\n";

				#if PISTON
					paraInfo << "-------------------------------" << "\n";
					paraInfo << "PISTON                         " << "\n";
					paraInfo << "-------------------------------" << "\n";
					paraInfo << "Applied pressure (appPressure)             :            " << appPressure << "\n";	
					paraInfo << "Piston force applied from time (pistonT0)  :            " << pistonT0 << "\n";
					paraInfo << "Time constant of piston force ( pistonW )  :            " << pistonW << "\n";
				#endif
			#endif
			paraInfo << "---------------------------" << "\n";
			paraInfo << "---------------------------" << "\n";
			paraInfo << "---------------------------" << "\n";


			/******************************* FLAGS ***************************************/
			flagList.open ( "./flagList.txt", std::ios::out );
			#if RANDOM_DISSIPATIVE           
			flagList << "RANDOM_DISSIPATIVE FLAG                :            " << "ON" << "\n";  
			#endif
			#if SPHERICAL_DROPLET                
			flagList << "SPHERICAL_DROPLET FLAG                 :            " << "ON" << "\n";  
			#endif
			#if SPHERICAL_CAP                    
			flagList << "SPHERICAL_CAP FLAG                     :            " << "ON" << "\n";  
			#endif
			#if CYLINDER_DROPLET                 
			flagList << "CYLINDER_DROPLET FLAG                  :            " << "ON" << "\n";  
			#endif
			#if PLANAR_SLAB                      
			flagList << "PLANAR_SLAB FLAG                       :            " << "ON" << "\n";  
			#endif
			#if CRYSTAL	                         
			flagList << "CRYSTAL FLAG                           :            " << "ON" << "\n";  
			#endif
			#if RESTART	                          
			flagList << "RESTART FLAG                           :            " << "ON" << "\n";  
			#endif

			// WALL flags
			#if WALL_ON                          
			flagList << "WALL_ON FLAG                           :            " << "ON" << "\n";  
			#if LOWER_WALL_ON                    
			flagList << "LOWER_WALL_ON FLAG                     :            " << "ON" << "\n";  
			#endif
			#if UPPER_WALL_ON                    
			flagList << "UPPER_WALL_ON FLAG                     :            " << "ON" << "\n";  
			#endif
			#if FCC_WALL                         
                flagList << "FCC_WALL FLAG                          :            " << "ON" << "\n";  
			#endif
			#if ROUGH_WALL                       
                flagList << "ROUGH_WALL FLAG                        :            " << "ON" << "\n";  
			#endif
			#endif

			// POISEUILLE flow
			#if BODY_FORCE                       
                flagList << "BODY_FORCE FLAG                        :            " << "ON" << "\n";  
			#endif

			// FILE_WRITE
            flagList << "STYLE_VMD FLAG                         :            " << "ON" << "\n";  

			// CORRELATION FUNCTIONS
			#if SACF                              
                flagList << "SACF FLAG                              :            " << "ON" << "\n";  
			#endif
			#if SACF_TEST                        
                flagList << "SACF_TEST FLAG                         :            " << "ON" << "\n";  
			#endif

			// LEES-EDWARDS BOUNDARY CONDITION
			#if LEES_EDWARDS_BC                  
                flagList << "LEES_EDWARDS_BC FLAG                   :            " << "ON" << "\n";  
			#endif

			// DENSITY CALCULATION
			#if DENS_EXACT			 
                flagList << "DENS_EXACT FLAG                        :            " << "ON" << "\n";  
			#endif
			
			// CAPILLARY IMBIBITION
			#if CAPILLARY_CYLINDER		 
                flagList << "CAPILLARY CYLINDER FLAG                :            " << "ON" << "\n";  
                #if PISTON
                    flagList << "PISTON FLAG                            :            " << "ON" << "\n";  
                #endif
			#endif
			#if CAPILLARY_SQUARE		 
                flagList << "CAPILLARY SQUARE FLAG                  :            " << "ON" << "\n";  
                #if PISTON
                    flagList << "PISTON FLAG                            :            " << "ON" << "\n";  
                #endif
			#endif

		}

		//--------------------------------------- Velocity, Momentum and Pressure file writing--------------------------------------//
		void fileWrite( std::ofstream& enStats, std::ofstream& eosStats, std::ofstream& momStats, std::ofstream& pTensStats, std::ofstream& colloidStats ){

			if ( step % saveCount == 0){
				simProg << step << " steps out of " << stepMax << " completed " << "\n";

			}

            #if HARD_SPHERES
            if( step % (saveCount/50) == 0 )
                    colloidStats << step << "\t\t" << colloid_com_pos << std::endl; 
            #endif

			//write output file in the .data format
			if (counter>=saveCount) {				

				// particle positions in ascii file format
				//asciiFileWritePosVel();
				binFileWritePosVel();

				// Energy
						#if LEES_EDWARDS_BC
				enStats 	<< std::setw(20) << std::setprecision(15) << pot_en << "\t" 
					        << std::setw(20) << std::setprecision(15) << kin_en << "\t" 
				        	<< std::setw(20) << std::setprecision(15) << tot_en << "\t"
				        	<< std::setw(20) << std::setprecision(15) << conservativePower << "\t"
				        	<< std::setw(20) << std::setprecision(15) << dissipativePower << "\t"
				        	<< std::setw(20) << std::setprecision(15) << randomPower << "\n";
						#else
							#if WALL_ON
								#if CAPILLARY_CYLINDER || CAPILLARY_SQUARE || HARD_SPHERES
									#if PISTON
                                    enStats     << std::setw(20) << std::setprecision(15) << pot_en << "\t" 
                                                << std::setw(20) << std::setprecision(15) << kin_en << "\t" 
                                                << std::setw(20) << std::setprecision(15) << tot_en << "\t" << "\n";
                                                // << std::setw(20) << std::setprecision(15) << forceOnPiston << "\n";
                                    #else 
                                       enStats     << std::setw(20) << std::setprecision(15) << pot_en << "\t" 
                                                    << std::setw(20) << std::setprecision(15) << kin_en << "\t" 
                                                    << std::setw(20) << std::setprecision(15) << tot_en << "\t" << "\n";
									#endif
								#endif
							#else
								enStats 	<< std::setw(20) << std::setprecision(15) << pot_en << "\t" 
										<< std::setw(20) << std::setprecision(15) << kin_en << "\t" 
										<< std::setw(20) << std::setprecision(15) << tot_en << "\n";
							#endif

						#endif

				// Density, Temperature, Average Pressure
				eosStats 	<< std::setw(20) << std::setprecision(15) << rho 	<< "\t" 
					        << std::setw(20) << std::setprecision(15) << temp	<< "\t" 
					        #if WALL_ON
					        << std::setw(20) << std::setprecision(15) << wallTemp	<< "\t" 
					        #endif
				        	<< std::setw(20) << std::setprecision(15) << pressure 	<< "\n";

				// Momentum
				momStats 	<< std::setw(20) << std::setprecision(15) << momX << "\t" 
				        	<< std::setw(20) << std::setprecision(15) << momY << "\t" 
				        	<< std::setw(20) << std::setprecision(15) << momZ << "\n";

					//reset the counter, write time to terminal
					counter = 0;
			}
		}

		//--------------------------------------- ascii file writing routine--------------------------------------//
		void asciiFileWritePosVel(){

                char filename[40];
                char filename1[40];
                //char filename2[40];

                // sprintf( filename, "./data/data1_%d.vtu", step);  
                sprintf( filename, "./data/pos%d.xyz", step);  
                sprintf( filename1, "./data/velocity%d.dat", step);  
                //sprintf( filename2, "./data/XYZ%d.xyz", step);  
                
                std::ofstream file(filename);
                std::ofstream file1(filename1);
                //std::ofstream file2(filename2);

                file << particles.size() << "\n";
                file << "#X Y Z co-ordinates" << "\n";

                file1 << particles.size() << "\n";
                file1 << "#vx vy vz" << "\n";

                //file2 << particles.size() << "\n";
                //file2 << "#X Y Z co-ordinates" << "\n";

                i = 0;
                while ( i < npart ){

                         if ( particles[i].type == 0 ){
                             file  << "O" << "\t" << std::setprecision(10) << particles[i].r_old << "\n";
                             file1 << "O" << "\t" << std::setprecision(10) << particles[i].v << "\n";
                         }
                        else if ( particles[i].type == 1 ){
                            file  << "H" << "\t" << std::setprecision(10) << particles[i].r_old  << "\t " << "\n";
                            file1 << "H" << "\t" << std::setprecision(10) << particles[i].v << "\n";
                        }
                        else if ( particles[i].type == 2 ){
                            file  << "C" << "\t" << std::setprecision(10) << particles[i].r_old << "\t " <<  "\n";
                            file1 << "C" << "\t" << std::setprecision(10) << particles[i].v << "\n";
                        }
                        else if ( particles[i].type == 3 ){
                            file  << "B" << "\t" << std::setprecision(10) << particles[i].r_old << "\n";
                            file1 << "B" << "\t" << std::setprecision(10) << particles[i].v << "\n";
                        }
                        else if ( particles[i].type == 4 ){
                            file  << "N" << "\t" << std::setprecision(10) << particles[i].r_old << "\n";
                            file1 << "N" << "\t" << std::setprecision(10) << particles[i].v << "\n";
                        }
                        else if ( particles[i].type == 5 ){
                            file  << "D" << "\t" << std::setprecision(10) << particles[i].r_old << "\n";
                            file1 << "D" << "\t" << std::setprecision(10) << particles[i].v << "\n";
                        }

                        i++;
                }

                file.close();
                file1.close();
		}
		//--------------------------------------- binary file writing routine--------------------------------------//
		void binFileWritePosVel(){

                char filename[40];
                sprintf( filename, "./data/posVel%d.bin", step);  
                std::ofstream writeBinary(filename);
                
                writeBinary.write( reinterpret_cast< const char * >( &npart ), sizeof( npart ) );
                for (Particle& p : particles){
                    // particle types
                    writeBinary.write( reinterpret_cast< const char * >( &p.type ), sizeof( p.type ) );

                    // particle positions - r(t + dt)
                    writeBinary.write( reinterpret_cast< const char * >( &p.r_old.X ), sizeof( p.r_old.X ) );
                    writeBinary.write( reinterpret_cast< const char * >( &p.r_old.Y ), sizeof( p.r_old.Y ) );
                    writeBinary.write( reinterpret_cast< const char * >( &p.r_old.Z ), sizeof( p.r_old.Z ) );

                    // mid-step velocities ( from Verlet ) - v( t - 0.5 * dt )
                    writeBinary.write( reinterpret_cast< const char * >( &p.v.X ), sizeof( p.v.X ) );
                    writeBinary.write( reinterpret_cast< const char * >( &p.v.Y ), sizeof( p.v.Y ) );
                    writeBinary.write( reinterpret_cast< const char * >( &p.v.Z ), sizeof( p.v.Z ) );
                }
                writeBinary.close();
		}
		//-------------------- Final velocity and positions ----------------------//
		void finalposvelWrite( std::ofstream& writeConfig ){

			writeConfig.write( reinterpret_cast< const char * >( &npart ), sizeof( npart ) );
            #if WALL_ON
                writeConfig.write( reinterpret_cast< const char * >( &bckgIdxStart ), sizeof( bckgIdxStart ) );
                writeConfig.write( reinterpret_cast< const char * >( &bckgIdxEnd ),   sizeof( bckgIdxEnd ) );
                
                #if SPRING_CONNECTED_SLD
                    writeConfig.write( reinterpret_cast< const char * >( &ngbrIdxStart ), sizeof( ngbrIdxStart ) );
                    writeConfig.write( reinterpret_cast< const char * >( &ngbrIdxEnd ),   sizeof( ngbrIdxEnd ) );
                #endif
            #endif
			writeConfig.write( reinterpret_cast< const char * >( &step ),  sizeof( step ) );
			writeConfig.write( reinterpret_cast< const char * >( &gen ),  sizeof( std::mt19937 ) );
			writeConfig.write( reinterpret_cast< const char * >( &normalDistribution ),  sizeof( std::normal_distribution<double> ) );

			for (Particle& p : particles){
                // particle types
				writeConfig.write( reinterpret_cast< const char * >( &p.type ), sizeof( p.type ) );

                // particle positions - r(t + dt)
				writeConfig.write( reinterpret_cast< const char * >( &p.r.X ), sizeof( p.r.X ) );
				writeConfig.write( reinterpret_cast< const char * >( &p.r.Y ), sizeof( p.r.Y ) );
				writeConfig.write( reinterpret_cast< const char * >( &p.r.Z ), sizeof( p.r.Z ) );

                // mid-step velocities ( from Verlet ) - v( t - 0.5 * dt )
				writeConfig.write( reinterpret_cast< const char * >( &p.w.X ), sizeof( p.w.X ) );
				writeConfig.write( reinterpret_cast< const char * >( &p.w.Y ), sizeof( p.w.Y ) );
				writeConfig.write( reinterpret_cast< const char * >( &p.w.Z ), sizeof( p.w.Z ) );

                // many body densities of the particles
				writeConfig.write( reinterpret_cast< const char * >( &p.dens ), sizeof( p.dens ) );

                // solid particles attached to the background by a spring
                #if WALL_ON 
                    writeConfig.write( reinterpret_cast< const char * >( &p.r0.X ), sizeof( p.r0.X ) );
                    writeConfig.write( reinterpret_cast< const char * >( &p.r0.Y ), sizeof( p.r0.Y ) );
                    writeConfig.write( reinterpret_cast< const char * >( &p.r0.Z ), sizeof( p.r0.Z ) );
                #endif
			}
		}

        //------------------------------ Create spring network : ENTIRE box ------------------------------//
        // creates springs between particles based on nearest neighbour considerations for the entire box //
        void springNetworkBox(){

           #if !(RESTART)
               totalBonds = 0;

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
                                    // simProg << "j1 "<<  mi[x] << " " << mi[y] << " " << mi[z] << " " << jj << " " << j << "\n";

                                    #include "createBondIndex.h"

                                } // jj

                                // particle j in neighbour cell to i
                                for ( m = 0 ; m < 13 ; m++ ){
                                    mj[x]	     = periodN[ mi[x] + dm[m][x] + 1 ][x];
                                    mj[y]	     = periodN[ mi[y] + dm[m][y] + 1 ][y];
                                    mj[z]	     = periodN[ mi[z] + dm[m][z] + 1 ][z];

                                    dR.X	     = periodR[ mi[x] + dm[m][x] + 1 ][x];
                                    dR.Y	     = periodR[ mi[y] + dm[m][y] + 1 ][y];
                                    dR.Z	     = periodR[ mi[z] + dm[m][z] + 1 ][z];

                                    for ( jj = 1 ; jj <= grid[mj[x]][mj[y]][mj[z]][0] ; ++jj ){
                                        j = grid[mj[x]][mj[y]][mj[z]][jj];
                                        // simProg << "j2 " << m << " " << mj[x] << " " << mj[y] << " " << mj[z] << " " << jj << " " << j << "\n";

                                            #include "createBondIndex.h"

                                    } // jj

                                } // m

                            } // ii

                #include "psfWrite.h"

                char fName[100];
                char fNameBin[100];

                sprintf(fName , "./restart/Neighbours.dat");
                sprintf(fNameBin , "./restart/Neighbours.bin");

                std::ofstream writeConfig ( fName, std::ios_base::out );
                std::ofstream writeConfigBin ( fNameBin, std::ios::binary |std::ios_base::out );

                //FORMAT : Col1: "Reference particle" Col2:"Bonded particle" Col3:"Eq bond Length" 
                writeConfig << totalBonds << "\n";
                writeConfigBin.write(reinterpret_cast< const char * >( &totalBonds ), sizeof(totalBonds));

                j = 0;
                i = solid_index[j];
                while (j < solidCount)
                {

                    for ( k=1; k<= particles[i].bondIndex[0]; ++k ){
                        // writing to ascii file
                        writeConfig << i << "\t" <<  particles[i].bondIndex[k] << "\t" << particles[i].eqBondLength[k] << "\n"; 

                        // writing to a binary file
                        writeConfigBin.write( reinterpret_cast< const char * >(&i), sizeof(i) );
                        writeConfigBin.write( reinterpret_cast< const char * >(&particles[i].bondIndex[k]), sizeof(particles[i].bondIndex[k]));
                        writeConfigBin.write( reinterpret_cast< const char * >(&particles[i].eqBondLength[k]), sizeof(particles[i].eqBondLength[k]));
                    }

                    j++;
                    i = solid_index[j];
                }

                writeConfig.close();
                writeConfigBin.close();

            #else 
                #include "readBondIndex.h"

                // write data of the bonds between particles and neighbours -- this is basically for
                // debugging
                char fName[100];
                sprintf(fName , "./data/Neighbours.dat");

                std::ofstream writeConfig ( fName, std::ios_base::out );
                writeConfig << totalBonds << "\n";
                writeConfig << "Reference particle" << "\t" << "Bonded particle" << "\t" << "Eq bond Length" << "\n";

                j = 0;
                i = solid_index[j];
                while ( j < solidCount )
                {

                    for ( k=1; k<= particles[i].bondIndex[0]; ++k ){
                        //writeConfig << " Particle " << particles[i].bondIndex[k] << " , eq bond length = " << particles[i].eqBondLength[k] << "\n"; 
                        writeConfig << i << "\t" <<  particles[i].bondIndex[k] << "\t" << particles[i].eqBondLength[k] << "\n"; 
                    }

                    j++;
                    i = solid_index[j];
                }

                writeConfig.close();
            #endif
        }
        //------------------------------ Mod function ------------------------------//
        int moduloAB( int A, int B){ 

            int result = std::fmod( A, B);

            result = ( result < 0 ) ? ( result + B ) : result; 

            return( result );
        }
};
#endif
