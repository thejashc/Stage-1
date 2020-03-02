#!/bin/bash -f

OPT1=1
OPT2=1
OPT3=1
OPT4=0

startFileNum=100000
endFileNum=1000000
deltaFileNum=1000

mkdir -p postProcessed  &&  
mkdir -p plots  &&  

####### OPT1 : Obtain v_{x,y,z} vs y at all times #######
if [ $OPT1 -eq 1 ] 
then 
icpc -fast -o v_vs_t.exe v_vs_t.cpp >> progress.txt &&

#mkdir -p postProcessed/velocity &&

#./v_vs_t.exe $startFileNum $endFileNum $deltaFileNum >> progress.txt &&

echo "v_{x,y,z} vs y as a function of time is obtained" >> progress.txt

fi
####### OPT2 : Obtain <v_{x}(y)>,<v_{y}(y)>,<v_{z}(y)>, <rho(y)> #######
if [ $OPT2 -eq 1 ] 
then 
icpc -fast -o av_v_rho.exe av_v_rho.cpp >> progress.txt &&

#./av_v_rho.exe $startFileNum $endFileNum $deltaFileNum >> progress.txt &&

echo "v_{x,y,z} vs y as a function of time is obtained" >> progress.txt

fi
####### OPT3 : Obtain kinetic viscosity as a function of time #######
if [ $OPT3 -eq 1 ] 
then 
icpc -fast -o kin_visc_xy.exe -std=c++11 kin_visc_xy.cpp >> progress.txt

#./kin_visc_xy.exe $startFileNum $endFileNum $deltaFileNum >> progress.txt &&

echo "kinetic component of viscosity as a function of time is obtained" >> progress.txt

fi
####### OPT3 : Obtain kinetic viscosity as a function of time #######
if [ $OPT4 -eq 1 ] 
then 

./visc.py >> progress.txt &&

echo "viscosity obtained" >> progress.txt

fi
