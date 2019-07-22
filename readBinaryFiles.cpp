#include <iostream>
#include <fstream>
#include <cstdlib>
#include <iomanip>

int main(){

    // particle properties
    double x,y,z,vx,vy,vz;
    unsigned int type;
    unsigned int npart;

    npart = 188035;

    char fName[100];
    sprintf(fName, "./data/posVel100.bin");
    std::ifstream readConfig;

    // read file
    readConfig.open( fName, std::ios_base::in);
    readConfig.read ( ( char * ) &npart, sizeof (npart) );
    std::cout << npart << std::endl;
    for (int j = 0 ; j<npart ; ++j){	

        readConfig.read ( ( char * ) &type, sizeof (type) );

        readConfig.read ( ( char * ) &x,    sizeof (x));
        readConfig.read ( ( char * ) &y,	sizeof (y));
        readConfig.read ( ( char * ) &z,	sizeof (z));

        readConfig.read ( ( char * ) &vx,	sizeof (vx));
        readConfig.read ( ( char * ) &vy,	sizeof (vy));
        readConfig.read ( ( char * ) &vz,	sizeof (vz));

        // simProg << particleType << " " << std::setprecision(15) << xind << std::setprecision(15) << " " << yind << std::setprecision(15) << " " <<  zind << std::endl;
        // simProg << particleType << " " << std::setprecision(15) << rand_gen_velx << std::setprecision(15) << " " << rand_gen_vely << std::setprecision(15) << " " <<  rand_gen_velz << std::endl;
        // simProg << type << " " << rand_gen_velx << " " << rand_gen_vely << " " << rand_gen_velz << std::endl;
        //
        std::cout << type << "\t"   << x << "\t" << std::setprecision(15)  
                                    << y << "\t"  << std::setprecision(15)     
                                    << z << "\t" << std::setprecision(15)   
                                    << vx << "\t" << std::setprecision(15)  
                                    << vy << "\t" << std::setprecision(15)  
                                    << vz << "\n";
        exit(0);
    }
    readConfig.close();

    return 0;
}


