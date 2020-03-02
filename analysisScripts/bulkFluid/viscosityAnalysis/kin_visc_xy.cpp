#include <iostream>
#include <fstream>
#include <cstdlib>
#include <iomanip>
#include <math.h>
#include <vector>

/////////////////////////////////////////////////////////////////////////
////////////////////////////// DECLARATIONS /////////////////////////////
/////////////////////////////////////////////////////////////////////////
void writeMatToFile(std::vector<double>& mat, unsigned int, double);  // write matrix to a file

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

    double sum_vx, sum_vy, sum_vz;
    unsigned int N;
    double Lx=15.;
    double Ly=15.;
    double Lz=3.;
    double vol=Lx*Ly*Lz;

    double ybinWidth=0.5;

    unsigned int yBins=int(Ly/ybinWidth);

    // define kin_visc_xy
    std::vector<double> av_vx(yBins);
    std::vector<double> av_vy(yBins);

    std::vector<double> kin_visc_xy;
    kin_visc_xy.resize(nElem);

    for(unsigned int j=0;j<nElem;j++)
        kin_visc_xy[j]=0;

    unsigned int count=0;

    std::cout << "File start = posVel" << tStart << ".bin" << std::endl;
    std::cout << "File end = posVel" << tEnd << ".bin" << std::endl;

    char fName[100];
    std::ifstream readPosVel;
    std::ifstream readAvVel;
    std::ofstream writePos;

    for (time=tStart;time<=tEnd;time+=tDiff){

        // extract average velocities in x and y at every time
        sprintf(fName, "./postProcessed/velocity/v_vs_y_%d.dat",count);
        readAvVel.open( fName, std::ios::in);

        for(int i=0;i<yBins;++i){

            readAvVel >> sum_vx;        // obtain the sum 
            readAvVel >> sum_vy;
            readAvVel >> sum_vz;        // dummy for this program
            readAvVel >> N;            // dummy for this program

            av_vx[i] = sum_vx/N;    // average velocity in x 
            av_vy[i] = sum_vy/N;    // average velocity in y
        }

        readAvVel.close();

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

                idx=floor(y/ybinWidth);                                          // calculate which bin the particle belongs to 

                kin_visc_xy[count] += (vx - av_vx[idx])*(vy - av_vy[idx]);       // xy component of kinetic viscosity at time t -- sum over all particles

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
    writeMatToFile(kin_visc_xy, nElem, vol);

    return 0;
}

/////////////////////////////////////////////////////////////////////////
////////////////////////////// FUNCTIONS ////////////////////////////////
/////////////////////////////////////////////////////////////////////////
void writeMatToFile(std::vector<double>& mat, unsigned int m, double vol){

    std::ofstream fWrite;

    char fname[100];
    sprintf(fname, "./postProcessed/kin_visc_xy.dat");
    fWrite.open(fname);

    for(int i=0;i<m;++i)
        fWrite << mat[i]/vol << "\n" ;

    fWrite.close();

    return;

}
