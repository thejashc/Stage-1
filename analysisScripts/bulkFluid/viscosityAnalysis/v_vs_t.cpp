#include <iostream>
#include <fstream>
#include <cstdlib>
#include <iomanip>
#include <math.h>

/////////////////////////////////////////////////////////////////////////
////////////////////////////// DECLARATIONS /////////////////////////////
/////////////////////////////////////////////////////////////////////////
void writeMatToFile(double mat[][4], unsigned int, unsigned int, unsigned int);  // write matrix to a file
void resetToZero(double mat[][4], unsigned int, unsigned int);  // write matrix to a file

/////////////////////////////////////////////////////////////////////////
////////////////////////////// MAIN /////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv){

    // particle properties
    double x,y,z,vx,vy,vz;

    unsigned int rbin, zbin;
    unsigned int type;
    unsigned int npart;

    unsigned int tStart=atoi(argv[1]);
    unsigned int tEnd=atoi(argv[2]);
    unsigned int tDiff=atoi(argv[3]);
    unsigned int nElem=int((tEnd-tStart)/tDiff)+1;
    unsigned int time;
    unsigned int idx;

    // define capStart and capEnd
    double Lx=15.;
    double Ly=15.;
    double Lz=3.;
    double vol=Lx*Ly*Lz;

    double ybinWidth=0.5;

    unsigned int yBins=int(Ly/ybinWidth);

    std::cout << "yBins= " << yBins << std::endl;

    double v_vs_y[yBins][4];

    // set matrix to 0
    for(unsigned int j=0;j<4;j++){
        for(unsigned int i=0;i<yBins;i++){
            v_vs_y[i][j]=0;
        }
    }

    unsigned int count=0;

    std::cout << "File start = posVel" << tStart << ".bin" << std::endl;
    std::cout << "File end = posVel" << tEnd << ".bin" << std::endl;

    char fName[100];
    std::ifstream readPosVel;
    std::ofstream writePos;

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
                readPosVel.read ( ( char * ) &y,    sizeof (y));
                readPosVel.read ( ( char * ) &z,    sizeof (z));

                readPosVel.read ( ( char * ) &vx,   sizeof (vx));
                readPosVel.read ( ( char * ) &vy,   sizeof (vy));
                readPosVel.read ( ( char * ) &vz,   sizeof (vz));

                idx=floor(y/ybinWidth);

                v_vs_y[idx][0] += vx;       // increase the x,y,z velocity inside a bin
                v_vs_y[idx][1] += vy;
                v_vs_y[idx][2] += vz;

                v_vs_y[idx][3] += 1.;          // increase the number of particle in the respective bin

            }
            readPosVel.close();
        }
        else{
            std::cout << "Error opening file" << std::endl;
            exit(0);
        }

        // write File
        writeMatToFile(v_vs_y, yBins, 4, count);

        // reset rhoSum, vrSum, vzSum
        resetToZero(v_vs_y, yBins, 4);

        // increment counter
        count++;
    }

    return 0;
}

/////////////////////////////////////////////////////////////////////////
////////////////////////////// FUNCTIONS ////////////////////////////////
/////////////////////////////////////////////////////////////////////////
void resetToZero(double mat[][4], unsigned int m, unsigned int n){

    for(int i=0;i<m;++i){
        for(int j=0;j<n;++j){
            mat[i][j]=0.;
        }
    }

    return;
}

void writeMatToFile(double mat[][4], unsigned int m, unsigned int n, unsigned int count){

    std::ofstream fWrite;

    char fname[100];
    sprintf(fname, "./postProcessed/velocity/v_vs_y_%d.dat", count);
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
