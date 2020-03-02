#include <iostream>
#include <fstream>
#include <cstdlib>
#include <iomanip>
#include <vector>
#include <math.h>

/////////////////////////////////////////////////////////////////////////
////////////////////////////// DECLARATIONS /////////////////////////////
/////////////////////////////////////////////////////////////////////////
void writeMatToFile(std::vector<std::vector<double> >& mat, unsigned int, unsigned int);  // write matrix to a file
void calculate_com(std::vector<std::vector<double> >& mat1, std::vector<double>& mat2,unsigned int, unsigned int);  // write matrix to a file

/////////////////////////////////////////////////////////////////////////
////////////////////////////// MAIN /////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv){

    // particle properties
    double x,y,z,vx,vy,vz;

    unsigned int xbin, zbin;
    unsigned int type;
    unsigned int npart;

    unsigned int tStart=atoi(argv[1]);
    unsigned int tEnd=atoi(argv[2]);
    unsigned int tDiff= 500;
    unsigned int nElem=int((tEnd-tStart)/tDiff)+1;
    unsigned int time;

    unsigned int nSolidCapWall=14442;

    // define wallStart and wallEnd
    double offSet=1.;
    double Lx=22;
    double Ly=10.;
    double Lz=60.;

    double wallStart=offSet;
    double wallEnd=Lx-offSet;              // length of the 2*wallThickness + space in between

    std::vector<double> N;
    std::vector<std::vector<double> > com;

    // resizing the N_or_rz 2D vector
    com.resize(nElem);
    N.resize(nElem);

    for (unsigned int i=0; i<nElem; ++i)   // 3 being x, y and z
        com[i].resize(3);

    // set matrix to 0
    for(unsigned int i=0;i<nElem;i++){
        com[i][0]=0;
        com[i][1]=0;
        com[i][2]=0;

        N[i]=0;
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

            // if file reading successful -- calculate the center of mass of the droplet at each time frame
            for (int j=0 ; j<npart ; ++j){	

                readPosVel.read ( ( char * ) &type, sizeof (type) );

                readPosVel.read ( ( char * ) &x,    sizeof (x));
                readPosVel.read ( ( char * ) &y,	sizeof (y));
                readPosVel.read ( ( char * ) &z,	sizeof (z));

                readPosVel.read ( ( char * ) &vx,	sizeof (vx));
                readPosVel.read ( ( char * ) &vy,	sizeof (vy));
                readPosVel.read ( ( char * ) &vz,	sizeof (vz));

                if(     (x>wallStart) && 
                        (x<wallEnd) && 
                        (j>=nSolidCapWall)  ){

                    //zbin=floor(z/zbinWidth);
                    //xbin=floor(x/xbinWidth);

                    /*
                    if ( (xbin >= xBins) || (zbin >= zBins) ){
                        std::cout << " Oops ! Particle(s) out of bounds " << std::endl;
                        abort();
                    }
                    */

                    // collect all centers of masses
                    com[count][0] += x;
                    com[count][1] += y;
                    com[count][2] += z;

                    N[count] += 1.;
                }
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

    // calculate the center of mass
    calculate_com(com, N, nElem, 3);

    // write File
    writeMatToFile(com, nElem, 3);


    return 0;
}
/////////////////////////////////////////////////////////////////////////
////////////////////////////// FUNCTIONS ////////////////////////////////
/////////////////////////////////////////////////////////////////////////
void calculate_com(std::vector<std::vector<double> >& mat1, std::vector<double>& mat2, unsigned int m, unsigned int n){

    for(int i=0;i<m;++i){
        for(int j=0;j<n;++j){
            mat1[i][j]/=mat2[i];
        }
    }

    return;
}

void writeMatToFile(std::vector<std::vector<double> >& mat, unsigned int m, unsigned int n){

    std::ofstream fWrite;

    char fname[100];
    sprintf(fname, "./postProcessed/com.dat");
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
