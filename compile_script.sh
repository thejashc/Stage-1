#!bash

# OCD
reset

# remove object files
rm -f *.txt
rm -f *.o
rm -f a.out
rm -f *.exe
rm -f *.dat
rm -f ./data/*.vtu
rm -f ./data/*.dat
rm -f ./plots/*.eps

# cpp compilation 
g++ -o DPD.exe  -std=c++11 -ggdb3 Vec3D.cc DPD_main.cpp 
#g++ -o DPD.exe  -std=c++11 Vec3D.cc DPD_main.cpp 

#./DPD.exe > simProg.txt &
#./DPD.exe > randnum.dat &
#./DPD.exe > local_density.txt &

# valgrind for checking memory leaks and seg-faults
#valgrind --leak-check=full --track-origins=yes -v ./DPD.exe
