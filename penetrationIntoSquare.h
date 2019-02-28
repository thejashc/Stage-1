// std::cout << "capTubeStart = " << capTubeStart << std::endl;
// std::cout << "capTubeEnd = " << capTubeEnd << std::endl;

// exit(0);
//
idx = 0;
i = fluid_index[idx];

/*
std::cout   <<  sqInnerEdgeXmin << ", " << 
                sqInnerEdgeXmax << ", " << 
                sqInnerEdgeYmin << ", " << 
                sqInnerEdgeYmax << ", " <<
                wallLowPos << ", " <<
                capTubeStart << ", " <<
                capTubeEnd << ", " <<
                wallPenetration;
*/

while ( idx < fluidCount ){

    if ( ( particles[i].r.Z > capTubeStart )  && ( particles[i].r.Z < capTubeEnd ) ){

        distInLeftWall 	= particles[i].r.X -  ( sqInnerEdgeXmin - wallPenetration );
        distInRightWall = particles[i].r.X -  ( sqInnerEdgeXmax + wallPenetration );
        distInBottomWall= particles[i].r.Y -  ( sqInnerEdgeYmin - wallPenetration );
        distInTopWall	= particles[i].r.Y -  ( sqInnerEdgeYmax + wallPenetration );

        reg1	= ( distInLeftWall   < 0.  ) && ( particles[i].r.X > sqXmin ) ; 
        reg2	= ( distInRightWall  > 0.  ) && ( particles[i].r.X < sqXmax ) ; 
        reg3	= ( distInBottomWall < 0.  ) && ( particles[i].r.Y > sqYmin ) ; 
        reg4	= ( distInTopWall    > 0.  ) && ( particles[i].r.Y < sqYmax ) ; 

        if ( reg1 ) {

            if ( reg3 ){

                particles[i].fext.X = -Brep[0][1] * distInLeftWall;
                particles[i].fext.Y = -Brep[0][1] * distInBottomWall;
                particles[i].fext.Z = 0.;

            }
            else if ( reg4 ){

                particles[i].fext.X = -Brep[0][1] * distInLeftWall;
                particles[i].fext.Y = -Brep[0][1] * distInTopWall;
                particles[i].fext.Z = 0.;
            }
            else{
                particles[i].fext.X = -Brep[0][1] * distInLeftWall;
                particles[i].fext.Y = 0.;
                particles[i].fext.Z = 0.;
            }
        }   // reg1
        else if ( reg2 ){

            if ( reg3 ){

                particles[i].fext.X = -Brep[0][1] * distInRightWall;
                particles[i].fext.Y = -Brep[0][1] * distInBottomWall;
                particles[i].fext.Z = 0.;
            }
            else if ( reg4 ){

                particles[i].fext.X = -Brep[0][1] * distInRightWall;
                particles[i].fext.Y = -Brep[0][1] * distInTopWall;
                particles[i].fext.Z = 0.;
            }
            else{

                particles[i].fext.X = -Brep[0][1] * distInRightWall;
                particles[i].fext.Y = 0.;
                particles[i].fext.Z = 0.;

            }

        }   // reg2 
        else if ( reg3 ){

            particles[i].fext.X = 0.;
            particles[i].fext.Y = -Brep[0][1] * distInBottomWall;
            particles[i].fext.Z = 0.;

        }   // reg3
        else if ( reg4 ){

            particles[i].fext.X = 0.;
            particles[i].fext.Y = -Brep[0][1] * distInTopWall;
            particles[i].fext.Z = 0.;

        }   // reg4
    }
    else if ( ( particles[i].r.Z > capTubeEnd ) && ( particles[i].r.Z < wallLowPos ) ){

        // (1)  wall force can be in the region where the reservoir separating wall is present
        wallLowDist 	= particles[i].r.Z - ( wallLowPos - wallPenetration );

        // make sure that the force is applied only particles inside the wall
        notInPoreEntry  =   ( particles[i].r.X > sqInnerEdgeXmax ) || 
                            ( particles[i].r.X < sqInnerEdgeXmin ) || 
                            ( particles[i].r.Y > sqInnerEdgeYmax ) || 
                            ( particles[i].r.Y < sqInnerEdgeYmin ); 

        // apply external force if particle is below the threshold penetration distance and not in the pore entry
        if ( wallLowDist < 0. && notInPoreEntry ){

            particles[i].fext.X = 0;
            particles[i].fext.Y = 0;
            particles[i].fext.Z = -Brep[0][1] * wallLowDist ; 
        }
    }

    // update particle index
    idx++;
    i = fluid_index[idx];
}
