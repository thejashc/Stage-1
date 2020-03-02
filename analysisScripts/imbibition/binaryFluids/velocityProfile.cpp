#include <iostream>
#include <fstream>
#include <cstdlib>
#include <iomanip>
#include <math.h>
#include <string>

/////////////////////////////////////////////////////////////////////////
////////////////////////////// DECLARATIONS /////////////////////////////
/////////////////////////////////////////////////////////////////////////
void writeMatToFile(double mat[][12], unsigned int, unsigned int, std::string &, unsigned int, char);  // write matrix to a file
void resetToZero(double mat[][12], unsigned int, unsigned int);  // write matrix to a file
void readMeniscusPos(double mat[][2], unsigned int);  // read Meniscus Position

/////////////////////////////////////////////////////////////////////////
////////////////////////////// MAIN /////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv){

    // particle properties
    double x,y,z,vx,vy,vz;
    double x_wrt_c, y_wrt_c, z_wrt_lowMen, zOffsetToMeniscus;
    double r,vr, rcap_x, rcap_y;

    unsigned int type;
    unsigned int npart;
    unsigned int zbin, rbin;

    unsigned int tStart=atoi(argv[1]);
    unsigned int tEnd=atoi(argv[2]);
    unsigned int tDiff=200;
    unsigned int nElem=int((tEnd-tStart)/tDiff)+1;
    unsigned int time;

    unsigned int nSolidCapWall=56202;

    std::string rho_fname="rho";
    std::string vr_fname="vr";
    std::string vz_fname="vz";

    // define capStart and capEnd
    double Lx=20.;
    double Ly=20.;

    char typeA='A';
    char typeB='B';

    double cx=0.5*Lx;
    double cy=0.5*Ly;

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

    double distBehindMeniscus=15.;                                   // distance behind meniscus - towards reservoir
    double distFrontMeniscus=10.;                                   // distance in front of meniscus - away from reservoir
    double distAroundMeniscus=distBehindMeniscus+distFrontMeniscus; // total length of distance around meniscus

    double rbinWidth=0.5;
    double zbinWidth=1.0;

    unsigned int rBins=int(effRad/rbinWidth);
    unsigned int zBins=int(distAroundMeniscus/zbinWidth);

    double rhoSumA[zBins][12];
    double vrSumA[zBins][12];
    double vzSumA[zBins][12];

    double rhoSumB[zBins][12];
    double vrSumB[zBins][12];
    double vzSumB[zBins][12];

    double meniscusPos[nElem][2];

    std::cout << "File start = posVel" << tStart << ".bin" << std::endl;
    std::cout << "File end = posVel" << tEnd << ".bin" << std::endl;

    char fName[100];
    std::ifstream readPosVel;

    readMeniscusPos(meniscusPos, nElem);        // read the position of the meniscus
    /*
     * DEBUG 1
    for (int i=0;i<nElem;++i)
        std::cout << meniscusPos[i][0] << "\t\t" << meniscusPos[i][1] << "\n";
        */

    // reset rhoSum, vrSum, vzSum
    resetToZero(rhoSumA, zBins, rBins);
    resetToZero(vrSumA, zBins, rBins);
    resetToZero(vzSumA, zBins, rBins);

    resetToZero(rhoSumB, zBins, rBins);
    resetToZero(vrSumB, zBins, rBins);
    resetToZero(vzSumB, zBins, rBins);

    unsigned int count=0;
    for (time=tStart;time<=tEnd;time+=tDiff){

        // open file streams
        sprintf(fName, "./data/posVel%d.bin",time);
        readPosVel.open( fName, std::ios_base::in);

        // read the posVel%(time).bin file
        readPosVel.read ((char *) &npart, sizeof (npart));

        //std::cout << "time=" << time << std::endl;
        zOffsetToMeniscus=capEnd-(meniscusPos[count][1] + distFrontMeniscus);

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

                x_wrt_c=x-cx;                                       // x-coordinate w.r.t to center of box
                y_wrt_c=y-cy;                                       // y-coordinate w.r.t to center of box
                z_wrt_lowMen=z-zOffsetToMeniscus;                     // z position w.r.t to 10 units behind meniscus

                radPos2=pow(x_wrt_c,2.)+pow(y_wrt_c,2.);            // square of the distance around meniscus

                if( (z_wrt_lowMen > 0.) && 
                    (z_wrt_lowMen < distAroundMeniscus) && 
                    (radPos2 < effRad2) &&
                    (j>=nSolidCapWall) ){
                     
                    r=sqrt(radPos2);

                    rcap_x = x_wrt_c/r;
                    rcap_y = y_wrt_c/r;

                    vr = vx * rcap_x + vy * rcap_y;          // projection of v on rcap

                    zbin=floor(z_wrt_lowMen/ zbinWidth);
                    rbin=floor(r/rbinWidth);

                    if (type == 1 ){
                        rhoSumA[zbin][rbin] += 1.;                 // add to the density
                        vrSumA[zbin][rbin] +=  vr;                // add to the radial velocity
                        vzSumA[zbin][rbin] +=  vz;                // add to the axial velocity
                    }
                    else if (type == 2){
                        rhoSumB[zbin][rbin] += 1.;                 // add to the density
                        vrSumB[zbin][rbin] +=  vr;                // add to the radial velocity
                        vzSumB[zbin][rbin] +=  vz;                // add to the axial velocity
                    }

                }

            }
            readPosVel.close();
        }
        else{
            std::cout << "Error opening file" << std::endl;
            exit(0);
        }

        // write File
        writeMatToFile(rhoSumA, zBins, rBins, rho_fname, count, typeA);
        writeMatToFile(vrSumA, zBins, rBins, vr_fname, count, typeA);
        writeMatToFile(vzSumA, zBins, rBins, vz_fname, count, typeA);

        writeMatToFile(rhoSumB, zBins, rBins, rho_fname, count, typeB);
        writeMatToFile(vrSumB, zBins, rBins, vr_fname, count, typeB);
        writeMatToFile(vzSumB, zBins, rBins, vz_fname, count, typeB);

        // reset rhoSum, vrSum, vzSum
        resetToZero(rhoSumA, zBins, rBins);
        resetToZero(vrSumA, zBins, rBins);
        resetToZero(vzSumA, zBins, rBins);

        resetToZero(rhoSumB, zBins, rBins);
        resetToZero(vrSumB, zBins, rBins);
        resetToZero(vzSumB, zBins, rBins);

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

    return 0;
}
/////////////////////////////////////////////////////////////////////////
////////////////////////////// FUNCTIONS ////////////////////////////////
/////////////////////////////////////////////////////////////////////////
void readMeniscusPos(double meniscusPos[][2], unsigned int maxCount){

    char fName[100];
    std::ifstream readIdx;

    double time;
    double pos;

    std::cout << "reading the position of the meniscus" << std::endl;
    sprintf(fName, "./postProcessed/meniscusPosition.dat");
    readIdx.open(fName, std::ios::in);
    for(int j=0;j<maxCount;++j){

            readIdx >> time;
            readIdx >> pos;

            meniscusPos[j][0]=time;
            meniscusPos[j][1]=pos;
    }
    readIdx.close();
    std::cout << "meniscus Position loaded" << std::endl;

    return;
}

void resetToZero(double mat[][12], unsigned int m, unsigned int n){

    for(int i=0;i<m;++i){
        for(int j=0;j<n;++j){
            mat[i][j]=0.;
        }
    }

    return;
}

void writeMatToFile(double mat[][12], unsigned int m, unsigned int n, std::string &s, unsigned int count, char type){

    std::ofstream fWrite;

    char fname[100];
    sprintf(fname, "./postProcessed/velocityAnalysis/%s%c_%d.dat", s.c_str(), type, count);
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
