#include <iostream>
#include <fstream>
#include <cstdlib>
#include <iomanip>
#include <math.h>
#include <string>

/////////////////////////////////////////////////////////////////////////
////////////////////////////// DECLARATIONS /////////////////////////////
/////////////////////////////////////////////////////////////////////////
void writeMatToFile(double mat[][2], unsigned int, unsigned int, unsigned int, double, double, std::string &);  // write matrix to a file
void resetToZero(double mat[][2], unsigned int, unsigned int);  // write matrix to a file

/////////////////////////////////////////////////////////////////////////
////////////////////////////// MAIN /////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv){

    // particle properties
    double x, y, z, yi, yo, vx, vy, vz;
    unsigned int type;
    unsigned int npart, idx;

    unsigned int tStart=atoi(argv[1]);
    unsigned int tEnd=atoi(argv[2]);
    unsigned int tDiff=atoi(argv[3]);
    unsigned int nElem=int((tEnd-tStart)/tDiff)+1;
    unsigned int time;

    // define capStart and capEnd
    double Lx=20.;
    double Ly=50.;
    double Lz=3.;

    double ybinWidth=0.1;

    unsigned int rho_nElem = ceil( Ly / ybinWidth ); 

    std::string fileName="rho_vs_y";
    double rho_vs_y[rho_nElem][2];

    // set the matrix to 0
    resetToZero(rho_vs_y, rho_nElem, 2);

    // initialize the radius 
    for ( int i=0; i< rho_nElem; ++i ){
        yi=i*ybinWidth;
        yo=(i+1)*ybinWidth;

        rho_vs_y[i][0] = ( yi + yo )/2.;
    }

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
        double pos[npart][3];       // very inefficient -- I have been lazy here because it runs fast

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

                idx=floor(y/ybinWidth);

                rho_vs_y[idx][1] += 1;

            }
            readPosVel.close();
        }
        else{
            std::cout << "Error opening file" << std::endl;
            exit(0);
        }
    }

    // write File
    writeMatToFile(rho_vs_y, rho_nElem, 2, nElem, Lx, ybinWidth, fileName);

    return 0;
}
/////////////////////////////////////////////////////////////////////////
////////////////////////////// FUNCTIONS ////////////////////////////////
/////////////////////////////////////////////////////////////////////////
void resetToZero(double mat[][2], unsigned int m, unsigned int n){

    for(int i=0;i<m;++i){
        for(int j=0;j<n;++j){
            mat[i][j]=0.;
        }
    }

    return;
}


void writeMatToFile(double mat[][2], unsigned int m, unsigned int n, unsigned int nElem, double Lx, double ybinWidth, std::string &s){

    std::ofstream fWrite;

    char fname[100];
    sprintf(fname, "./postProcessed/%s.dat", s.c_str());
    fWrite.open(fname);

    for(int i=0;i<m;++i){
        for(int j=0;j<n;++j){
            fWrite << mat[i][j] << "\t";
        }
        fWrite << Lx << "\t" << ybinWidth << "\t" << nElem << std::endl;
    }
    fWrite.close();

    return;
}
