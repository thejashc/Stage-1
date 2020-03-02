#!/bin/bash -f

begin=$(date +%s)
### This script is used to run python scripts to 
### generate velocity vector fields.
#
#### (0) convert the binary to ascii data files
startFileNum=98000
startAnalysisFrom=200000
endFileNum=882000

OPT1=1
OPT2=1
OPT3=1
OPT4=1
OPT5=1
OPT6=1

mkdir -p postProcessed  &&
mkdir -p plots  &&
#
##./z_vs_t.exe $startFileNum $endFileNum > progress.txt &&
#
##### (1) Obtain meniscus position as a function of time : z(t) vs t 
##### Input : data/pos{$t}.xyz 
##### Output : postProcessed/imbibitionFront.dat
if [ $OPT1 -eq 1 ]
then 
    icpc -fast -o density_vs_time.exe density_vs_time.cpp >> progress.txt &&

    mkdir -p postProcessed/density &&

    ./density_vs_time.exe $startFileNum $endFileNum >> progress.txt &&

    echo "Density as a function of time generated in the folder in postProcessed/density" >> progress.txt

fi

##### Find Interface
if [ $OPT2 -eq 1 ]
then 
    ./findInterface.py 0 3920 10 &&
    echo "Interfacial position for the sake of plots is generated" 
fi

##### (2) Obtain meniscus velocity as a function of time : v(t) vs t 
##### Input : test/imbibitionFront.dat
##### Output : test/imbibitionVelocity.dat
if [ $OPT3 -eq 1 ]
then 
    python findMeniscus.py $startAnalysisFrom $endFileNum $startFileNum >> progress.txt &&

    echo "Shape of meniscus for different times is written to postProcessed/interface_vs_time.dat" >> progress.txt
fi

if [ $OPT4 -eq 1 ]
then 
    python vt_vs_t.py $startAnalysisFrom $endFileNum >> progress.txt &&

    echo "velocity of meniscus as a function of time is written to postProcessed/meniscusVelocity.dat" >> progress.txt
fi

## (3) Obtain the normalized velocity and density fields 
## using the meniscus position and velocity
## Input : ./data/pos{$t}.xyz , ./data/vel{$t}.xyz
## Output : ./test/velocityAnalysis/A/vrA{$t}.dat  , similarly for B
##        : ./test/velocityAnalysis/A/vzA{$t}.dat  , similarly for B
##        : ./test/velocityAnalysis/A/rhoA{$t}.dat , similarly for B

if [ $OPT5 -eq 1 ]
then 
    icpc -fast -o velocityProfile.exe velocityProfile.cpp >> progress.txt &&

    mkdir -p postProcessed/velocityAnalysis &&

    ./velocityProfile.exe $startAnalysisFrom $endFileNum >> progress.txt &&

    echo "velocity profiles generated" >> progress.txt 
fi

if [ $OPT6 -eq 1 ]
then 
    python quiverPlots.py $startAnalysisFrom $endFileNum >> progress.txt &&

    echo "quiverPlots hve been generated" >> progress.txt
fi

end=$(date +%s)

tottime=$(expr $end - $begin)

echo "execution time is $tottime seconds" >> progress.txt
