#include <iostream>
#include <fstream>
#include <cstdlib>
#include <iomanip>
#include <vector>
#include <math.h>

/////////////////////////////////////////////////////////////////////////
////////////////////////////// DECLARATIONS /////////////////////////////
/////////////////////////////////////////////////////////////////////////
void writeMatToFile(std::vector<std::vector<double> >& mat, unsigned int, unsigned int, unsigned int);  // write matrix to a file
void resetToZero(std::vector<std::vector<double> >& mat, unsigned int, unsigned int);  // write matrix to a file

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

    double xcom;
    double zcom;
    double x_wrt_wallStart;

    double zmin=-(Lz/2.)-10.;
    double xmin=-(Lx/2.)-10.;

    double zmax=(Lz/2.)+10.;
    double xmax=(Lx/2.)+10.;

    double xbinWidth=0.5;
    double zbinWidth=0.5;

    double wallStart=offSet;
    double wallEnd=Lx-offSet;              // length of the 2*wallThickness + space in between

    unsigned int xBins=int((xmax-xmin)/xbinWidth);
    unsigned int zBins=int((zmax-zmin)/zbinWidth);

    std::cout << "xBins = " <<  xBins << ", zBins=" << zBins << ", nElem=" << nElem << std::endl;
    std::vector<std::vector<double> > N_of_rz;
    std::vector<std::vector<double> > com;

    // resizing the N_or_rz 2D vector
    com.resize(nElem);
    N_of_rz.resize(zBins);

    for (unsigned int i=0; i<zBins; ++i)
        N_of_rz[i].resize(xBins);

    for (unsigned int i=0; i<nElem; ++i)   // 3 being x, y and z
        com[i].resize(3);

    // read the center of mass file
    std::ifstream fRead;

    char fname[100];
    sprintf(fname, "./postProcessed/com.dat");
    fRead.open(fname);

    for(unsigned int i=0;i<nElem;i++){

        fRead >> com[i][0];
        fRead >> com[i][1];
        fRead >> com[i][2];
    }

    fRead.close();

    // set matrix to 0
    for(unsigned int j=0;j<xBins;j++){
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

        xcom=com[count][0];
        zcom=com[count][2];

        // write file if reading is successful
        if( readPosVel.is_open() ){

            // if file reading successful
            for (int j=0 ; j<npart ; ++j){	

                readPosVel.read ( ( char * ) &type, sizeof (type) );

                readPosVel.read ( ( char * ) &x,    sizeof (x));
                readPosVel.read ( ( char * ) &y,	sizeof (y));
                readPosVel.read ( ( char * ) &z,	sizeof (z));

                readPosVel.read ( ( char * ) &vx,	sizeof (vx));
                readPosVel.read ( ( char * ) &vy,	sizeof (vy));
                readPosVel.read ( ( char * ) &vz,	sizeof (vz));

                x-=xcom;    // shift particle positions to the center of mass frame of reference
                z-=zcom;

                if(    // (x>wallStart) && 
                       // (x<wallEnd) && 
                        (j>=nSolidCapWall)  ){

                    xbin=floor((x-xmin)/xbinWidth);
                    zbin=floor((z-zmin)/zbinWidth);

                    if ( (xbin >= xBins)){
                        std::cout << " Oops ! Particle(s) out of bounds with x and xbin =" << x << ", " << xbin << std::endl;
                        abort();
                    }
                    if ( (zbin >= zBins)){
                        std::cout << " Oops ! Particle(s) out of bounds with z and zbin = " << z << ", " << zbin << std::endl;
                        abort();
                    }

                    N_of_rz[zbin][xbin] += 1.0;
                }
            }
            readPosVel.close();
        }
        else{
            std::cout << "Error opening file" << std::endl;
            exit(0);
        }

        // write File
        writeMatToFile(N_of_rz, zBins, xBins, count);

        // reset rhoSum, vrSum, vzSum
        resetToZero(N_of_rz, zBins, xBins);

        // increment counter
        count++;
    }


    return 0;
}
/////////////////////////////////////////////////////////////////////////
////////////////////////////// FUNCTIONS ////////////////////////////////
/////////////////////////////////////////////////////////////////////////
void resetToZero(std::vector<std::vector<double> >& mat, unsigned int m, unsigned int n){

    for(int i=0;i<m;++i){
        for(int j=0;j<n;++j){
            mat[i][j]=0.;
        }
    }

    return;
}

void writeMatToFile(std::vector<std::vector<double> >& mat, unsigned int m, unsigned int n, unsigned int count){

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
