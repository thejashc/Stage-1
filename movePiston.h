appForce = appPressure * pistonArea;          								// push means appPressure -ve, pull means appPressure +ve, delForce = z_{cap} direction
delForce = ( - forceOnPiston - appForce ) * ( dt / pistonW ) * ( step > pistonT0 );	      // force deficit is always with reference to the applied force: 

// Move piston depending on force
vzPist 	= vz0Pist + delForce * ( dt );
drPist	= vzPist * dt;

// move the spring-points and the piston particles by an amount delR
i = pistonStartIndex;

while ( i <= pistonEndIndex ){
	particles[i].r0.Z += drPist;  
	particles[i].r.Z  += drPist;
	
	i++;
}

// assign vz0Pist ( for next time step ) as the vz for this time step
vz0Pist = vzPist;
