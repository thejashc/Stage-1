#!bash

# remove object files
rm -f *.o
rm -f a.out
rm -f *.dat
#rm -f ./data/*

# cpp compilation 
g++ -std=c++11 Vec3D.cc DPD_main.cpp 
