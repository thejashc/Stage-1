#!/bin/bash

#destination='/storage3/usr/people/thejas/imbibition/multiviscosity/particleparticle'
destination='/storage3/usr/people/thejas/modelColloids2'
destination='/storage3/usr/people/thejas/imbibition/multiwetting/Asl_m_35_40'
#scp -r {DPD.exe,plot_script.py,param.in} thejas@msm2.ctw.utwente.nl:/storage3/usr/people/thejas/Green_Kubo #filenames should not contain commas in between
#scp -r {DPD.exe,visualizeCapillary.tcl,param.in} thejas@msm2.ctw.utwente.nl:/storage3/usr/people/thejas/squareCapillary #filenames should not contain commas in between
#scp -r {DPD.exe,param.in} thejas@msm2.ctw.utwente.nl:$destination/lowVisc_sigma_6_12/ #filenames should not contain commas in between

scp -r {DPD.exe,param.in} thejas@msm2.ctw.utwente.nl:$destination/ #filenames should not contain commas in between
