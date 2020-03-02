#include <iostream>
#include <fstream>
#include <cstdlib>
#include <iomanip>
#include <math.h>

/////////////////////////////////////////////////////////////////////////
////////////////////////////// DECLARATIONS /////////////////////////////
/////////////////////////////////////////////////////////////////////////
void writeMatToFile(double mat[][4], unsigned int, unsigned int, unsigned int, double, double, double);  // write matrix to a file

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
    double N;

    // define capStart and capEnd
    double Lx=9.;
    double Ly=9.;
    double Lz=9.;
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

    std::cout << "File start = posVel" << tStart << ".bin" << std::endl;
    std::cout << "File end = posVel" << tEnd << ".bin" << std::endl;

    char fName[100];
    std::ifstream readPosVel;
    std::ofstream writePos;

    std::cout << "nElem = " << nElem <<std::endl;
    for (unsigned int count=0;count<nElem;count++){

        // open file streams
        sprintf(fName, "./postProcessed/velocity/v_vs_y_%d.dat",count);
        readPosVel.open( fName, std::ios::in);

        // write file if reading is successful
        if( readPosVel.is_open() ){

            // if file reading successful
            for (int idx=0 ; idx<yBins; ++idx){

                readPosVel >> vx;
                readPosVel >> vy;
                readPosVel >> vz;

                readPosVel >> N;

                v_vs_y[idx][0] += vx;       // increase the x,y,z velocity inside a bin
                v_vs_y[idx][1] += vy;
                v_vs_y[idx][2] += vz;

                v_vs_y[idx][3] += N;          // increase the number of particle in the respective bin

            }
            readPosVel.close();
        }
        else{
            std::cout << "Error opening file" << std::endl;
            exit(0);
        }
    }

    // write File
    writeMatToFile(v_vs_y, yBins, 4, nElem, Lx, Ly, ybinWidth);

    return 0;
}

/////////////////////////////////////////////////////////////////////////
////////////////////////////// FUNCTIONS ////////////////////////////////
/////////////////////////////////////////////////////////////////////////
void writeMatToFile(double mat[][4], unsigned int m, unsigned int n, unsigned int nElem, double Lx, double Ly, double dy){

    std::ofstream fWrite;

    char fname[100];
    sprintf(fname, "./postProcessed/av_v_vs_y.dat");
    fWrite.open(fname);

    for(int i=0;i<m;++i){
        for(int j=0;j<n-1;++j){
            fWrite << mat[i][j]/mat[i][3] << "\t" ;
        }
        fWrite << std::endl;
    }
    fWrite.close();

    sprintf(fname, "./postProcessed/av_rho_vs_y.dat");
    fWrite.open(fname);

    for(int i=0;i<m;++i)
        fWrite << mat[i][3]/(nElem*dy*Lx*Ly) << "\n" ;

    fWrite.close();

    return;
}
