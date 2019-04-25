//sprintf(posfname, "./restart/frozenParticlePositions/Asl_m_%d_%d_XYZ_Init.xyz", int(-Asl2), int(-Asl1) );
//sprintf(posfname, "./readConfig/solids/CAPILLARY_CYLINDER/Lx_%d_Ly_%d_capLen_%d_capRad_%d_r0.xyz", int(boxEdge[x]), int(boxEdge[y]), int(capLen), int(capRad) );
sprintf(posfname, "./restart/capImb/XYZ1.xyz");

readConfig.open(posfname, std::ifstream::in);

if ( ! readConfig ) { simProg << "*** The restart file " << posfname  << " could not be opened/ does not exist *** \n Aborting !! " << std::endl; abort(); }

readConfig >> nPartPerStructure;
readConfig >> dummyString;
readConfig >> dummyString;
readConfig >> dummyString;
readConfig >> dummyString;

for ( j = 1 ; j <= nPartPerStructure ; ++ j ){	

    readConfig >> type;
    readConfig >> xind;
    readConfig >> yind;
    readConfig >> zind;

    if ( type == 'O' ){
        particles[i].r0.X = xind;
        particles[i].r0.Y = yind;
        particles[i].r0.Z = zind;

        bckgIdxParticles++;

        idx++;
        i = solid_index[idx];
    }
    else if ( type == 'B' ){
        particles[i].r0.X = xind;
        particles[i].r0.Y = yind;
        particles[i].r0.Z = zind;

        bckgIdxParticles++;

        idx++;
        i = solid_index[idx];
    }

    // simProg << type << "\t" << xind << "\t" << yind << "\t" << zind << "\t" << rand_gen_velx << "\t" << rand_gen_vely << "\t" << rand_gen_velz << std::endl; 
}

simProg << "\n  " << nPartPerStructure << " particle initial positions were read" << std::endl;

readConfig.close();
