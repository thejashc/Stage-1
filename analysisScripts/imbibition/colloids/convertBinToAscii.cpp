#include <iostream>
#include <fstream>
#include <cstdlib>
#include <iomanip>

int main(int argc, char** argv){

    // particle properties
    double x,y,z,vx,vy,vz;
    unsigned int type;
    unsigned int npart;

    unsigned int tStart = atoi(argv[1]);
    unsigned int tEnd = atoi(argv[2]);
    //unsigned int tEnd = 1046200;
    unsigned int tDiff = 200;
    unsigned int time;

    std::cout << "File start = posVel" << tStart << ".bin" << std::endl;
    std::cout << "File end = posVel" << tEnd << ".bin" << std::endl;

    char fName[100];
    std::ifstream readPosVel;
    std::ofstream writePos;
    std::ofstream writeVel;

    for (time=tStart;time<=tEnd;time+=tDiff){

        // open file streams
        sprintf(fName, "./data/posVel%d.bin",time);
        readPosVel.open( fName, std::ios_base::in);

        sprintf(fName, "./data/pos%d.xyz",time);
        writePos.open( fName, std::ios_base::out);

        sprintf(fName, "./data/vel%d.xyz",time);
        writeVel.open( fName, std::ios_base::out);

        // read the posVel%(time).bin file
        readPosVel.read ( ( char * ) &npart, sizeof (npart) );

        // write lines to the pos and vel files in vmd style format
        writePos << npart << "\n";
        writeVel << npart << "\n";

        writePos << "#X Y Z co-ordinates" << "\n";
        writeVel << "#X Y Z co-ordinates" << "\n";

        // write file if reading is successful
        if( readPosVel.is_open() ){

            //std::cout << npart << std::endl;

            // if file reading successful
            for (int j = 0 ; j<npart ; ++j){	

                readPosVel.read ( ( char * ) &type, sizeof (type) );

                readPosVel.read ( ( char * ) &x,    sizeof (x));
                readPosVel.read ( ( char * ) &y,	sizeof (y));
                readPosVel.read ( ( char * ) &z,	sizeof (z));

                readPosVel.read ( ( char * ) &vx,	sizeof (vx));
                readPosVel.read ( ( char * ) &vy,	sizeof (vy));
                readPosVel.read ( ( char * ) &vz,	sizeof (vz));

                if (type == 0){
                    writePos << "O" << "\t" << x << "\t" << y << "\t" << z << "\n";
                    writeVel << "O" << "\t" << vx << "\t" << vy << "\t" << vz << "\n";
                }
                else if (type == 1){
                    writePos << "H" << "\t" << x << "\t" << y << "\t" << z << "\n";
                    writeVel << "H" << "\t" << vx << "\t" << vy << "\t" << vz << "\n";
                }
                else if (type == 2){
                    writePos << "C" << "\t" << x << "\t" << y << "\t" << z << "\n";
                    writeVel << "C" << "\t" << vx << "\t" << vy << "\t" << vz << "\n";
                }
                else if (type == 3){
                    writePos << "B" << "\t" << x << "\t" << y << "\t" << z << "\n";
                    writeVel << "B" << "\t" << vx << "\t" << vy << "\t" << vz << "\n";
                }
                else if (type == 4){
                    writePos << "N" << "\t" << x << "\t" << y << "\t" << z << "\n";
                    writeVel << "N" << "\t" << vx << "\t" << vy << "\t" << vz << "\n";
                }


                /*
                   std::cout << type << "\t"   << x << "\t" << std::setprecision(15)  
                   << y << "\t"  << std::setprecision(15)     
                   << z << "\t" << std::setprecision(15)   
                   << vx << "\t" << std::setprecision(15)  
                   << vy << "\t" << std::setprecision(15)  
                   << vz << "\n";
                   exit(0);
                   */
            }
            readPosVel.close();
            writePos.close();
            writeVel.close();
        }
        else{
            std::cout << "Error opening file" << std::endl;
            exit(0);
        }

    }

    return 0;
}


