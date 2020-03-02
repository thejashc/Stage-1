#include <iostream>
#include <fstream>
#include <cstdlib>
#include <iomanip>
#include <math.h>

/////////////////////////////////////////////////////////////////////////
////////////////////////////// DECLARATIONS /////////////////////////////
/////////////////////////////////////////////////////////////////////////
void writeMatToFile(double mat[][12], unsigned int, unsigned int, unsigned int);  // write matrix to a file
void resetToZero(double mat[][12], unsigned int, unsigned int);  // write matrix to a file

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
    unsigned int tDiff= 200;
    unsigned int nElem=int((tEnd-tStart)/tDiff)+1;
    unsigned int time;

    unsigned int nSolidCapWall=56202;

    // define capStart and capEnd
    double Lx=20.;
    double Ly=20.;

    double cx=0.5*Lx;
    double cy=0.5*Ly;

    double rbinWidth=0.5;
    double zbinWidth=1.0;

    double capRad=5.;
    double offSet=1.;
    double effRad=capRad+offSet;
    double effRad2=pow(effRad,2.);
    double bufferLen=10.;
    double capLen=115.;
    double capStart=bufferLen;
    double capEnd=bufferLen+capLen;
    double dt=0.01;
    double radPos2;

    unsigned int rBins=int(effRad/rbinWidth);
    unsigned int zBins=int(capLen/zbinWidth);

    std::cout << rBins << ", " << zBins << ", " << nElem << std::endl;
    double N_of_rz[zBins][12];

    // set matrix to 0
    for(unsigned int j=0;j<rBins;j++){
        for(unsigned int i=0;i<zBins;i++){
            N_of_rz[i][j]=0;
        }
    }

    unsigned int count=0;

    std::cout << "File start = posVel" << tStart << ".bin" << std::endl;
    std::cout << "File end = posVel" << tEnd << ".bin" << std::endl;
    std::cout << " okay 1 " << std::endl;

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
                readPosVel.read ( ( char * ) &y,	sizeof (y));
                readPosVel.read ( ( char * ) &z,	sizeof (z));

                readPosVel.read ( ( char * ) &vx,	sizeof (vx));
                readPosVel.read ( ( char * ) &vy,	sizeof (vy));
                readPosVel.read ( ( char * ) &vz,	sizeof (vz));

                radPos2=pow(x-cx,2.)+pow(y-cy,2.);

                if((z>capStart) && (z<capEnd) && (j>=nSolidCapWall) && (radPos2<=effRad2)){

                    rbin=floor( sqrt(radPos2) / rbinWidth);
                    zbin=floor( (capEnd-z) / zbinWidth);

                    N_of_rz[zbin][rbin] += 1.0;
                }
            }
            readPosVel.close();
        }
        else{
            std::cout << "Error opening file" << std::endl;
            exit(0);
        }

        // write File
        writeMatToFile(N_of_rz, zBins, rBins, count);

        // reset rhoSum, vrSum, vzSum
        resetToZero(N_of_rz, zBins, rBins);

        // increment counter
        count++;
    }


    return 0;
}
/////////////////////////////////////////////////////////////////////////
////////////////////////////// FUNCTIONS ////////////////////////////////
/////////////////////////////////////////////////////////////////////////
void resetToZero(double mat[][12], unsigned int m, unsigned int n){

    for(int i=0;i<m;++i){
        for(int j=0;j<n;++j){
            mat[i][j]=0.;
        }
    }

    return;
}

void writeMatToFile(double mat[][12], unsigned int m, unsigned int n, unsigned int count){

    std::ofstream fWrite;

    char fname[100];
    sprintf(fname, "./postProcessed/density/density_vs_time_%d.dat", count);
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
