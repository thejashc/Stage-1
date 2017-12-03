#!bash

# remove object files
rm -f *.o
rm -f a.out

# cpp compilation 
g++ -std=c++11 Vec2D.cc DPD_main.cpp 
