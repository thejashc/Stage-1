xCOM /= fluidCount;
yCOM /= fluidCount;
zCOM /= fluidCount;

for ( i = fluid_index[0] ; i <= fluid_index[fluidCount-1] ; ++i ) {

	if ( particles[i].r.X > xCOM ){

		segPlane_ind = round( ( particles[i].r.Z - segPlane_zMin ) / segPlane_zDelta );

		segPlane_xCOM[segPlane_ind] += particles[i].r.X;
		segPlane_zCOM[segPlane_ind] += particles[i].r.Z;

		segPlane_count[segPlane_ind] += 1;
	}
}

// reset variables to 0
xCOM = 0.0;
yCOM = 0.0;
zCOM = 0.0;
