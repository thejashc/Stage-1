#include <iostream>
#include <fstream>
#include <cstdlib>
#include <iomanip>
#include <math.h>

int main(int argc, char** argv){

    // particle properties
    double x,y,z,vx,vy,vz;
    double x_wrt_c, y_wrt_c;
    unsigned int type;
    unsigned int npart;

    unsigned int tStart=atoi(argv[1]);
    unsigned int tEnd=atoi(argv[2]);
    unsigned int tDiff=atoi(argv[3]);
    unsigned int nElem=int((tEnd-tStart)/tDiff)+1;
    unsigned int time;

    unsigned int nSolidCapWall=26581;

    // define capStart and capEnd
    double Lx=20.;
    double Ly=20.;
    double capRad=5.;
    double offSet=2.;
    double effRad2=pow(capRad+offSet,2.);
    double bufferLen=10.;
    double capLen=50.;
    double capStart=bufferLen;
    double capEnd=bufferLen+capLen;
    double dt=0.01;
    double radPos2;

    double cx=0.5*Lx;
    double cy=0.5*Ly;

    unsigned int N_vs_t[nElem][2];
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

        //std::cout << "time=" << time << std::endl;
        // assign time
        N_vs_t[count][0]=time;
        N_vs_t[count][1]=0;

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

                radPos2=pow(x_wrt_c,2.)+pow(y_wrt_c,2.);

                if((z>capStart) && (z<capEnd) && (j>=nSolidCapWall) && (radPos2<=effRad2)){
                    N_vs_t[count][1]++;
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

    /*
    std::cout << N_vs_t[0][0] << ", " << N_vs_t[0][1] << std::endl;
    std::cout << N_vs_t[1][0] << ", " << N_vs_t[1][1] << std::endl;
    std::cout << N_vs_t[2][0] << ", " << N_vs_t[2][1] << std::endl;
    std::cout << N_vs_t[3][0] << ", " << N_vs_t[3][1] << std::endl;
    std::cout << N_vs_t[4][0] << ", " << N_vs_t[4][1] << std::endl;
    */

    // write results to a file
    sprintf(fName, "./postProcessed/z_vs_t.dat");
    writePos.open(fName);
    for(unsigned int j=0;j<nElem;j++)
        writePos << N_vs_t[j][0] << "\t\t" << N_vs_t[j][1] << std::endl;

    writePos.close();

    return 0;
}
