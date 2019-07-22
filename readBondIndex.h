std::ifstream readConfig("./restart/Neighbours.bin", std::ios::binary |std::ios_base::in);  // read from binary file

unsigned int totalBonds;
unsigned int refPartIdx;
unsigned int bondPartIdx;

unsigned int idx;
double eqBondLen;

char dummy[100];

//readConfig >> totalBonds;
readConfig.read ( ( char * ) &totalBonds,sizeof (unsigned int) );
simProg << " A total of " << totalBonds << " bonds will be assigned " << std::endl;

/*
readConfig >> dummy;
readConfig >> dummy;
readConfig >> dummy;
readConfig >> dummy;
readConfig >> dummy;
readConfig >> dummy;
readConfig >> dummy;
*/

for( unsigned int line=1; line <=totalBonds; line++ ){

	readConfig.read ( ( char * ) &refPartIdx,	sizeof ( unsigned int ) );
	readConfig.read ( ( char * ) &bondPartIdx,	sizeof ( unsigned int )  );
	readConfig.read ( ( char * ) &eqBondLen,	sizeof ( double )  );

    /*
    readConfig >> refPartIdx;
    readConfig >> bondPartIdx;
    readConfig >> eqBondLen;
    */

    idx = particles[refPartIdx].bondIndex[0] + 1;

    // add neighbor to the list
    particles[refPartIdx].bondIndex[idx]    = bondPartIdx;
    particles[refPartIdx].eqBondLength[idx] = eqBondLen;

    // increment the number of neighbors
    particles[refPartIdx].bondIndex[0]    += 1;                    
    particles[refPartIdx].eqBondLength[0] += 1;                 
}

readConfig.close();
