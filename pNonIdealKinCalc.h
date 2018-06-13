/* calculating kinetic contribution to the pressure tensor: 
The total kinetic stress contribution due to particles is split between boundary and bulk region. 
bulk region :[ 0.25*Ly, 0.75*Ly ]
boundary region : [ 0, 0.25*Ly ) and ( 0.75*Ly, Ly ] 

Only particles within this region contribute to the kinetic component of the stress tensor. This
is implemented by multiplying the numerical value with a conditional. 

I have been lazy, hence, I have split up the contribution between bulk and boundaries into the variables
pNonIdealKin_temp[0][1] and pNonIdealKin_temp[1][0], respectively. As soon as this test is done, revert 
back to the actual definitions. 
*/

// Non-Ideal contribution to pressure -- Conservative forces	
pNonIdealKin_temp[0][0] 	+= particles[i].v.X * particles[i].v.X;
pNonIdealKin_temp[0][1] 	+= particles[i].v.X * particles[i].v.Y; 
pNonIdealKin_temp[0][2] 	+= particles[i].v.X * particles[i].v.Z;

pNonIdealKin_temp[1][0] 	+= particles[i].v.Y * particles[i].v.X;
pNonIdealKin_temp[1][1] 	+= particles[i].v.Y * particles[i].v.Y;
pNonIdealKin_temp[1][2] 	+= particles[i].v.Y * particles[i].v.Z;

pNonIdealKin_temp[2][0] 	+= particles[i].v.Z * particles[i].v.X;
pNonIdealKin_temp[2][1] 	+= particles[i].v.Z * particles[i].v.Y;
pNonIdealKin_temp[2][2] 	+= particles[i].v.Z * particles[i].v.Z;

// pNonIdealKin_temp[0][1] 	+= ( particles[i].v.X * particles[i].v.Y ) * ( ( particles[i].r.Y >= 0.25 * boxEdge[y] ) && ( particles[i].r.Y <= 0.75 * boxEdge[y] )  );
// pNonIdealKin_temp[1][0] 	+= ( particles[i].v.X * particles[i].v.Y ) * ( ( particles[i].r.Y < 0.25 * boxEdge[y] ) || ( particles[i].r.Y > 0.75 * boxEdge[y] )  );
