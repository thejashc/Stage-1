#!/bin/bash

# OCD
reset

# remove object files
rm -f *.txt
rm -f *.o
rm -f *.exe
rm -f *.dat
rm -f ./data/*.xyz
rm -f ./data/*.dat
rm -f ./data/*.vtu
rm -f ./data/*.psf
rm -f ./data/*.bin
rm -f ./plots/*.eps
rm -f param.out

# cpp compilation 
#g++ -O1 -O2 -o DPD.exe -static -std=c++11 -ggdb3 Vec3D.cc DPD_main.cpp 
#g++ -O2 -o DPD.exe -std=c++11 Vec3D.cc DPD_main.cpp 
#g++ -Wl,--stack, 4194304 -o DPD.exe  -std=c++11 Vec3D.cc DPD_main.cpp -- doesnt work
#g++ -O2 -pg -o DPD.exe -std=c++11 Vec3D.cc DPD_main.cpp
g++ -o convert.exe -std=c++11 convertBinToAscii.cpp 

# intel compiler
icpc -fast -o DPD.exe -std=c++11 Vec3D.cc DPD_main.cpp
#icpc -prof-gen -o DPD.exe -std=c++11 Vec3D.cc DPD_main.cpp
#icpc -prof-use -o DPD.exe -std=c++11 Vec3D.cc DPD_main.cpp
#icpc -O2 -o DPD.exe -std=c++11 Vec3D.cc DPD_main.cpp

#./DPD.exe > simProg.txt &
#./DPD.exe > randnum.dat &
#./DPD.exe > local_density.txt &

# valgrind for checking memory leaks and seg-faults
#valgrind --leak-check=full --track-origins=yes -v ./DPD.exe
