// std::cout << "capTubeStart = " << capTubeStart << std::endl;
// std::cout << "capTubeEnd = " << capTubeEnd << std::endl;

// exit(0);

if ( particles[fluid_index[i]].r.Z > capTubeStart  && particles[fluid_index[i]].r.Z < capTubeEnd ){

	distInLeftWall 	= particles[fluid_index[i]].r.X -  ( sqInnerEdgeXmin - wallPenetration );
	distInRightWall = particles[fluid_index[i]].r.X -  ( sqInnerEdgeXmax + wallPenetration );
	distInBottomWall= particles[fluid_index[i]].r.Y -  ( sqInnerEdgeYmin - wallPenetration );
	distInTopWall	= particles[fluid_index[i]].r.Y -  ( sqInnerEdgeYmax + wallPenetration );

	reg1	= ( distInLeftWall   < 0.  ) && ( particles[fluid_index[i]].r.X > sqXmin ) ; 
	reg2	= ( distInRightWall  > 0.  ) && ( particles[fluid_index[i]].r.X < sqXmax ) ; 
	reg3	= ( distInBottomWall < 0.  ) && ( particles[fluid_index[i]].r.Y > sqYmin ) ; 
	reg4	= ( distInTopWall    > 0.  ) && ( particles[fluid_index[i]].r.Y < sqYmax ) ; 

	if ( reg1 ) {

		if ( reg3 ){

			particles[fluid_index[i]].fext.X = -Brep * distInLeftWall;
			particles[fluid_index[i]].fext.Y = -Brep * distInBottomWall;
			particles[fluid_index[i]].fext.Z = 0.;

		}
		else if ( reg4 ){

			particles[fluid_index[i]].fext.X = -Brep * distInLeftWall;
			particles[fluid_index[i]].fext.Y = -Brep * distInTopWall;
			particles[fluid_index[i]].fext.Z = 0.;
		}
		else{
			particles[fluid_index[i]].fext.X = -Brep * distInLeftWall;
			particles[fluid_index[i]].fext.Y = 0.;
			particles[fluid_index[i]].fext.Z = 0.;
		}
	}   // reg1
	else if ( reg2 ){

		if ( reg3 ){

			particles[fluid_index[i]].fext.X = -Brep * distInRightWall;
			particles[fluid_index[i]].fext.Y = -Brep * distInBottomWall;
			particles[fluid_index[i]].fext.Z = 0.;
		}
		else if ( reg4 ){

			particles[fluid_index[i]].fext.X = -Brep * distInRightWall;
			particles[fluid_index[i]].fext.Y = -Brep * distInTopWall;
			particles[fluid_index[i]].fext.Z = 0.;
		}
		else{

			particles[fluid_index[i]].fext.X = -Brep * distInRightWall;
			particles[fluid_index[i]].fext.Y = 0.;
			particles[fluid_index[i]].fext.Z = 0.;

		}

	}   // reg2 
	else if ( reg3 ){

		particles[fluid_index[i]].fext.X = 0.;
		particles[fluid_index[i]].fext.Y = -Brep * distInBottomWall;
		particles[fluid_index[i]].fext.Z = 0.;

	}   // reg3
	else if ( reg4 ){

		particles[fluid_index[i]].fext.X = 0.;
		particles[fluid_index[i]].fext.Y = -Brep * distInTopWall;
		particles[fluid_index[i]].fext.Z = 0.;

	}   // reg4
}
