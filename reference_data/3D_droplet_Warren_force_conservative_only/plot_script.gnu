#! gnuplot

# plotting the energy data
set terminal postscript eps enhanced color font 'Helvetica,10'
set output './energy.eps' 

set key top left

plot './en_data.dat' u 1 w lp ps 0.45 t'pot',\
     './en_data.dat' u 2 w lp ps 0.45 t'kin',\
     './en_data.dat' u 3 w lp ps 0.45 t'tot',\
     
reset

# plotting the g(r) function
set terminal postscript eps enhanced color font 'Helvetica,10'
set output './g_r.eps'

# variables
shellVol(ri, ro) = (4.0/3.0)*pi*(ro**3.0 - ri**3.0)
nHomo(ri, ro, rho) = shellVol(ri, ro)*rho

set key top left

plot './gr_data.dat' u ($3):($4/($5*$6*nHomo($1, $2, $7))) w lp ps 0.45 t'radial distribution function',\


reset

# plotting the pressure and temperature
set terminal postscript eps enhanced color font 'Helvetica,10'
set output './temperature.eps' 

set key top left

plot './eos_data.dat' u 2 w lp ps 0.45 t'temperature',\

reset
# temperature
set terminal postscript eps enhanced color font 'Helvetica,10'
set output './pressure.eps' 

set key top left

plot './eos_data.dat' u 3 w lp ps 0.45 t'pressure',\

reset

# plotting the momentum
set terminal postscript eps enhanced color font 'Helvetica,10'
set output './momentum.eps'

set key top left

plot[][-1e-10:1e-10] './mom_data.dat' u 1 w lp ps 0.45 t'p_{x}',\
     './mom_data.dat' u 2 w lp ps 0.45 t'p_{y}',\
     './mom_data.dat' u 3 w lp ps 0.45 t'p_{z}',\

reset
# velocity distribution
set terminal postscript eps enhanced color font 'Helvetica,10'
set output './velDist.eps'

m=1.0
kb=1.0
MB(x, T)=sqrt(m/(2.0*pi*kb*T))*exp(-(0.5*m*x**2.0)/(kb*T))
set samples 1e4
set key top left

plot[-5:5] './velDist_data.dat' u ($1):($3/$4) w lp ps 0.45 t'x',\
     './velDist_data.dat' u ($1):($5/$6) w lp ps 0.45 t'y',\
     './velDist_data.dat' u ($1):($7/$8) w lp ps 0.45 t'z',\
     './velDist_data.dat' u 1:(MB($1, $2)) w l t'MB(T_{sim})',\
    
