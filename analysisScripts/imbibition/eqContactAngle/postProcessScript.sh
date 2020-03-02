#!/bin/bash -f

begin=$(date +%s)
### This script is used to run python scripts to 
### generate velocity vector fields.
#
#### (0) convert the binary to ascii data files
startFileNum=100000
endFileNum=1000000
diffFileNum=500

OPT1=1
OPT2=1
OPT3=1
OPT4=1

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
    icpc -fast -o find_com.exe find_com.cpp >> progress.txt &&

    mkdir -p postProcessed/density &&

    ./find_com.exe $startFileNum $endFileNum >> progress.txt &&

    echo "Center of mass as a function of time extracted" >> progress.txt
fi

##### Extract Density Maps
if [ $OPT2 -eq 1 ]
then 
    icpc -fast -o density_vs_time.exe density_vs_time.cpp >> progress.txt &&

    ./density_vs_time.exe $startFileNum $endFileNum >> progress.txt &&

    echo "Density as a function of time generated in the folder in postProcessed/density" >> progress.txt

fi

##### Extract Interface shape
if [ $OPT3 -eq 1 ]
then 
    ./extractInterface.py $startFileNum $endFileNum $diffFileNum &&
    echo "shape of interface for different times extracted" 
fi

end=$(date +%s)

tottime=$(expr $end - $begin)

echo "execution time is $tottime seconds" >> progress.txt

##### Find CA
if [ $OPT4 -eq 1 ]
then 
    python -W ignore findCA.py $startFileNum $endFileNum $diffFileNum &&
    echo "contact angle for various times was obtained" 
fi

end=$(date +%s)

tottime=$(expr $end - $begin)

echo "execution time is $tottime seconds" >> progress.txt
