#!bash

# remove object files
rm -f *.o
rm -f a.out
rm -f *.dat
rm -f ./data/*.vtu
rm -f ./data/*.dat
rm -f ./plots/*.eps

# cpp compilation 
#g++ -o DPD.exe  -std=c++11 -ggdb3 Vec3D.cc DPD_main.cpp 
g++ -o DPD.exe  -std=c++11 Vec3D.cc DPD_main.cpp 
