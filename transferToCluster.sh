#!/bin/bash

destination='/storage3/usr/people/thejas/mDPD_parameterization/contactAngle/colloidalDroplets/'

#scp -r {DPD.exe,visualizeCapillary.tcl,param.in} thejas@msm2.ctw.utwente.nl:$destination #filenames should not contain commas in between
scp -r DPD.exe thejas@msm2.ctw.utwente.nl:$destination #filenames should not contain commas in between
#scp -r readConfig/ thejas@msm2.ctw.utwente.nl:$destination #filenames should not contain commas in between
