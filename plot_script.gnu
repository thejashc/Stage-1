#! gnuplot

# plotting the energy data
set terminal postscript eps enhanced color font 'Helvetica,10'
set output './plots/energy.eps' 

set key top left

plot './data/en_data.dat' u 1 w lp ps 0.15 t'pot',\
     './data/en_data.dat' u 2 w lp ps 0.15 t'kin',\
     './data/en_data.dat' u 3 w lp ps 0.15 t'tot'

reset

# plotting the g(r) function
set terminal postscript eps enhanced color font 'Helvetica,10'
set output './plots/g_r.eps' 

set key top left

plot './data/gr_data.dat' u 1:2 w lp ps 0.15 t'radial distribution function'

reset

# plotting the pressure and temperature
set terminal postscript eps enhanced color font 'Helvetica,10'
set output './plots/temperature.eps' 

set key top left

plot './data/eos_data.dat' u 2 w lp ps 0.15 t'temperature'

reset
# temperature
set terminal postscript eps enhanced color font 'Helvetica,10'
set output './plots/pressure.eps' 

set key top left

plot './data/eos_data.dat' u 3 w lp ps 0.15 t'pressure'

reset
