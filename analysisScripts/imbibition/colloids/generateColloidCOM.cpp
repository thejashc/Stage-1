#include <iostream>
#include <fstream>
#include <cstdlib>
#include <iomanip>
#include <math.h>

/////////////////////////////////////////////////////////////////////////
////////////////////////////// DECLARATIONS /////////////////////////////
/////////////////////////////////////////////////////////////////////////
void readColIdx(unsigned int colIdx[][100], unsigned int, unsigned int);
void writeMatToFile(unsigned int mat[][100], unsigned int, unsigned int);  // write matrix to a file
void readFile(double pos[][3], unsigned int);    // read particle positions
void obtainColloidCOM(double pos[][3], unsigned int colIdx[][100], double colloidCOM[][3], unsigned int NColloids); // obtain com-x,y,z of all colloids
void writeColloidCOM(double colloidCOM[][3], unsigned int, unsigned int);

/////////////////////////////////////////////////////////////////////////
////////////////////////////// MAIN /////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv){

    // simulation parameters
    unsigned int npart=188035;      // total number of particles
    unsigned int NColloids=120;     // total number of colloids
    unsigned int maxNgbrs=100;      // maximum number of particles within a colloid

    // file-processing start and end points, folder
    unsigned int tStart=atoi(argv[1]);
    unsigned int tEnd=atoi(argv[2]);
    unsigned int tDiff= 200;

    std::cout << "File start = posVel" << tStart << ".bin" << std::endl;
    std::cout << "File end = posVel" << tEnd << ".bin" << std::endl;

    unsigned int colIdx[NColloids][maxNgbrs];
    double colloidCOM[NColloids][3];    // xyz center of mass
    double pos[npart][3];           // position of all particles at a given time

    readColIdx(colIdx, NColloids, maxNgbrs);
    writeMatToFile(colIdx,NColloids,maxNgbrs);  // write matrix to a file, provide name of matrix, # of rows, # of columns

    for (int time=tStart;time<=tEnd;time+=tDiff){

        readFile(pos, time);        // read the posVel%t.bin file and store positions in pos matrix
        obtainColloidCOM(pos,colIdx,colloidCOM, NColloids);   // obtain com-x,y,z of all colloids
        writeColloidCOM(colloidCOM, time, NColloids);   // write the center of mass colloid information
    }
    return 0;
}

/////////////////////////////////////////////////////////////////////////
////////////////////////////// ROUTINES /////////////////////////////////
/////////////////////////////////////////////////////////////////////////

// read indices of particles making up the colloid 
// colIdx is a matrix containing NColloids-rows and 100-columns
// It cotains the indices of all particles making up a colloidal particle
void readColIdx(unsigned int colIdx[][100], unsigned int NColloids, unsigned int maxNgbrs){

    char fName[100];
    std::ifstream readIdx;
    unsigned int ngbr;

    std::cout << "reading the colloid indices file" << std::endl;
    sprintf(fName, "./postProcessed/Ngbrs_colloids.dat");
    readIdx.open(fName, std::ios::in);
    for(int j=0;j<NColloids;++j){
        for(int i=0;i<maxNgbrs;++i){
            readIdx >> ngbr;
            colIdx[j][i]=ngbr;
        }
    }
    readIdx.close();
    std::cout << "colloid indices loaded" << std::endl;

    return;
}

// write a matrix to a file with m-rows and n-columns
void writeMatToFile(unsigned int mat[][100], unsigned int m, unsigned int n){

    std::ofstream fWrite;

    char fname[100];
    sprintf(fname, "./postProcessed/colloidIndicesCheck.dat");
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

// read x,y,z positions of a file
void readFile(double pos[][3], unsigned int time){

    unsigned int npart;
    char fName[100];
    std::ifstream readPosVel;
    double x,y,z,vx,vy,vz;
    unsigned int type;

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

            pos[j][0]=x;
            pos[j][1]=y;
            pos[j][2]=z;
        }
        readPosVel.close();
    }
    else{
        std::cout << "Error opening file" << std::endl;
        exit(0);
    }

    return;
}

// read x,y,z positions of a file
void obtainColloidCOM(double pos[][3], unsigned int colIdx[][100], double colloidCOM[][3], unsigned int NColloids){ // obtain com-x,y,z of all colloids

    unsigned int idx;
    unsigned int partPerColloid;

    for(unsigned int n=0;n<NColloids;++n){      // loop over all colloids

        colloidCOM[n][0]=0.;            // set the center of mass of nth colloid to 0,0,0
        colloidCOM[n][1]=0.;
        colloidCOM[n][2]=0.;

        partPerColloid=colIdx[n][0];
        for(unsigned int j=1;j<=partPerColloid;++j){   // loop over all particle indices comprising nth colloid

            idx=colIdx[n][j];               // obtain particle index

            colloidCOM[n][0]+=pos[idx][0];  // x-com of the nth colloid - beware there is no correction for the COM
            colloidCOM[n][1]+=pos[idx][1];
            colloidCOM[n][2]+=pos[idx][2];
        }

        colloidCOM[n][0] /= partPerColloid; // normalize by the total number of particles within colloid
        colloidCOM[n][1] /= partPerColloid; 
        colloidCOM[n][2] /= partPerColloid; 
    }

    /*
     * DEBUGGING
    for(unsigned int n=0;n<NColloids;++n)
        std::cout << colloidCOM[n][0] << ", " << colloidCOM[n][1] << ", " << colloidCOM[n][2] << std::endl;
        */


    return;
}

// write the colloid com information to a file
void writeColloidCOM(double colloidCOM[][3], unsigned int time, unsigned int NColloids){

    std::ofstream fWrite;

    char fname[100];
    sprintf(fname, "./postProcessed/colloidCOMdata/COM_%d.dat",time);
    fWrite.open(fname, std::ios::out);

    for(int i=0;i<NColloids;++i)
        fWrite << colloidCOM[i][0] << "\t" << colloidCOM[i][1] <<"\t"<< colloidCOM[i][2] << std::endl;

    fWrite.close();

    return;
}
