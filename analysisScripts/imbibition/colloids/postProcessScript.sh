#!/bin/bash -f

startFile=76000
endFile=589400
itrvl=1

./z_vs_t.exe $startFile $endFile $itrvl &
P1=$!

startFile=589600
endFile=1190000
itrvl=2

./z_vs_t.exe $startFile $endFile $itrvl &
P2=$!

wait $P1 $P2

cat postProcessed/z_vs_t_*.dat > postProcessed/z_vs_t.dat
