#!/bin/bash

destination='/storage3/usr/people/thejas/mDPD_parameterization/contactAngle/singleComponentDroplets/'
destination='/storage3/usr/people/thejas/mDPD_parameterization/contactAngle/binaryMixtureDroplets'
destination='/storage3/usr/people/thejas/imbibition/multiwetting/frozenParticleSimulations'
#scp -r {DPD.exe,plot_script.py,param.in} thejas@msm2.ctw.utwente.nl:/storage3/usr/people/thejas/Green_Kubo #filenames should not contain commas in between
#scp -r {DPD.exe,visualizeCapillary.tcl,param.in} thejas@msm2.ctw.utwente.nl:$destination #filenames should not contain commas in between
scp -r {DPD.exe,visualizeCapillary.tcl,param.in} thejas@msm2.ctw.utwente.nl:$destination #filenames should not contain commas in between
#scp -r {DPD.exe,param.in} thejas@msm2.ctw.utwente.nl:$destination/lowVisc_sigma_6_12/ #filenames should not contain commas in between

#scp -r {DPD.exe,param.in,readConfig/multipleColloids/Lx_20_Ly_20_Lz_50/*} thejas@msm2.ctw.utwente.nl:$destination/ #filenames should not contain commas in between
#scp -r visualizeCapillary.tcl thejas@msm2.ctw.utwente.nl:$destination/ #filenames should not contain commas in between
