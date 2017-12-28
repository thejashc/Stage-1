#! gnuplot

# plotting the energy data
set terminal postscript eps enhanced color font 'Helvetica,10'
set output './energy.eps' 

set grid 
set key top left

plot './kbT_1_en_data.dat' u 1 w lp ps 0.45 t'pot, T=1',\
     './kbT_1_en_data.dat' u 2 w lp ps 0.45 t'kin, T=1',\
     './kbT_1_en_data.dat' u 3 w lp ps 0.45 t'tot, T=1',\
     './kbT_3_en_data.dat' u 1 w lp ps 0.45 t'pot, T=3',\
     './kbT_3_en_data.dat' u 2 w lp ps 0.45 t'kin, T=3',\
     './kbT_3_en_data.dat' u 3 w lp ps 0.45 t'tot, T=3',\

reset

# plotting the g(r) function
set terminal postscript eps enhanced color font 'Helvetica,10'
set output './g_r.eps' 

set grid 
# variables
shellVol(ri, ro) = (4.0/3.0)*pi*(ro**3.0 - ri**3.0)
nHomo(ri, ro, rho) = shellVol(ri, ro)*rho

set key top left

plot './kbT_1_gr_data.dat' u ($3):($4/($5*$6*nHomo($1, $2, $7))) w lp ps 0.45 t'T=1',\
     './kbT_3_gr_data.dat' u ($3):($4/($5*$6*nHomo($1, $2, $7))) w lp ps 0.45 t'T=3',\

reset

# plotting the pressure and temperature
set terminal postscript eps enhanced color font 'Helvetica,10'
set output './temperature.eps' 

set grid 
set key top left

plot './kbT_1_eos_data.dat' u 2 w lp ps 0.45 t'T=1',\
     './kbT_3_eos_data.dat' u 2 w lp ps 0.45 t'T=3',\

reset

# plotting the x,y,z momentum
set terminal postscript eps enhanced color font 'Helvetica,10'
set output './momentum.eps' 

set grid 
set key top left

plot[][-1e-10:1e-10] './kbT_1_mom_data.dat' u 1 w lp ps 0.45 t'p_{x}',\
     './kbT_1_mom_data.dat' u 2 w lp ps 0.45 t'p_{y}',\
     './kbT_1_mom_data.dat' u 3 w lp ps 0.45 t'p_{z}',\
     './kbT_3_mom_data.dat' u 1 w lp ps 0.45 t'p_{x}, T=3',\
     './kbT_3_mom_data.dat' u 2 w lp ps 0.45 t'p_{y}, T=3',\
     './kbT_3_mom_data.dat' u 3 w lp ps 0.45 t'p_{z}, T=3',\

# velocity distribution
set terminal postscript eps enhanced color font 'Helvetica,10'
set output './velDist.eps' 

set grid 
m=1.0
kb=1.0
Tset=1.0
MB(x, T)=sqrt(m/(2.0*pi*kb*T))*exp(-(0.5*m*x**2.0)/(kb*T))
set samples 1e4
set key top left

plot './kbT_1_velDist_data.dat' u ($1):($3/$4) w lp ps 0.45 t'x, T=1',\
     './kbT_1_velDist_data.dat' u ($1):($5/$6) w lp ps 0.45 t'y',\
     './kbT_1_velDist_data.dat' u ($1):($7/$8) w lp ps 0.45 t'z',\
     './kbT_1_velDist_data.dat' u 1:(MB($1, $2)) w l t'MB(T_{sim})',\
     './kbT_1_velDist_data.dat' u 1:(MB($1, Tset)) w l t'MB(T_{set})',\
     './kbT_3_velDist_data.dat' u ($1):($3/$4) w lp ps 0.45 t'x, T=3',\
     './kbT_3_velDist_data.dat' u ($1):($5/$6) w lp ps 0.45 t'y',\
     './kbT_3_velDist_data.dat' u ($1):($7/$8) w lp ps 0.45 t'z',\
     './kbT_3_velDist_data.dat' u 1:(MB($1, $2)) w l t'MB(T_{sim})',\
     './kbT_3_velDist_data.dat' u 1:(MB($1, 3.0)) w l t'MB(T_{set})'


reset
