#include <iostream>
#include <fstream>
#include <cstdlib>
#include <iomanip>
#include <math.h>
#include <string>

/////////////////////////////////////////////////////////////////////////
////////////////////////////// DECLARATIONS /////////////////////////////
/////////////////////////////////////////////////////////////////////////
void writeMatToFile(double mat[][3], unsigned int, unsigned int, std::string &);  // write matrix to a file
void resetToZero(double mat[][3], unsigned int, unsigned int);  // write matrix to a file

/////////////////////////////////////////////////////////////////////////
////////////////////////////// MAIN /////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv){

    // particle properties
    double x,y,z,vx,vy,vz;
    unsigned int type;
    unsigned int npart;

    unsigned int tStart=atoi(argv[1]);
    unsigned int tEnd=atoi(argv[2]);
    unsigned int tDiff=atoi(argv[3]);
    unsigned int nElem=int((tEnd-tStart)/tDiff)+1;
    unsigned int time;

    // define capStart and capEnd
    std::string fileName="temperature";
    double temperature[nElem][3];

    // set the matrix to 0
    resetToZero(temperature, nElem, 3);

    std::cout << "File start = posVel" << tStart << ".bin" << std::endl;
    std::cout << "File end = posVel" << tEnd << ".bin" << std::endl;

    char fName[100];
    std::ifstream readPosVel;

    unsigned int count=0;
    for (time=tStart;time<=tEnd;time+=tDiff){

        // open file streams
        sprintf(fName, "./data/posVel%d.bin",time);
        readPosVel.open( fName, std::ios_base::in);

        // read the posVel%(time).bin file
        readPosVel.read ((char *) &npart, sizeof (npart));

        // write file if reading is successful
        if( readPosVel.is_open() ){

            // if file reading successful
            for (int j = 0 ; j<npart ; ++j){	

                readPosVel.read ( ( char * ) &type, sizeof (type) );

                readPosVel.read ( ( char * ) &x,    sizeof (x));
                readPosVel.read ( ( char * ) &y,	sizeof (y));
                readPosVel.read ( ( char * ) &z,	sizeof (z));

                readPosVel.read ( ( char * ) &vx,	sizeof (vx));
                readPosVel.read ( ( char * ) &vy,	sizeof (vy));
                readPosVel.read ( ( char * ) &vz,	sizeof (vz));

                temperature[count][0] += pow(vx,2.);
                temperature[count][1] += pow(vy,2.);
                temperature[count][2] += pow(vz,2.);

            }
            readPosVel.close();
        }
        else{
            std::cout << "Error opening file" << std::endl;
            exit(0);
        }

        // increment counter
        count++;
    }

    // write File
    writeMatToFile(temperature, nElem, 3, fileName);

    return 0;
}
/////////////////////////////////////////////////////////////////////////
////////////////////////////// FUNCTIONS ////////////////////////////////
/////////////////////////////////////////////////////////////////////////
void resetToZero(double mat[][3], unsigned int m, unsigned int n){

    for(int i=0;i<m;++i){
        for(int j=0;j<n;++j){
            mat[i][j]=0.;
        }
    }

    return;
}

void writeMatToFile(double mat[][3], unsigned int m, unsigned int n, std::string &s){

    std::ofstream fWrite;

    char fname[100];
    sprintf(fname, "./postProcessed/%s.dat", s.c_str());
    fWrite.open(fname);

    for(int i=0;i<m;++i){
        for(int j=0;j<n;++j){
            fWrite << mat[i][j] << "\t" ;
        }
        fWrite << std::endl;
    }
    fWrite.close();

    return;
}
