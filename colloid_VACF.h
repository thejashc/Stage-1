// finding the average velocity
colloid_com_Vel = totCOM / solidCount; 

colloid_com_Vel_Av = ( 1./3. )* ( colloid_com_Vel.X + colloid_com_Vel.Y + colloid_com_Vel.Z );

// std::cout << colloid_com_Vel << "\n" << std::endl;

// correlating the velocities
recursive_addCorr( colloid_com_Vel.X,  0, 1 );
recursive_addCorr( colloid_com_Vel.Y,  1, 1 );
recursive_addCorr( colloid_com_Vel.Z,  2, 1 );
recursive_addCorr( colloid_com_Vel_Av, 3, 1 ); 
                   
// auto-correlation at time t=0
normalizeCorr[0]  +=  colloid_com_Vel.X * colloid_com_Vel.X;
normalizeCorr[1]  +=  colloid_com_Vel.Y * colloid_com_Vel.Y;
normalizeCorr[2]  +=  colloid_com_Vel.Z * colloid_com_Vel.Z;
normalizeCorr[3]  +=  colloid_com_Vel_Av * colloid_com_Vel_Av;

normalizeCorr_count += 1;
