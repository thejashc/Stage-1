#! gnuplot

# plotting the energy data
set terminal postscript eps enhanced color font 'Helvetica,10'
set output './plots/energy.eps' 

set key top left

plot './data/en_data.dat' u 1 w lp ps 0.45 t'pot',\
     './data/en_data.dat' u 2 w lp ps 0.45 t'kin',\
     './data/en_data.dat' u 3 w lp ps 0.45 t'tot',\

reset

# plotting the g(r) function
set terminal postscript eps enhanced color font 'Helvetica,10'
set output './plots/g_r.eps' 

set key top left

plot './data/gr_data.dat' u 1:2 w lp ps 0.45 t'radial distribution function',\

reset

# plotting the pressure and temperature
set terminal postscript eps enhanced color font 'Helvetica,10'
set output './plots/temperature.eps' 

set key top left

plot './data/eos_data.dat' u 2 w lp ps 0.45 t'temperature',\

reset
# temperature
set terminal postscript eps enhanced color font 'Helvetica,10'
set output './plots/pressure.eps' 

set key top left

plot './data/eos_data.dat' u 3 w lp ps 0.45 t'pressure',\

reset
# velocity distribution
set terminal postscript eps enhanced color font 'Helvetica,10'
set output './plots/velDist.eps' 

m=1.0
kb=1.0
MB(x, T)=sqrt(m/(2.0*pi*kb*T))*exp(-(0.5*m*x**2.0)/(kb*T))
set samples 1e4
set key top left

plot './data/velDist_data.dat' u ($1):($3/$4) w lp ps 0.45 t'x',\
     './data/velDist_data.dat' u ($1):($5/$6) w lp ps 0.45 t'y',\
     './data/velDist_data.dat' u ($1):($7/$8) w lp ps 0.45 t'z',\
     './data/velDist_data.dat' u 1:(MB($1, $2)) w l t'MB dist'
