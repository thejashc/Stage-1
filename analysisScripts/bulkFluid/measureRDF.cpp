#include <iostream>
#include <fstream>
#include <cstdlib>
#include <iomanip>
#include <math.h>
#include <string>

/////////////////////////////////////////////////////////////////////////
////////////////////////////// DECLARATIONS /////////////////////////////
/////////////////////////////////////////////////////////////////////////
void writeMatToFile(double mat[][2], unsigned int, unsigned int, std::string &);  // write matrix to a file
void measureRDF(double pos[][3], double RDF[][2], unsigned int npart, double Lx, double Ly, double Lz, double rbinWidth, unsigned int RDF_nElem);
void resetToZero(double mat[][2], unsigned int, unsigned int);  // write matrix to a file

/////////////////////////////////////////////////////////////////////////
////////////////////////////// MAIN /////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv){

    // particle properties
    double x,y,z,vx,vy,vz, ri, ro;
    unsigned int type;
    unsigned int npart;

    unsigned int tStart=atoi(argv[1]);
    unsigned int tEnd=atoi(argv[2]);
    unsigned int tDiff=atoi(argv[3]);
    unsigned int nElem=int((tEnd-tStart)/tDiff)+1;
    unsigned int time;

    // define capStart and capEnd
    double Lx=15.;
    double Ly=15.;
    double Lz=2.;

    double rbinWidth=0.01;

    unsigned int RDF_nElem = ceil( Lx / rbinWidth ); 

    std::string fileName="rdf";
    double RDF[RDF_nElem][2];

    // set the matrix to 0
    resetToZero(RDF, RDF_nElem, 2);

    // initialize the radius 
    for ( int i=0; i< RDF_nElem; ++i ){
        ri=i*rbinWidth;
        ro=(i+1)*rbinWidth;

        RDF[i][0] = ( ri + ro )/2.;
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

                pos[j][0] = x;
                pos[j][1] = y;
                pos[j][2] = z;

            }
            readPosVel.close();
        }
        else{
            std::cout << "Error opening file" << std::endl;
            exit(0);
        }

        // measure gr
        measureRDF(pos, RDF, npart, Lx, Ly, Lz, rbinWidth, RDF_nElem);
    }

    // write File
    writeMatToFile(RDF, RDF_nElem, 2, fileName);

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


void measureRDF(double pos[][3], double RDF[][2], unsigned int npart, double Lx, double Ly, double Lz, double rbinWidth, unsigned int RDF_nElem){

    unsigned int i, j, ig;
    double rij_x, rij_y, rij_z, dist, r2;

    //loop over all contacts p=1..N-1, q=p+1..N to evaluate forces
    for( i=0; i <= npart-1; ++i ){
        for ( j=i+1; j<= npart; ++j ){

            rij_x = pos[i][0] - pos[j][0];      // x-component of the distance
            rij_y = pos[i][1] - pos[j][1];      // y-component of the distance
            rij_z = pos[i][2] - pos[j][2];      // z-component of the distance

            // nearest image distance
            rij_x   -= Lx * round( rij_x / Lx );	
            rij_y   -= Ly * round( rij_y / Ly );
            rij_z   -= Lz * round( rij_z / Lz );

            r2      = pow(rij_x, 2.) + pow(rij_y, 2.) + pow(rij_z, 2.);
            dist    = sqrt(r2);

            ig = ceil( dist / rbinWidth ) - 1;
            if ( (ig < 0) || (ig>=RDF_nElem) ) {std::cout << " gRcalc(): out of bounds for dist = " << dist << " and ig = " << ig  << std::endl; abort();} 
            RDF[ig][1] += 2.0;	
        }
    }

    return;
}

void writeMatToFile(double mat[][2], unsigned int m, unsigned int n, std::string &s){

    std::ofstream fWrite;

    char fname[100];
    sprintf(fname, "./postProcessed/%s.dat", s.c_str());
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
