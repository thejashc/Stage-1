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
