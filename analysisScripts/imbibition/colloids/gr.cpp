#include <iostream>
#include <fstream>
#include <cstdlib>
#include <iomanip>
#include <math.h>

/////////////////////////////////////////////////////////////////////////
////////////////////////////// DECLARATIONS /////////////////////////////
/////////////////////////////////////////////////////////////////////////
void readFile(double colloidCOM[][3], unsigned int, unsigned int);    // read colloid com's
void radialRDF(double gr[][2], double colloidCOM[][3], unsigned int, double, double, double);
void writeRDF(double gr[][2], unsigned int, double, unsigned int);
//void axialRDF(double pos[][3], unsigned int, unsigned int);    // calculate height distribution

/////////////////////////////////////////////////////////////////////////
////////////////////////////// MAIN /////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv){

    // simulation parameters
    unsigned int npart=188035;      // total number of particles
    unsigned int NColloids=120;     // total number of colloids
    unsigned int maxNgbrs=100;      // maximum number of particles within a colloid

    double Lx=20.;                  // x and y dimensions of the box
    double Ly=20.;

    double capEnd=150.;             // z position of the capillary pore throat
    double capRad=5.;               // radius of the capillary
    double dr=0.5;                  // width of bin
    unsigned int nBins=int(capRad/dr);    // total number of bins

    // file-processing start and end points, folder
    unsigned int tStart=atoi(argv[1]);
    unsigned int tEnd=atoi(argv[2]);

    unsigned int tDiff= 200;
    unsigned int nElem=int((tEnd-tStart)/tDiff)+1;

    std::cout << "File start = posVel" << tStart << ".bin" << std::endl;
    std::cout << "File end = posVel" << tEnd << ".bin" << std::endl;

    unsigned int N_vs_t[nElem][2];
    unsigned int colIdx[NColloids][maxNgbrs];
    double colloidCOM[NColloids][3];    // xyz center of mass
    double pos[npart][3];           // position of all particles at a given time
    double gr[nBins][2];            // radial distribution function for colloids

    for (int time=tStart;time<=tEnd;time+=tDiff){

        readFile(colloidCOM, time, NColloids);        // read the posVel%t.bin file and store positions in pos matrix
        radialRDF(gr, colloidCOM, NColloids, capEnd, Lx, Ly);       // calculate the distribution of colloids in the radial direction 
        //axialRDF(ax_gr, colloidCOM);                // calculate the distribution of colloids in the axial direction
    }

    std::cout << "radial RDF calculated" << std::endl;

    writeRDF(gr, nBins, dr, nElem);                                 // write the radial distribution function
    return 0;
}

/////////////////////////////////////////////////////////////////////////
////////////////////////////// ROUTINES /////////////////////////////////
/////////////////////////////////////////////////////////////////////////
// read x,y,z com of all colloids
void readFile(double colloidCOM[][3], unsigned int time, unsigned int NColloids){

    unsigned int npart;
    char fName[100];
    std::ifstream fRead;
    double x,y,z,vx,vy,vz;
    unsigned int type;

    // open file streams
    sprintf(fName, "./postProcessed/colloidCOMdata/COM_%d.dat",time);
    fRead.open( fName, std::ios::in);

    // write file if reading is successful
    if( fRead.is_open() ){

        // if file reading successful
        for (int j = 0 ; j<NColloids; ++j){	

            fRead >> colloidCOM[j][0];
            fRead >> colloidCOM[j][1];
            fRead >> colloidCOM[j][2];
        }
        fRead.close();
    }
    else{
        std::cout << "Error opening file" << std::endl;
        exit(0);
    }
    return;
}

void radialRDF(double gr[][2], double colloidCOM[][3], unsigned int NColloids, double capEnd, double Lx, double Ly){

    double x,y,z;
    double r;
    unsigned int bin;

    double dr=0.5;
    double zMin=capEnd-75.;
    double zMax=capEnd-25.;

    double cx=0.5*Lx;
    double cy=0.5*Ly;

    for(int j=0; j<NColloids; ++j ){                   // loop over all colloids
        z=colloidCOM[j][2];         
        if((z>zMin) && (z<zMax)){         // check if colloids are within a given height range
             
            x=colloidCOM[j][0]-cx;          // x-coordinate w.r.t center of cylinder (also box)
            y=colloidCOM[j][1]-cy;          // y-coordinate w.r.t center of cylinder (also box)

            r=sqrt(pow(x,2.)+pow(y,2.));    // squared of the radius

            bin=floor(r/dr);                // bin 

            gr[bin][1]+=1;                  // increment the counter of the bin
        }
    }
    return;
}

void writeRDF(double gr[][2], unsigned int nBins, double dr, unsigned int nElem){

    std::ofstream fWrite;
    char fName[100];
    double r;
    double pi=3.14;

    // open file streams
    sprintf(fName, "./postProcessed/gr.dat");
    fWrite.open(fName, std::ios::out);

    fWrite << "#" << "\t" <<  "r"  << "\t" << "2*pi*r*dr" << "\t" << "nSamples" << "\t" << "N(r)" << std::endl;
    if( fWrite.is_open() ){
        for(int i=0;i<nBins;++i){
            r = (i + 0.5)*dr;
            fWrite << r  << "\t" << 2*pi*r*dr << "\t" << nElem << "\t" << gr[i][1] << std::endl;
        }
    }
    else{
        std::cout << "Error opening file" << std::endl;
        exit(0);
    }
    fWrite.close();
   
    return;
}
