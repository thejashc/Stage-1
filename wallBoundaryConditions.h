#if WALL_ON

    #if UPPER_WALL_ON && LOWER_WALL_ON

        // additional force from soft potential to avoid particles from entering into wall
        wallLowDist = particles[fluid_index[i]].r.Z - ( wallLowPos - wallPenetration );
        wallTopDist = particles[fluid_index[i]].r.Z - ( wallTopPos + wallPenetration );

        if ( wallLowDist < 0. )
            particles[fluid_index[i]].fext.Z = -Brep[0][1] * wallLowDist;		// Lower wall -- act on particles below threshold: wallHeight + tolerance
        else if ( wallTopDist > 0. )
            particles[fluid_index[i]].fext.Z = -Brep[0][1] * wallTopDist;		// Upper wall -- act on particles above threshold: wallHeight - tolerance

        //simProg << "step = " << step << ", ( step > 50 ) " << ( step > 50 ) << ", particles[fluid_index[i]].fext.Z " << particles[fluid_index[i]].fext.Z << std::endl;

        // no external force in X and Y directions : ** Buggy : this should be calculated for the wall-normal direction ** 
        particles[fluid_index[i]].fext.X = 0.; 
        particles[fluid_index[i]].fext.Y = 0.;
        particles[fluid_index[i]].fext.Z = 0.;

    #elif !(UPPER_WALL_ON) && LOWER_WALL_ON


        /*
        droplet_ZcomNew += particles[fluid_index[i]].r.Z;

        // std::cout << "Zcom of drop = " << droplet_Zcom << ", capRad = " << capRad << ", zind_max = " << zind_max << std::endl;

        particles[fluid_index[i]].fext.X = 0;
        particles[fluid_index[i]].fext.Y = 0;
        particles[fluid_index[i]].fext.Z = -1e-3 * ( step > 1e4 ) * ( residual >= 4.5 * rcutoff ) ;
        */

        // additional force from soft potential to avoid particles from entering into wall
        wallLowDist = particles[fluid_index[i]].r.Z - ( wallLowPos - wallPenetration );

        if ( wallLowDist < 0. )
            particles[fluid_index[i]].fext.Z = -Brep[0][1] * wallLowDist;		    // Lower wall -- act on particles below threshold: wallHeight + tolerance

        //simProg << "step = " << step << ", ( step > 50 ) " << ( step > 50 ) << ", particles[fluid_index[i]].fext.Z " << particles[fluid_index[i]].fext.Z << std::endl;

        // no external force in X and Y directions : ** Buggy : this should be calculated for the wall-normal direction ** 
        particles[fluid_index[i]].fext.X = 0.; 
        particles[fluid_index[i]].fext.Y = 0.;
        particles[fluid_index[i]].fext.Z = 0.;

    #elif CAPILLARY_CYLINDER
        
        // (1)  wall force can be in the region where the reservoir separating wall is present
        wallLowDist 	= particles[fluid_index[i]].r.Z - ( wallLowPos - wallPenetration );
        #if PISTON
            distInPiston 	= particles[fluid_index[i]].r.Z - ( pistonStart + wallPenetration );
        #endif

        Rij.X	=  ( cylCenterX - particles[fluid_index[i]].r.X );		// Finding the inward normal to the cylinder
        Rij.Y	=  ( cylCenterY - particles[fluid_index[i]].r.Y );
        Rij.Z	=  0.;

        r2	    = Rij.getLengthSquared();
        dist	= std::sqrt( r2 );
        
        rInner	= capRad + wallPenetration;
        rOuter  = capRad + capThick; 

        penDist = dist - rInner;	// distance penetrated
        perDist = dist - rOuter;	// distance from the capillary tube periphery

        inCapTube   = ( ( particles[fluid_index[i]].r.Z > capTubeStart ) && ( particles[fluid_index[i]].r.Z < capTubeEnd ) );
        notInPoreEntry = dist > capRad;

        /*
        std::cout << " wallLowPos = " << wallLowPos 
                  << ", \n wallPenetration = " << wallPenetration
                  << ", \n capThick = " << capThick 
                  << ", \n capRad = " << capRad 
                  << ", \n rInner = " << rInner
                  << ", \n rOuter = " << rOuter << std::endl;
        */

        // the force acts only: (1.1) if it has penetrated beyond wallPenetration inside capillary tube [if]
        // 		   	(1.2) if it is inside the wall region next to capillary [else if]
        if ( wallLowDist < 0. && particles[fluid_index[i]].r.Z > ( wallLowPos - capWallWdth ) && notInPoreEntry ){

            particles[fluid_index[i]].fext.X = 0;
            particles[fluid_index[i]].fext.Y = 0;
            particles[fluid_index[i]].fext.Z = -Brep[0][1] * wallLowDist ; 
        }
        else if ( ( penDist > 0 ) && ( perDist < 0 ) && inCapTube ){

            capRij =  Rij / dist;

            particles[fluid_index[i]].fext.X = Brep[0][1] * penDist * capRij.X ;
            particles[fluid_index[i]].fext.Y = Brep[0][1] * penDist * capRij.Y ;
            particles[fluid_index[i]].fext.Z =  0.;
        }
        #if PISTON
            else if ( distInPiston > 0. ){
                particles[fluid_index[i]].fext.X = 0;
                particles[fluid_index[i]].fext.Y = 0;
                particles[fluid_index[i]].fext.Z = -Brep[0][1] * distInPiston; 
            }
        #endif

    #elif CAPILLARY_SQUARE

        #include "penetrationIntoSquare.h"

        // (1)  wall force can be in the region where the reservoir separating wall is present
        wallLowDist 	= particles[fluid_index[i]].r.Z - ( wallLowPos - wallPenetration );
        #if PISTON
            distInPiston 	= particles[fluid_index[i]].r.Z - ( pistonStart + wallPenetration );
        #endif
        notInPoreEntry  = ( particles[fluid_index[i]].r.X > sqInnerEdgeXmax ) || 
                          ( particles[fluid_index[i]].r.X < sqInnerEdgeXmin ) || 
                          ( particles[fluid_index[i]].r.Y > sqInnerEdgeYmax ) || 
                          ( particles[fluid_index[i]].r.Y < sqInnerEdgeYmin ); 

        // to ensure particles mix inside the reservoir and
        // the large fluctuations are killed once that is 
        // ensured then open the gates peacefully
        if ( wallLowDist < 0. && particles[fluid_index[i]].r.Z > ( wallLowPos - capWallWdth ) && notInPoreEntry ){

            particles[fluid_index[i]].fext.X = 0;
            particles[fluid_index[i]].fext.Y = 0;
            particles[fluid_index[i]].fext.Z = -Brep[0][1] * wallLowDist ; 
        }

        #if PISTON
            else if ( distInPiston > 0. ){
                particles[fluid_index[i]].fext.X = 0;
                particles[fluid_index[i]].fext.Y = 0;
                particles[fluid_index[i]].fext.Z = -Brep[0][1] * distInPiston; 
            }
        #endif

    #elif CYLINDER_ARRAY 
        /*
        droplet_ZcomNew += particles[fluid_index[i]].r.Z;

        std::cout << "Zcom of drop = " << droplet_Zcom << ", capRad = " << capRad << ", zind_max = " << zind_max << std::endl;

        particles[fluid_index[i]].fext.X = 0;
        particles[fluid_index[i]].fext.Y = 0;
        particles[fluid_index[i]].fext.Z = -1e-3 * ( step > 10000 ) * ( residual >= 4.5 * rcutoff ) ;
        */

    #endif

#endif // WALL_ON
