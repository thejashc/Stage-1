// calculating total number of bonds
totalBonds = 0;

j = 0;
i = solid_index[j];
simProg << "counting total number of bonds" << std::endl;
while ( j < solidCount  ){

    totalBonds += particles[i].bondIndex[0];

    j++;
    i = solid_index[j];
}

simProg << " The total number of bonds encountered are : " << totalBonds << std::endl;

// writing the psf file
char filename[40];

sprintf( filename, "./data/Bonds.psf");  

std::ofstream file(filename);

file << "PSF" << std::endl;
file << "\n" ;
file << "       1 !NTITLE" << std::endl;        // 7 spaces
file << " REMARKS etc \n" << std::endl;
file << "   " << npart << " !NATOM: atoms" << std::endl;

i=0;
while ( i < npart ){
    file << std::setw(8) << i+1 << "      " << "1" << "    " << "UNK" << "   " << "S 0" << "  " << "S 0" << "    " << "0.000000" << "        " << "0.0000" << std::endl;

    i++;
}

// writing all the bonds now
j = 0;
i = solid_index[j];
file << "\n" ;
file << "   " << totalBonds << " !NBOND: bonds \n";
unsigned int bondsPerLine = 0;
while ( j < solidCount ){
    for ( k = 1; k <= particles[i].bondIndex[0]; ++k ){
        if ( bondsPerLine < 4 ){
            file << std::setw(8) << i + 1 << std::setw(8) <<  particles[i].bondIndex[k] + 1 << std::setw(8); 
            bondsPerLine++;
        }
        else{
            file << "\n" ;
            file << std::setw(8) << i + 1 << std::setw(8) <<  particles[i].bondIndex[k] + 1 << std::setw(8); 
            bondsPerLine = 1;
        }
    }

    j++;
    i = solid_index[j];
}

// last few lines
file << "\n"  << std::endl;
file << "       0 !NTHETA: angles \n" << std::endl;
file << "       0 !NPHI: dihedrals \n" << std::endl;
file << "       0 !NIMPHI: impropers \n" << std::endl;
file << "       0 !NDON: donors \n" << std::endl;
file << "       0 !NACC: acceptors \n" << std::endl;
file << "       0 !NNB: non-bonded exclusions \n" << std::endl;
file << "       0 \t\t 0 !NGRP: ignored" << std::endl;

file.close();
