std::ifstream partPerFibre( "./readConfig/randomFibreBundle/particlesPerFibre.dat", std::ifstream::in);
if ( ! partPerFibre ) { simProg << "*** The parts per fibre file could not be opened/ does not exist *** \n Aborting !! " << std::endl; abort(); }

unsigned int bondsPerFibre;
unsigned int noOfFibres;
unsigned int startIdx, endIdx;
// read all the startingIndices of fibres
partPerFibre >> noOfFibres;
unsigned int fibreStartId[noOfFibres];

for ( i=0; i< noOfFibres; )
    partPerFibre >> fibreStartId[i++];

startIdx = 0;
endIdx = fibreStartId[0] - 1;
// attach particles within a fibre
for (unsigned int m=1; m <= noOfFibres; m++ ){

    bondsPerFibre = springNetworkFixed(startIdx, endIdx);

    simProg << "Fibre No.: " << m 
            << ", startidx : " << startIdx 
            << ", endIdx: " << endIdx 
            << ", noOfParticles: " << endIdx - (startIdx - 1)
            << ", bonds: " << bondsPerFibre << std::endl;

    startIdx = endIdx + 1;
    endIdx = endIdx + fibreStartId[m];
}

partPerFibre.close();
