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
#define RANDOM_DISSIPATIVE		1
#define SPHERICAL_DROPLET		0
#define SPHERICAL_CAP			0
#define CYLINDER_DROPLET		0
#define PLANAR_SLAB			    0
#define CRYSTAL				    0
#define RESTART				    0

// WALL flags
#define WALL_ON				    1
#define LOWER_WALL_ON			0
#define UPPER_WALL_ON			0
#define ROUGH_WALL			    0
#define SPRING_CONNECTED_SLD    0
#define BCKGRND_CONNECTED_SLD   1

#define CAPILLARY_CYLINDER		0
#define CAPILLARY_SQUARE		0
#define PISTON				    0
#define CYLINDER_ARRAY          1
#define HARD_SPHERES            0
#define SLIM                    0
#define RANDOM_FIBRE_BUNDLE     0
#define READ_FROM_FILE          0

// FILE_WRITE
#define STYLE_VMD			    0

// CORRELATION FUNCTIONS
#define SACF				    0

// LEES-EDWARDS BOUNDARY CONDITION
#define LEES_EDWARDS_BC			0

// DENSITY CALCULATION
#define DENS_EXACT			    0    

// MULTI-VISCOSITY LIQUIDS
#define MULTI_VISCOSITY_LIQUIDS 0

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
			dof 			= dim*(npart - 1);				// total degrees of freedom (momentum only, no energy conservation)
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
                        //#include "psfWrite.h"
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
                    std::ofstream colloidStats   ("./data/colloid_box_crossing.dat", std::ios_base::app );
                #endif
			#endif // RESTART

			// write parameters and initial configuration
			vtkFileWritePosVel();
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

                /*
				if ( (step > gR_tStart) && (step % gR_tDelta == 0) ) {  
                                                                        grSample(0, fluidCount, 0);
                                                                        #if MULTI_VISCOSITY_LIQUIDS
                                                                            grSample(fluidType1_idxStart, fluidType1_idxEnd, 1); 
                                                                            grSample(fluidType2_idxStart, fluidType2_idxEnd, 2); 
                                                                        #endif
                                                                     }

                 */

				counter += 1;						// filewriting
				fileWrite(enStats, eosStats, momStats, pTensStats);
                
                #if HARD_SPHERES
					if ( step % saveCount == 0 )
                        // colloidStats << step << "\t\t" << colloid_com_pos << "\t\t" <<  colloid_boxCrossing << std::endl;
                        colloidStats << step << "\t\t" << colloid_com_pos << std::endl; 
                #endif

				resetVar();						// reset variables to zero
				
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


				// from lyophobic to lyophilic
				step += 1;						// increment time step

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
            
            //#include "inputFields.h"
			#include "paramIn.h"
            //#include "geometryIn.h"

			#if WALL_ON
				#include "defineWall.h"

				#if SPHERICAL_DROPLET
					#include "sphDropInit.h"
                #endif

				#if CYLINDER_DROPLET
					#include "cylDropInit.h"
                #endif

				#if CAPILLARY_CYLINDER
                    pCount = 0;
                    bckgIdxStart = pCount;
                    //#include "wettingLiquidInCapillaryTube.h" 
                    //#include "definePiston.h"
					#include "capillaryTube.h"
                    bckgIdxEnd = pCount - 1;

                    //#include "readPiston.h"
                    //#include "readCapillaryTube.h"
                    //bckgIdxEnd = pCount - 1;
                    #if PISTON
                        ngbrIdxStart = pCount;
                        #include "singleLayerWall.h"
                        ngbrIdxEnd = pCount;

                        pistonStartIndex = ngbrIdxStart;
                        pistonEndIndex = ngbrIdxEnd;
                        pistonParticles = ngbrIdxEnd - ngbrIdxStart + 1;
                        pistonForce = -appPressure * pistonArea ; 
                    #endif

                    //#include "cylindricalFluids.h"
					#include "reservoir.h"
                    //#include "nonWettingReservoir.h" 
                    //#include "separateReservoir.h"
                    //
					//#include "mixedReservoir.h"
					//#include "separateReservoir.h"
                    // SLIM
					//#include "reservoir.h"
                    //#include "SLIM_capillary.h"
                    //#include "definePiston.h"

                    // COLLOIDS
                    // #include "multipleColloids.h"
					// #include "capillaryTube.h"
                #endif

				#if PLANAR_SLAB
					#include "planarSlabInit.h" 
                    //#include "readPlanarSlabInit.h"
                #endif

				#if CAPILLARY_SQUARE
					//#include "squarecapillaryTube.h"
                    //#include "rectangularCapillaryTube.h"
                    #include "rectangularCapillaryTubeOpen.h"
                    //ngbrIdxStart = particles.size();              // no of particles present + 1 -- particles.size() takes care of that
                    //#include "singleLayerWall.h"
                    //ngbrIdxEnd = particles.size() - 1;
                    #if PISTON
                        pistonStartIndex = ngbrIdxStart;
                        pistonEndIndex = ngbrIdxEnd;
                        pistonParticles = ngbrIdxEnd - ngbrIdxStart + 1;
                        pistonForce = -appPressure * pistonArea ; 
                    #endif
					//#include "reservoir.h"
					#include "reservoirNew.h"
                #endif

                #if CYLINDER_ARRAY
                    //#include "cylinderArray.h"
                    //#include "ellipseArray.h"
                    #include "createSquareArray.h"
                    #include "planarSlab.h"
                #endif
                #if HARD_SPHERES
                    #include "sphericalColloids.h"
                #endif
                #if SLIM
                    //#include "definePiston.h"
                    //#include "SLIMLiquid.h"
                    //#include "SLIMSolid.h"
					#include "reservoir.h"
                    #include "SLIM_capillary.h"
                #endif
                #if RANDOM_FIBRE_BUNDLE
                    //#include "randomFibreBundleParallel.h"
                    #include "squashedFibre.h"
                    #include "readSphericalCap.h"
					//#include "sphericalCap.h"
                    //#include "definePiston.h"
					//#include "sphericalCap.h"
                #endif
                #if READ_FROM_FILE & !(CAPILLARY_CYLINDER)
                    pCount = 0;
                    //#include "readConfigFromFile.h"
                    #include "SLIM_capillary.h"
                    bckgIdxEnd = npart - 1;         // this is a temporary workaround
				#endif
				#if RESTART 
					#include "restartConfig.h"
				#endif
			#else
				#if SPHERICAL_DROPLET 
					#include "sphDropInit.h"
				#elif SPHERICAL_CAP
					#include "sphericalCap.h"
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
            gR_nCount.resize( gR_nElem );

			for (i=0; i < gR_nElem; ++i)
                gR_nCount[i].resize(3);

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

            //fluidEndIndex = 0;
           // solidEndIndex = 0;
            simProg << "***************************************************" << std::endl;
            simProg << "Counting total particles" << std::endl;

			for ( i = 0; i < particles.size(); ++i ){
                if ( particles[i].type == 1 || particles[i].type == 2 ){
                    fluid_index.push_back(i);
                    fluidCount++;
                }
				else if( particles[i].type == 0 || particles[i].type == 3 ){
					solid_index.push_back(i);
					solidCount++;
				}
			}

            /*
            if ( solidCount > 0 ){
                solidStartIndex = solid_index[0];
                solidEndIndex   = solid_index[solidCount-1];

            }

            if ( fluidCount > 0 ){
                fluidStartIndex = fluid_index[0];
                fluidEndIndex   = fluid_index[fluidCount-1];
            }

            i=0;
            simProg << " SOLID " << std::endl;
            while( i < solidCount ){
                simProg << " i = " << solid_index[i] << std::endl;

                i++;
            }

            i=0;
            simProg << " FLUID " << std::endl;
            while( i < fluidCount ){
                simProg << " i = " << fluid_index[i] << std::endl;

                i++;
            }
            
            simProg << fluidStartIndex << " : starting index of fluid particles " << std::endl;
            simProg << fluidEndIndex << " : ending index of fluid particles " << std::endl;
			simProg << fluidCount << " : fluid particles indexed \n" << std::endl;

            simProg << solidStartIndex << " : starting index of solid particles " << std::endl;
            simProg << solidEndIndex << " : ending index of solid particles " << std::endl;
			simProg << solidCount << " : solid particles indexed \n" << std::endl;
            */

            #if PISTON
                //pistonStartIndex = solidEndIndex - pistonParticles + 1;
                //pistonEndIndex = solidEndIndex;

                //simProg << pistonStartIndex << " : start index of piston particles" << std::endl; 
                //simProg << pistonEndIndex << " : end index of piston particles \n" << std::endl; 
            #endif

            simProg << fluidCount << " fluid particles indexed" << std::endl;
            simProg << solidCount << " solid particles indexed" << std::endl;
            simProg << fluidCount + solidCount << " total particles indexed" << std::endl;

			#if WALL_ON
                /*
				j = 0;
                i = solid_index[j];
                #if BCKGRND_CONNECTED_SLD
                    while ( j <  solidCount ) {
                            particles[i].r0 = particles[i].r;

                            j++;
                            i = solid_index[j];
                    }
                    simProg << "\n" << solidCount << " solid particles stuck to their initial positions" << std::endl;
                #endif
                #if SPRING_CONNECTED_SLD
                    while( j < solidCount ){
                        particles[i].bondIndex[0] = 0;

                        j++;
                        i = solid_index[j];
                    }
                    simProg << "\n" << solidCount << " solid particles bond indices set to 0" << std::endl;
                #endif
                */

                #if SPRING_CONNECTED_SLD
                simProg << "***************************************************" << std::endl;
                simProg << "Spring Connected Solid Structures" << std::endl;
				idx = 0;
                ngbrIdxParticles=0;
                i = solid_index[idx];

                // simProg << " i = "  << i << ", ngbrIdxStart= " << ngbrIdxStart << " , ngbrIdxEnd " << ngbrIdxEnd << std::endl;

                    while( idx < solidCount ){

                        if ( i >= ngbrIdxStart && i <= ngbrIdxEnd ){
                            particles[i].bondIndex[0] = 0;
                            ngbrIdxParticles++;
                        }

                        idx++;
                        i = solid_index[idx];
                    }
                    simProg << "\n" << ngbrIdxParticles << " solid particles from " << ngbrIdxStart << " and " << ngbrIdxEnd  << " have their bond indices set to 0" << std::endl;

                    #if RANDOM_FIBRE_BUNDLE
                        //#include "springConnectionsFibre.h"
                    #endif

                    /*
                    m = 0;
                    for (unsigned int k=1; k<= particles[m].bondIndex[0]; ++k )
                        simProg << " Particle " << particles[m].bondIndex[k] << " , eq bond length = " << particles[m].eqBondLength[k] << std::endl; 
                        */
                #endif
                #if BCKGRND_CONNECTED_SLD
                simProg << "***************************************************" << std::endl;
                simProg << "Background Connected Solid Structures" << std::endl;
				idx = 0;
                bckgIdxParticles=0;
                i = solid_index[idx];

                    #if READ_FROM_FILE
                        #include "readInitPosFromFile.h"
                    #elif RESTART
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
                    simProg << "\n" << bckgIdxParticles << " solid particles from " << bckgIdxStart << " and " << bckgIdxEnd  << " have their positions attached to their initial position" << std::endl;
                #endif
			#endif // WALL_ON
			
			// Remove excess velocity and set particle density to 0
            /*
			Vec3D velAvg={0.0, 0.0, 0.0};
			if ( fluidCount != 0 ){
                j = 0;
				i = fluid_index[j];
				while( j < fluidCount ){
                        velAvg 					+= particles[i].w/fluidCount;
                        particles[i].dens 	 	= 0.0;
                        particles[i].dens_new	= 0.0;
                        particles[i].fC.setZero();

                        j++;
                        i = fluid_index[j];
				}

                j = 0;
				i = fluid_index[j];
				while( k < fluidCount ){
					particles[i].w -= velAvg;

                    j++;
                    i = fluid_index[j];
				}

				simProg << "Excess average velocity from fluid particles removed" << std::endl;
				simProg << "Densities of the fluid particles set to 0" << std::endl;

                #if SPHERICAL_CAP || CAPILLARY_CYLINDER || CAPILLARY_SQUARE
                    k = 0;
                    i = fluid_index[k];
                    while( k < fluidCount ){
                        //if ( particles[i].type == 2 )
                        particles[i].w.Z += resCOMVel;

                        k++;
                        i = fluid_index[k];
                    }
                    simProg << "The fluid particles have been assigned a velocity of " << resCOMVel << " rc/dt" << std::endl;
                #endif
            }
			else{
				simProg << "No fluid particles intialized" << std::endl;
			}
            */

			#if WALL_ON
                    /*
				velAvg={0.0, 0.0, 0.0};
				// Remove excess velocity and set particle density to 0
                k = 0;
                i = solid_index[k];
				while ( k < solidCount ){
					velAvg 					+= particles[i].w/solidCount;
					particles[i].dens 	 	= 0.0;
					particles[i].dens_new	 = 0.0;

					k++;
                    i = solid_index[k];
				}

                k = 0;
                i = solid_index[k];
				while ( k < solidCount ){
					particles[i].w.X -= velAvg.X;
					particles[i].w.Y -= velAvg.Y;
					particles[i].w.Z -= velAvg.Z;

                    particles[i].rUnfolded.X = particles[i].r.X;
                    particles[i].rUnfolded.Y = particles[i].r.Y;
                    particles[i].rUnfolded.Z = particles[i].r.Z;

					k++;
                    i = solid_index[k];
				}
				
				simProg << "Excess average velocity from solid particles removed" << std::endl;
				simProg << "Densities of the solid particles set to 0" << std::endl;
                */
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
            
            // defining the matrix for Brep and Aatt
            simProg << " ********************************************************************* " << std::endl;
            simProg << " Defining the matrix of repulsion parameters and attraction parameters " << std::endl;

            Brep[0][0] = repParam;  Aatt[0][0] = Ass;
            Brep[0][1] = repParam;  Aatt[0][1] = Asl1;
            Brep[0][2] = repParam;  Aatt[0][2] = Asl2;
            Brep[0][3] = repParam;  Aatt[0][3] = Ass;

            Brep[1][0] = repParam;  Aatt[1][0] = Aatt[0][1];
            Brep[1][1] = repParam;  Aatt[1][1] = All1;
            Brep[1][2] = repParam;  Aatt[1][2] = All12;
            Brep[1][3] = repParam;  Aatt[1][3] = Asl1;     // Interaction of liquid 1 with both 0 and 3 is same

            Brep[2][0] = repParam;  Aatt[2][0] = Aatt[0][2];    
            Brep[2][1] = repParam;  Aatt[2][1] = Aatt[1][2];
            Brep[2][2] = repParam2; Aatt[2][2] = All2;
            Brep[2][3] = repParam;  Aatt[2][3] = Asl2;   // Interaction of liquid 2 with both 0 and 3 is same

            Brep[3][0] = repParam;  Aatt[3][0] = Ass;
            Brep[3][1] = repParam;  Aatt[3][1] = Asl1;
            Brep[3][2] = repParam;  Aatt[3][2] = Asl2;
            Brep[3][3] = repParam;  Aatt[3][3] = Ass;

            simProg << " Finished defining the matrix of repulsion parameters and attraction parameters " << std::endl;
            simProg << " ********************************************************************* " << std::endl;

			// determine the number of types of particles present
			#if RANDOM_DISSIPATIVE
                simProg << " ********************************************************************* " << std::endl;
                simProg << " Defining the matrix of noise and friction parameters " << std::endl;

                sigma.resize( 4 );
                gamma.resize( 4 );

                for ( i=0; i < 4; ++i ){
                    sigma[i].resize( 4 );
                    gamma[i].resize( 4 );
                }

                // defining the elements of the sigma and gamma array
               sigma[0][0] 	= noise;						// S1-S1 
               sigma[0][1] 	= noise;                        // S1-L1
               sigma[0][2] 	= noise;                        // S1-L2
               sigma[0][3] 	= noise;                        // S1-S2

               sigma[1][0] 	= sigma[0][1];                  // L1-S1 
               sigma[1][1] 	= noise;       					// L1-L1    
               sigma[1][2] 	= noise12;                      // L1-L2
   	 		   sigma[1][3] 	= sigma[0][3];                  // L1-S2

               sigma[2][0] 	= sigma[0][2];                  // L2-S1
               sigma[2][1] 	= sigma[1][2];                  // L2-L1 
               sigma[2][2] 	= noise2;        			    // L2-L2		
               sigma[2][3] 	= sigma[2][0];        			// L2-S2		

               sigma[3][0] 	= noise;						// S2-S1 
               sigma[3][1] 	= noise;                        // S2-L1
               sigma[3][2] 	= noise;                        // S2-L2
               sigma[3][3] 	= noise;                        // S2-S2

   			   gamma[0][0] 	= friction;						// S1-S1 
               gamma[0][1] 	= friction;                     // S1-L1
               gamma[0][2] 	= friction;                     // S1-L2
               gamma[0][3] 	= friction;                     // S1-S2

               gamma[1][0] 	= gamma[0][1];                  // L1-S1
               gamma[1][1] 	= friction;       		        // L1-L1     
               gamma[1][2] 	= friction12;                   // L1-L2     
               gamma[1][3] 	= gamma[1][0];                  // L1-S2     

               gamma[2][0] 	= gamma[0][2];                  // L2-S1
               gamma[2][1] 	= gamma[1][2];                  // L2-L1
               gamma[2][2] 	= friction2;        	        // L2-L2	
               gamma[2][3] 	= gamma[2][0];        	        // L2-S2	

               gamma[3][0] 	= friction;						// S2-S1 
               gamma[3][1] 	= friction;                     // S2-L1
               gamma[3][2] 	= friction;                     // S2-L2
               gamma[3][3] 	= friction;                     // S2-S2

               simProg << " ********************************************************************* " << std::endl;
               simProg << " Finished defining the matrix of noise and friction parameters " << std::endl;
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
					simProg << "*** position: " << particles[i].r << std::endl;
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

			#if WALL_ON
                /*
                repParam_Inst = 0. + repParam * ( 0.5 * ( 1.0 + tanh( ( step - 10000. ) / 10000. ) ) );	

                Brep[0][1] = repParam_Inst;     Brep[1][0] = repParam_Inst;
                Brep[0][2] = repParam_Inst;     Brep[2][0] = repParam_Inst;

                Brep[3][1] = repParam_Inst;     Brep[1][3] = repParam_Inst;
                Brep[3][2] = repParam_Inst;     Brep[2][3] = repParam_Inst;
                */
			#endif

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
                                particles[j].dR = dR;
                                
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
                                        particles[j].dR = dR;
                                        
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
                                        particles[j].dR = dR;

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
                                    particles[j].dR = dR;

                                    #include "pairforce.h"

								} // jj
							} // m
							#endif

						} // ii

		}

		//--------------------------------------- Integrate Equations of motion --------------------------------------//
		void integrateEOM(){

			// #include "fluidParticles.h"

			// #include "solidParticles.h"

            #if SPRING_CONNECTED_SLD && BCKGRND_CONNECTED_SLD
                #include "springNetworkForceNew.h"  
            #elif SPRING_CONNECTED_SLD && !(BCKGRND_CONNECTED_SLD)
                #include "springNetworkForceOnly.h"  
            #elif !(SPRING_CONNECTED_SLD) && BCKGRND_CONNECTED_SLD
                #include "backgroundForceOnly.h"  
            #endif

          /*
          #if CAPILLARY_SQUARE
              #include "penetrationIntoSquare.h"  // background repulsive potential
          #endif
          */

          /*
          #if PISTON
            //if ( ( step % saveCount == 0 )  && ( step > pistonT0 )  ){
            if ( step % saveCount == 0 ){

                forceOnPiston /= saveCount;
            
                forceOnPistonPerParticle = ( pistonForce - 0.01*( forceOnPiston - pistonForce ) ) / pistonParticles;

                forceOnPiston = 0.;
            }
          #endif   
          */

          i = 0;
          while ( i < npart ){

                // calculate total center of mass momentum
                totCOM += particles[i].w; 

                // store velocity (mid-step)
                particles[i].r_old = particles[i].r;        // position at t: r(t)
                particles[i].w_old = particles[i].w;        // velocity at t-dt/2 : v(t - dt/2)
                   
                #if PISTON
                if ( i >= pistonStartIndex && i <= pistonEndIndex )
                    particles[i].fext.Z = forceOnPistonPerParticle;     // adding the force deficit per particle
                #endif

                /*
                #if CAPILLARY_CYLINDER 
                    // WARNING : Just implementing this for a single experiment
                    if ( i >= fluid_index[0] && i <= fluid_index[fluidCount-1] ){
                        particles[i].fext.Z = 0.1;
                    }
                #endif
                */

                // update velocities (mid-step)
                particles[i].w += ( particles[i].fC       + 
                                    particles[i].fD       + 
                                    particles[i].fR       + 
                                    particles[i].fHarmonic  + 
                                    particles[i].fext )*( dt/particles[i].m );      // evaluating velocity at t+dt/2 : v(t+dt/2)

                // update position (integral time step) using the velocities (mid-step)
                particles[i].r += particles[i].w*dt;                                // evaluating position at t+dt: r(t+dt)	

                // implement periodic boundary condition 
                // #include "pbcNew.h"
                #include "pbcNewReflecting.h"
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
                    if ( particles[i].type == 0 || particles[i].type == 3  ){
                        particles[i].rUnfolded += particles[i].w * dt;
                        colloid_com_pos += particles[i].rUnfolded;
                    }
                #endif

                #if PISTON
                    if ( i >= pistonStartIndex && i <= pistonEndIndex ){
                        /*forceOnPiston += particles[i].fC.Z;*/				                    // z-component of force on the piston due to fluid, the conservative force cancels out because internal forces
                        forceOnPistonInst += particles[i].fC.Z + particles[i].fR.Z + particles[i].fD.Z;	// z-component of force on the piston due to fluid, the conservative force cancels out because internal forces
                    }

                    /*
                    if ( i >= upperPistonIdxStart && i <= upperPistonIdxEnd ){
                        upperPistonCOMZ += particles[i].r.Z;
                    }
                    else if ( i >= lowerPistonIdxStart && i <= lowerPistonIdxEnd ){
                        lowerPistonCOMZ += particles[i].r.Z;
                    }
                    */
                #endif												

                /* to be sorted out : calculate only for fluid particles */
                #include "pNonIdealKinCalc.h"
	
                // update count for fluid particles
                i++;
          }

            #if PISTON

                /*
                std::cout << pistonT0 << ", " 
                          << avgWindow << ", " 
                          << pistonW << ", " 
                          << expFactor << ", "
                          << delOverTau << ", " 
                          << pistonForce << ", "
                          << pistonParticles << std::endl;*/

                if ( step <= pistonT0 - avgWindow ){
                    //forceOnPistonPerParticle = pistonForce / pistonParticles;
                    forceOnPistonPerParticle = 0.;
                }
                else if ( step > pistonT0 - avgWindow &&  step < pistonT0 ){

                    forceOnPistonPerParticle = pistonForce / pistonParticles;
                    // std::cout << step << "\t\t\t" << forceOnPistonInst << "\t\t\t" << exp( -( pistonT0 - step ) / pistonW ) << std::endl;
                    forceOnPistonCumulative += forceOnPistonInst * exp( -( pistonT0 - step ) / pistonW );
                }
                else if ( step == pistonT0 ) {
                    forceOnPistonPerParticle = pistonForce / pistonParticles;

                    // std::cout << step << "\t\t\t" << forceOnPistonInst << "\t\t\t" << exp( -( pistonT0 - step ) / pistonW ) << std::endl;
                    forceOnPistonCumulative += forceOnPistonInst;       // basically, the exponential factor becomes one at step = tStart
                    fOld = forceOnPistonCumulative/pistonW; 
                }           // pistonW is the normalization factor
                else if ( step > pistonT0 ) {
                    fNew = expFactor * fOld  +  delOverTau * forceOnPistonInst;   // F(t) = expFactor * F(t-1) + delOverTau * f(t), F(4001) = expFactor * F(4000) + delOvertau * f(4001) 
                    // fOld = fNew;        // swap values, for example : Fold = F(4001). 

                    std::cout << forceOnPistonInst << "\t\t\t" << fNew << std::endl;
                    // forceOnPistonPerParticle = -fNew / pistonParticles; 
                    forceOnPistonPerParticle = pistonForce / pistonParticles;
                }

                forceOnPistonInst = 0.;
            #endif

            //std::cout << step << "\t\t\t" << (forceOnPistonPerParticle*pistonParticles)/pistonArea << std::endl;
            // std::cout << step << "\t\t\t" << fNew << "\t\t\t" << (forceOnPistonPerParticle*pistonParticles)/pistonArea << std::endl;


			// calculation of  pressure tensor 
			#include "pTensCalc.h"

            #if HARD_SPHERES
                // #include "colloidBoxCrossing.h"
                colloid_com_pos /= solidCount;
            #endif
            /*
            #if PISTON
                #include "movePiston.h"
                // std::cout << "upper piston center of mass = " << upperPistonCOMZ << " and lower piston center of mass = " << lowerPistonCOMZ << " pistonParticles = " << pistonParticles  << std::endl;
                upperPistonCOMZ /= pistonParticles;
                lowerPistonCOMZ /= pistonParticles;
                if ( upperPistonCOMZ - lowerPistonCOMZ >= finalHeight ){
                    #include "movePiston.h"
                }
            #endif 
            */

			// calculate auto-correlation
			#if SACF
				if ( step > 5e4 ){
                    #if HARD_SPHERES 
                        #include "colloid_VACF.h"
                    #else
					    #include "ACF.h"
                    #endif
				}
			#endif

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
            #if PISTON
                //forceOnPiston = 0.;
            #endif

            // std::cout << step << ", " << totCOM/npart << std::endl;

            totCOM.X      = 0.;
            totCOM.Y      = 0.;
            totCOM.Z      = 0.;

			#if LEES_EDWARDS_BC
                dissipativeWork = 0.0;
                randomWork	= 0.0;
			#endif

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

            /*
            #if CYLINDER_ARRAY || LOWER_WALL_ON
                    if ( step % 1000 == 0 && residual > 0.)
                        simProg << "droplet Zcom = " << droplet_Zcom << ", capRad = " << capRad << ", zind_max = " << zind_max << " & residual= " << residual << " at time " << step << std::endl;

                    droplet_Zcom = droplet_ZcomNew / fluidCount;
                    droplet_ZcomNew = 0.;

                    residual = droplet_Zcom - capRad - zind_max;
            #endif
            */

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

            #if HARD_SPHERES
                pBondInteractions_temp[0][0] = 0.;
                pBondInteractions_temp[0][1] = 0.;
                pBondInteractions_temp[0][2] = 0.;

                pBondInteractions_temp[1][0] = 0.;
                pBondInteractions_temp[1][1] = 0.;
                pBondInteractions_temp[1][2] = 0.;

                pBondInteractions_temp[2][0] = 0.;
                pBondInteractions_temp[2][1] = 0.;
                pBondInteractions_temp[2][2] = 0.;
            #endif

            /*
            if ( step == 20000 ){

                noise = orig_noise;
                noise2 = orig_noise2;
                noise12 = orig_noise12;

                friction   = pow( noise, 2.0 )/( 2.0 * kBT ); 	    // DPD dissipative force parameter
                noise	  *= sqrtTwelve * inv_sqrt_dt;	    // Rescale sigma - sqrt(12) and inv_sqrt_dt

                friction2	= pow( noise2, 2.0 )/( 2.0 * kBT );     // DPD dissipative force parameter
                noise2 	  *= sqrtTwelve * inv_sqrt_dt;    // Rescale sigma - sqrt(12) and inv_sqrt_dt

                friction12	= pow( noise12, 2.0 )/( 2.0 * kBT );     // DPD dissipative force parameter
                noise12   *= sqrtTwelve * inv_sqrt_dt;    // Rescale sigma - sqrt(12) and inv_sqrt_dt

                simProg  << "noise and friction parameters are changed to " << std::endl;
                simProg  << "friction = " << friction << ", \n "
                         << "friction2 = " << friction2 << ", \n" 
                         << "noise = " << noise << ", \n" 
                         << "noise2 = " << noise2 << std::endl;

                sigma[0][0] 	= noise;						// S1-S1 
                sigma[0][1] 	= noise;                        // S1-L1
                sigma[0][2] 	= noise;                        // S1-L2
                sigma[0][3] 	= noise;                        // S1-S2

                sigma[1][0] 	= sigma[0][1];                  // L1-S1 
                sigma[1][1] 	= noise;       					// L1-L1    
                sigma[1][2] 	= noise12;                      // L1-L2
                sigma[1][3] 	= sigma[0][3];                  // L1-S2

                sigma[2][0] 	= sigma[0][2];                  // L2-S1
                sigma[2][1] 	= sigma[1][2];                  // L2-L1 
                sigma[2][2] 	= noise2;        			    // L2-L2		
                sigma[2][3] 	= sigma[2][0];        			// L2-S2		

                sigma[3][0] 	= noise;						// S2-S1 
                sigma[3][1] 	= noise;                        // S2-L1
                sigma[3][2] 	= noise;                        // S2-L2
                sigma[3][3] 	= noise;                        // S2-S2

                gamma[0][0] 	= friction;						// S1-S1 
                gamma[0][1] 	= friction;                     // S1-L1
                gamma[0][2] 	= friction;                     // S1-L2
                gamma[0][3] 	= friction;                     // S1-S2

                gamma[1][0] 	= gamma[0][1];                  // L1-S1
                gamma[1][1] 	= friction;       		        // L1-L1     
                gamma[1][2] 	= friction12;                   // L1-L2     
                gamma[1][3] 	= gamma[1][0];                  // L1-S2     

                gamma[2][0] 	= gamma[0][2];                  // L2-S1
                gamma[2][1] 	= gamma[1][2];                  // L2-L1
                gamma[2][2] 	= friction2;        	        // L2-L2	
                gamma[2][3] 	= gamma[2][0];        	        // L2-S2	

                gamma[3][0] 	= friction;						// S2-S1 
                gamma[3][1] 	= friction;                     // S2-L1
                gamma[3][2] 	= friction;                     // S2-L2
                gamma[3][3] 	= friction;                     // S2-S2
            }
            */
		}

		//--------------------------------------- g(r) sampling --------------------------------------//
		void grSample(unsigned int idxStart, unsigned int idxEnd, unsigned int idx){    // idx specifies the gR is between AA, AB, BB species

			//loop over all contacts p=1..N-1, q=p+1..N to evaluate forces
             for( i=idxStart; i <= idxEnd-1; ++i ){
                 for ( j=i+1; j<= idxEnd; ++j ){
					Rij     = particles[i].r - particles[j].r;	

                    // nearest image distance
                    Rij.X   = Rij.X - boxEdge[x] * round( Rij.X / boxEdge[x] );		// rij shear-flow correction : dR
                    Rij.Y   = Rij.Y - boxEdge[y] * round( Rij.Y / boxEdge[y] );
                    Rij.Z   = Rij.Z - boxEdge[z] * round( Rij.Z / boxEdge[z] );

					r2      = Rij.getLengthSquared();
					dist    = std::sqrt(r2);

                    ig = ceil( dist / gR_radDelta ) - 1;
                    if ( (ig < 0) || (ig >= gR_nElem) ) {simProg << " gRcalc(): out of bounds for dist = " << dist << " and ig = " << ig  << std::endl; abort();} 
                    gR_nCount[ig][idx] += 2.0;	
					j++;
				}
				i++;
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

				grWrite << ri << "\t" << ro << "\t" << rad << "\t" << gR_nCount[i][0] << "\t" << gR_nCount[i][1] << "\t" << gR_nCount[i][2] << "\t" << npart << "\t" << gR_tSamples << "\t" << rho << std::endl;
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
				paraInfo << "Radius of the spherical droplet (sphDropRad)            :           " << sphDropRad << std::endl;
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
				paraInfo << "Rescaled Noise(sqrt(12)*sigma*inv_sqrt_dt) :           " << noise << std::endl;
				paraInfo << "Actual Noise level (sigma)                 :           " << noise/  ( std::sqrt(12.) * inv_sqrt_dt ) << std::endl;
				paraInfo << "Friction parameter (gamma)                 :           " << friction << std::endl;
                paraInfo << "Liquid2 rescaled noise level (sigma2)      :           " << sigma[2][2] << std::endl;
                paraInfo << "Liquid2 friction parameter (gamma2)        :           " << gamma[2][2] << std::endl;
                paraInfo << "L1-L2 rescaled noise level (sigma2)        :           " << sigma[1][2] << std::endl;
                paraInfo << "L1-L2 friction parameter (gamma2)          :           " << gamma[1][2] << std::endl;
			#endif 
			
			#if WALL_ON
                paraInfo << "---------------------------" << std::endl;
                paraInfo << "Wall parameters      " << std::endl;
                paraInfo << "---------------------------" << std::endl;
                paraInfo << "Number of solid particles (solidCount)     :           " << solidCount << std::endl;
                paraInfo << "Wall density (initWallRho)                 :           " << initWallRho << std::endl;
                paraInfo << "Solid-Solid Attraction Strength   (Ass)    :           " << Aatt[0][0] << std::endl;
                paraInfo << "Solid-Solid Repulsion  Strength   (Bss)    :           " << Brep[0][0] << std::endl;
                paraInfo << "Solid-Liquid Attraction Strength   (asl)   :           " << Aatt[0][1] << std::endl;
                paraInfo << "Solid-Liquid2 Attraction Strength   (asl)  :           " << Aatt[0][2] << std::endl;
                paraInfo << "Liquid2-Liquid2 Attraction Strength (asl)  :           " << Aatt[2][2] << std::endl;
                paraInfo << "Solid-Liquid Repulsion  Strength   (Bsl)   :           " << Brep[0][1] << std::endl;
                paraInfo << "Soft Repulsive force Strength   (Brep)     :           " << Brep[0][0] << std::endl;
                paraInfo << "Penetration tolerance (wallPenetration)    :           " << wallPenetration << std::endl;
                paraInfo << "Background Spring constant for wall ( kWall )         :           " << kWallBckg << std::endl;
                paraInfo << "Neighbour Spring constant for wall ( kWall )         :           " << kWallNgbr << std::endl;
			#endif // WALL_ON
			
			paraInfo << "---------------------------" << std::endl;
			paraInfo << "Conservative Force         " << std::endl;
			paraInfo << "---------------------------" << std::endl;
			paraInfo << "Liquid-Liquid Attraction Strength (All)    :           " << Aatt[1][1] << std::endl;
			paraInfo << "Liquid-Liquid Repulsion Strength (Bll)     :           " << Brep[1][1] << std::endl;

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
			#if CAPILLARY_CYLINDER
				paraInfo << "-------------------------------" << std::endl;
				paraInfo << "Capillary cylinder             " << std::endl;
				paraInfo << "-------------------------------" << std::endl;
				paraInfo << "Buffer length left of capillary (bufferLen):            " << bufferLen << std::endl;	
				paraInfo << "Capillary tube length ( capLen )           :            " << capLen << std::endl;
				paraInfo << "Capillary radius ( capRad )                :            " << capRad << std::endl;
				paraInfo << "Wall width adj. to capillary (capWallWdth) :            " << capWallWdth << std::endl;
				paraInfo << "Initial width of reservoir (resWdth)       :            " << resWdth << std::endl;
			#elif CAPILLARY_SQUARE
				paraInfo << "-------------------------------" << std::endl;
				paraInfo << "Capillary Square               " << std::endl;
				paraInfo << "-------------------------------" << std::endl;
				paraInfo << "Buffer length left of capillary (bufferLen):            " << bufferLen << std::endl;	
				paraInfo << "Capillary tube length ( capLen )           :            " << capLen << std::endl;
				paraInfo << "Inner-edge-length of capillary ( sqEdge )  :            " << sqEdge << std::endl;
				paraInfo << "Wall width adj. to capillary (capWallWdth) :            " << capWallWdth << std::endl;
				paraInfo << "Initial width of reservoir (resWdth)       :            " << resWdth << std::endl;

				#if PISTON
					paraInfo << "-------------------------------" << std::endl;
					paraInfo << "PISTON                         " << std::endl;
					paraInfo << "-------------------------------" << std::endl;
					paraInfo << "Applied pressure (appPressure)             :            " << appPressure << std::endl;	
					paraInfo << "Piston force applied from time (pistonT0)  :            " << pistonT0 << std::endl;
					paraInfo << "Time constant of piston force ( pistonW )  :            " << pistonW << std::endl;
				#endif
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
            flagList << "STYLE_VMD FLAG                         :            " << "ON" << std::endl;  

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
			#if CAPILLARY_CYLINDER		 
                flagList << "CAPILLARY CYLINDER FLAG                :            " << "ON" << std::endl;  
                #if PISTON
                    flagList << "PISTON FLAG                            :            " << "ON" << std::endl;  
                #endif
			#endif
			#if CAPILLARY_SQUARE		 
                flagList << "CAPILLARY SQUARE FLAG                  :            " << "ON" << std::endl;  
                #if PISTON
                    flagList << "PISTON FLAG                            :            " << "ON" << std::endl;  
                #endif
			#endif

		}

		//--------------------------------------- Velocity, Momentum and Pressure file writing--------------------------------------//
		void fileWrite( std::ofstream& enStats, std::ofstream& eosStats, std::ofstream& momStats, std::ofstream& pTensStats ){

			if ( step % saveCount == 0){
				simProg << step << " steps out of " << stepMax << " completed " << std::endl;
			}

			// separate module for pressure -- requires better averaging
			if ( pcounter >= psaveCount ) {

                // std::cout << "pCounter = " << pcounter << ", at step = " << step << std::endl;
				
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
                            #if HARD_SPHERES
                                <<  pRandom[2][2]                << " "

                                <<  pBondInteractions[0][0]                << " " 
                                <<  pBondInteractions[0][1]                << " " 
                                <<  pBondInteractions[0][2]                << " "

                                <<  pBondInteractions[1][0]                << " "
                                <<  pBondInteractions[1][1]                << " "
                                <<  pBondInteractions[1][2]                << " "

                                <<  pBondInteractions[2][0]                << " "
                                <<  pBondInteractions[2][1]                << " "
                                <<  pBondInteractions[2][2]                << std::endl;

                            #else
                                <<  pRandom[2][2]                << std::endl;
                            #endif
						
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
							#if WALL_ON
								#if CAPILLARY_CYLINDER || CAPILLARY_SQUARE || HARD_SPHERES
									#if PISTON
                                    enStats     << std::setw(20) << std::setprecision(15) << pot_en << "\t" 
                                                << std::setw(20) << std::setprecision(15) << kin_en << "\t" 
                                                << std::setw(20) << std::setprecision(15) << tot_en << "\t" << std::endl;
                                                // << std::setw(20) << std::setprecision(15) << forceOnPiston << std::endl;
                                    #else 
                                       enStats     << std::setw(20) << std::setprecision(15) << pot_en << "\t" 
                                                    << std::setw(20) << std::setprecision(15) << kin_en << "\t" 
                                                    << std::setw(20) << std::setprecision(15) << tot_en << "\t" << std::endl;
									#endif
								#endif
							#else
								enStats 	<< std::setw(20) << std::setprecision(15) << pot_en << "\t" 
										<< std::setw(20) << std::setprecision(15) << kin_en << "\t" 
										<< std::setw(20) << std::setprecision(15) << tot_en << std::endl;
							#endif

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

                char filename[40];
                char filename1[40];
                char filename2[40];

                // sprintf( filename, "./data/data1_%d.vtu", step);  
                sprintf( filename, "./data/pos%d.xyz", step);  
                sprintf( filename1, "./data/velocity%d.dat", step);  
                sprintf( filename2, "./data/XYZ%d.xyz", step);  
                
                std::ofstream file(filename);
                std::ofstream file1(filename1);
                std::ofstream file2(filename2);

                file << particles.size() << std::endl;
                file << "#X Y Z co-ordinates" << std::endl;

                file1 << particles.size() << std::endl;
                file1 << "#vx vy vz" << std::endl;

                file2 << particles.size() << std::endl;
                file2 << "#X Y Z co-ordinates" << std::endl;

                i = 0;
                while ( i < npart ){

                         if ( particles[i].type == 0 ){
                             file  << "O" << "\t" << std::setprecision(10) << particles[i].r_old << std::endl;
                             file1 << "O" << "\t" << std::setprecision(10) << particles[i].v << std::endl;
                             file2 << "O" << "\t" << std::setprecision(10) << particles[i].r << std::endl;
                         }
                        else if ( particles[i].type == 1 ){
                            file  << "H" << "\t" << std::setprecision(10) << particles[i].r_old  << "\t " << std::endl;
                            file1 << "H" << "\t" << std::setprecision(10) << particles[i].v << std::endl;
                            file2 << "H" << "\t" << std::setprecision(10) << particles[i].r << std::endl;
                        }
                        else if ( particles[i].type == 2 ){
                            file  << "C" << "\t" << std::setprecision(10) << particles[i].r_old << "\t " <<  std::endl;
                            file1 << "C" << "\t" << std::setprecision(10) << particles[i].v << std::endl;
                            file2 << "C" << "\t" << std::setprecision(10) << particles[i].r << std::endl;
                        }
                        else if ( particles[i].type == 3 ){
                            file  << "B" << "\t" << std::setprecision(10) << particles[i].r_old << std::endl;
                            file1 << "B" << "\t" << std::setprecision(10) << particles[i].v << std::endl;
                            file2 << "B" << "\t" << std::setprecision(10) << particles[i].r << std::endl;
                         }

                        i++;
                }
		}
		//-------------------- Final velocity and positions ----------------------//
		void finalposvelWrite( std::ofstream& writeConfig ){

			writeConfig.write( reinterpret_cast< const char * >( &npart ), sizeof( npart ) );
            #if WALL_ON
                writeConfig.write( reinterpret_cast< const char * >( &bckgIdxStart ), sizeof( bckgIdxStart ) );
                writeConfig.write( reinterpret_cast< const char * >( &bckgIdxEnd ),   sizeof( bckgIdxEnd ) );
            #endif
			writeConfig.write( reinterpret_cast< const char * >( &step ),  sizeof( step ) );
			//writeConfig.write( reinterpret_cast< const char * >( &seed ),  sizeof( std::default_random_engine ) );

			for (Particle& p : particles){
                // particle types
				writeConfig.write( reinterpret_cast< const char * >( &p.type ), sizeof( p.type ) );

                // particle positions
				writeConfig.write( reinterpret_cast< const char * >( &p.r.X ), sizeof( p.r.X ) );
				writeConfig.write( reinterpret_cast< const char * >( &p.r.Y ), sizeof( p.r.Y ) );
				writeConfig.write( reinterpret_cast< const char * >( &p.r.Z ), sizeof( p.r.Z ) );

                // mid-step velocities ( from Verlet ) 
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
        //---------------------- Create cylinder array -----------------------------//
        #if CYLINDER_ARRAY 
        /*
        void createCylinderArray ( Vec3D p1, Vec3D p2, double cylRad ){

            pCount = 0;

            xind_min = 0.00;
            yind_min = 0.00;
            zind_min = 0.00;

            xind_max = boxEdge[x];
            yind_max = boxEdge[y];
            zind_max = boxEdge[z];
            
            zind = zind_min;
            aCube = pow( 1. / initRho, 1./3. );

            simProg << "***************************************************" << std::endl;
            // simProg << "cylinder with cylCenterX: " << cylCenterX << ", cylCenterZ: " << cylCenterZ << ", cylRad: " << cylRad << std::endl;

            while ( zind < zind_max ){
                xind = xind_min;
                // Particle position intialization in a crystal structure 
                while ( xind < xind_max){
                    yind = yind_min;
                    while( yind < yind_max){

                        x01.X = xind - p1.X; x01.Y = yind - p1.Y; x01.Z = zind - p1.Z;
                        x02.X = xind - p2.X; x02.Y = yind - p2.Y; x02.Z = zind - p2.Z;

                        x21.X = p2.X - p1.X; x21.Y = p2.Y - p1.Y; x21.Z = p2.Z - p1.Z;

                        x012Cross = x01%x02;

                        Num = x012Cross.getLength(); 
                        Den = x21.getLength(); 

                        shortestDist = Num / Den; 
                        // std::cout << "x01 = " <<  x01 << ", x02 = " << x02 <<", x012Cross = " << x012Cross << ", x21= " << x21 << std::endl;
                        // std::cout << "shortestDist = " << shortestDist << std::endl;
            
                        // if ( pow( xind - cylCenterX, 2.0 ) + pow( zind - cylCenterZ, 2.0 ) <= pow( cylRad, 2.0 ) ){
                        if ( shortestDist <= cylRad ){
            
                            // generate random velocities
                            rand_gen_velx = ((double) rand() / (RAND_MAX));
                            rand_gen_vely = ((double) rand() / (RAND_MAX));
                            rand_gen_velz = ((double) rand() / (RAND_MAX));
            
                            // initializing particle radius, mass, position and velocity
                            // if ( xind*xind + yind*yind + zind*zind <= radSqr )
                            particles.push_back({1.0,1.0,{xind, yind, zind},{rand_gen_velx, rand_gen_vely, rand_gen_velz},0});
                            pCount += 1;
            
                        }// inside cylinder
                        yind += aCube*rcutoff;
                    }// yind			
                    xind += aCube*rcutoff;
                }
                zind += aCube*rcutoff;
            }// zind

            simProg << "finished initialization of  " << pCount << " particles inside crystal lattice" << std::endl;
            simProg << "***************************************************" << std::endl;
        }
        */
        #endif

        /******************************* ELLIPSE **********************************************/
        void createEllipseArray ( double Xc, double Zc, double alphaOut, double majAxisOut, double alphaIn, double majAxisIn, bool y_axis ){

            pCount = 0;

            xind_min = 0.00;
            yind_min = 0.00;
            zind_min = 0.00;

            xind_max = boxEdge[x];
            yind_max = boxEdge[y];
            zind_max = boxEdge[z];

            bool outerEllipse, innerEllipse;
            
            zind = zind_min;
            aCube = pow( 1. / initRho, 1./3. );

            simProg << "***************************************************" << std::endl;
            // simProg << "cylinder with cylCenterX: " << cylCenterX << ", cylCenterZ: " << cylCenterZ << ", cylRad: " << cylRad << std::endl;

            while ( zind < zind_max ){
                xind = xind_min;
                // Particle position intialization in a crystal structure 
                while ( xind < xind_max){
                    yind = yind_min;
                    while( yind < yind_max){

                            rand_gen_velx = ((double) rand() / (RAND_MAX));
                            rand_gen_vely = ((double) rand() / (RAND_MAX));
                            rand_gen_velz = ((double) rand() / (RAND_MAX));

                            if ( y_axis ) {
                                outerEllipse = pow( yind - Xc, 2. ) +  pow(alphaIn, 2. ) * pow( zind - Zc , 2. ) <= pow(majAxisOut, 2.);
                                innerEllipse = pow( yind - Xc, 2. ) +  pow(alphaOut, 2. ) * pow( zind - Zc , 2. ) >= pow(majAxisIn, 2.);
                            }
                            else {
                                outerEllipse = pow( xind - Xc, 2. ) +  pow(alphaIn, 2. ) * pow( zind - Zc , 2. ) <= pow(majAxisOut, 2.);
                                innerEllipse = pow( xind - Xc, 2. ) +  pow(alphaOut, 2. ) * pow( zind - Zc , 2. ) >= pow(majAxisIn, 2.);
                            }
            
                            if ( outerEllipse && innerEllipse  ){
                                particles.push_back({0.5,1.0,{xind, yind, zind},{0., 0., 0.},0});
                                pCount++;
                            }
            
                        yind += aCube*rcutoff;
                    }// yind			
                    xind += aCube*rcutoff;
                }
                zind += aCube*rcutoff;
            }// zind

            simProg << "finished initialization of  " << pCount << " particles inside elliptical cylinder" << std::endl;
            simProg << "***************************************************" << std::endl;
        }
        /******************************* SQUARE **********************************************/
        /*
        void createSquareArray ( double squareCenterX, double squareCenterZ, double squareEdge, double rotationTheta ){

            // defining variables
            bool insideSquare;
            double xindRotated;
            double zindRotated;

            // creating a rectangular region
            double innerEdgeX    = - 0.5 * squareEdge;
            double outerEdgeX    =   0.5 * squareEdge;

            double innerEdgeZ    = - 0.5 * squareEdge;
            double outerEdgeZ    =   0.5 * squareEdge;

            aCube = pow( 1. / initRho, 1./3. );

            xind_min = -boxEdge[x];
            yind_min = 0.01;
            zind_min = -boxEdge[z];

            xind_max = boxEdge[x];
            yind_max = boxEdge[y];
            zind_max = boxEdge[z];
            
            zind = zind_min;
            aCube = pow( 1. / initRho, 1./3. );

            simProg << "***************************************************" << std::endl;
            // simProg << "cylinder with cylCenterX: " << cylCenterX << ", cylCenterZ: " << cylCenterZ << ", cylRad: " << cylRad << std::endl;

            pCount = 0;
            while ( zind < zind_max ){
                xind = xind_min;
                while ( xind < xind_max){
                    yind = yind_min;
                    while( yind < yind_max){

                        insideSquare = ( (xind <= outerEdgeX ) && ( xind >= innerEdgeX  ) && (zind <= outerEdgeZ ) && ( zind >= innerEdgeZ ) ) ;

                        if ( insideSquare ){

                            // rotate co-ordinates
                            xindRotated = xind * cos( rotationTheta ) - zind * sin( rotationTheta );
                            zindRotated = xind * sin( rotationTheta ) + zind * cos( rotationTheta );

                            particles.push_back( { 1.0, 1.0, {xindRotated + squareCenterX, yind, zindRotated + squareCenterZ}, {0., 0., 0.}, 0} );
                            pCount++;

                        }// inside square
                        yind += aCube * rcutoff;
                    }// yind			
                    xind += aCube * rcutoff;
                }
                zind += aCube * rcutoff;
            }// zind

            simProg << "finished initialization of  " << pCount << " particles inside elliptical cylinder" << std::endl;
            simProg << "***************************************************" << std::endl;
        }
        */
        void createSquareArray ( double squareCenterX, double squareCenterZ, double squareEdge, double rotationTheta ){

            for (auto p = particles.begin();  p!=particles.end(); ++p){

                double Expr1 = p->r.Z + p->r.X - ( squareCenterZ + squareCenterX + 0.5 * squareEdge );
                double Expr2 = p->r.Z + p->r.X - ( squareCenterZ + squareCenterX - 0.5 * squareEdge );
                double Expr3 = p->r.Z - p->r.X - ( squareCenterZ - squareCenterX + 0.5 * squareEdge );
                double Expr4 = p->r.Z - p->r.X - ( squareCenterZ - squareCenterX - 0.5 * squareEdge );

                bool insideSquare = ( Expr1 <= 0. && Expr2 >=0. && Expr3 <=0. && Expr4 >= 0. );

                if ( insideSquare ){
                    p->type = 0;
                    pCount++;
                }

                Expr1 = p->r.Z + p->r.X - ( squareCenterZ + squareCenterX + 0.2 * squareEdge );
                Expr2 = p->r.Z + p->r.X - ( squareCenterZ + squareCenterX - 0.2 * squareEdge );
                Expr3 = p->r.Z - p->r.X - ( squareCenterZ - squareCenterX + 0.2 * squareEdge );
                Expr4 = p->r.Z - p->r.X - ( squareCenterZ - squareCenterX - 0.2 * squareEdge );

                bool insideCore = ( Expr1 <= 0. && Expr2 >=0. && Expr3 <=0. && Expr4 >= 0. );

                if ( insideCore ){
                    p->type = 3;
                    pCount++;
                }

            }

        }
        //------------------------------ Create spring network : FIXED geometry ------------------------------//
        // creates springs between particles based on nearest neighbour considerations for a fixed geometry //
        // supply the beginning and the ending index of the particles //
        unsigned int springNetworkFixed(unsigned int startIndex, unsigned int endIndex){

           totalBonds = 0;
           double r2;
           double bondDistCutoff = 0.8;

            //loop over all contacts p=1..N-1, q=p+1..N to evaluate forces
            for ( i=startIndex; i< endIndex ; ++i){
                for ( j=i+1;  j<= endIndex; ++j){

                    Vec3D temp;
                    Vec3D minRij;

                    Vec3D Rij = particles[i].r - particles[j].r;    

                    temp.X = Vec3D::roundOff_x(Rij, boxEdge[x]);
                    temp.Y = Vec3D::roundOff_y(Rij, boxEdge[y]);
                    temp.Z = Vec3D::roundOff_z(Rij, boxEdge[z]);

                    minRij.X = Rij.X - temp.X*boxEdge[x];
                    minRij.Y = Rij.Y - temp.Y*boxEdge[y];
                    minRij.Z = Rij.Z - temp.Z*boxEdge[z];

                    r2 = minRij.getLengthSquared();
                    dist = std::sqrt( r2 );

                    if ( dist < bondDistCutoff ) {

                        // simProg << " bond between " << i << " and " << j << std::endl;
                        k = particles[i].bondIndex[0] + 1;

                        // add neighbor to the list
                        particles[i].bondIndex[k]    = j;
                        particles[i].eqBondLength[k] = dist;

                        // increment the number of neighbors
                        particles[i].bondIndex[0]    += 1;                    
                        particles[i].eqBondLength[0] += 1;                 

                        totalBonds++;

                    }       

                }
            }

            return( totalBonds );
        }
        //------------------------------ Create spring network : ENTIRE box ------------------------------//
        // creates springs between particles based on nearest neighbour considerations for the entire box //
        void springNetworkBox(){

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
                                // simProg << "j1 "<<  mi[x] << " " << mi[y] << " " << mi[z] << " " << jj << " " << j << std::endl;

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
                                    // simProg << "j2 " << m << " " << mj[x] << " " << mj[y] << " " << mj[z] << " " << jj << " " << j << std::endl;

                                        #include "createBondIndex.h"

                                } // jj

                            } // m

                        } // ii

            #include "psfWrite.h"

            j = 0;
            i = solid_index[j];
            while ( j < solidCount )
            {
                simProg << "************ Particle " << i << "************" << std::endl;

                for ( k=1; k<= particles[i].bondIndex[0]; ++k )
                    simProg << " Particle " << particles[i].bondIndex[k] << " , eq bond length = " << particles[i].eqBondLength[k] << std::endl; 

                j++;
                i = solid_index[j];
            }
        }
        //------------------------------ Mod function ------------------------------//
        int moduloAB( int A, int B){ 

            int result = std::fmod( A, B);

            result = ( result < 0 ) ? ( result + B ) : result; 

            return( result );
        }
        //------------------------------ Brute force ------------------------------//
        // Brute force implementation of the Force Calculation 
        void forceCalc_bruteForce(){

            //loop over all contacts p=1..N-1, q=p+1..N to evaluate forces
            for (auto p = particles.begin();  p!=particles.end()-1; ++p){
                for (auto q = p+1;  q!=particles.end(); ++q) {

                    Vec3D temp;
                    Vec3D minRij;

                    Vec3D Rij = p->r - q->r;    

                    temp.X = Vec3D::roundOff_x(Rij, boxEdge[x]);
                    temp.Y = Vec3D::roundOff_y(Rij, boxEdge[y]);
                    temp.Z = Vec3D::roundOff_z(Rij, boxEdge[z]);

                    minRij.X = Rij.X - temp.X*boxEdge[x];
                    minRij.Y = Rij.Y - temp.Y*boxEdge[y];
                    minRij.Z = Rij.Z - temp.Z*boxEdge[z];

                    double r2 = minRij.getLengthSquared();

                    if ( r2 < rc2 ) {
                        double r2i = 1/r2;
                        double r6i = pow(r2i,3);

                        double ff = 48.*1.*sig6*r2i*r6i*(sig6*r6i - 0.5);
                        Vec3D Fij = ff*minRij;
                        p->fC += Fij;
                        q->fC += Fij*(-1.0); 

                        // potential energy
                        double pair_pot_en = 4.0*1.*sig6*r6i*(sig6*r6i - 1.0);
                        pot_en += pair_pot_en - ecutLJ;
                    }       

                }
            }
        } 
        };

#endif
