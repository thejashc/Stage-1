#!/bin/bash

destination='/storage3/usr/people/thejas/imbibition/multiwetting/parameterScan'
destination='/storage3/usr/people/thejas/imbibition/multiwetting/capRad_5/Asl_m_25_40/capLen_140_capRad_5_resWdth_50_Lx_Ly_20_dt_0_01_1'
destination='/storage3/usr/people/thejas/imbibition/multiwetting/highWethighVisc_lowWetlowVisc'

#scp -r {DPD.exe,visualizeCapillary.tcl,param.in} thejas@msm2.ctw.utwente.nl:$destination #filenames should not contain commas in between
#scp -r restartDPD.exe thejas@msm2.ctw.utwente.nl:$destination #filenames should not contain commas in between
#scp -r readConfig/ thejas@msm2.ctw.utwente.nl:$destination #filenames should not contain commas in between

#scp -r -i "~/.ssh/wouter" -P 46172 fluidInCylinder.h thejas@snowwhite.tnw.utwente.nl:/home3/thejas/src/
scp -r -i "~/.ssh/wouter" -P 46172 thejas@snowwhite.tnw.utwente.nl:/home3/thejas/evaporation/wet/Asl_12/dataVisualize/* ./dataVisualize/
