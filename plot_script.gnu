#! gnuplot

set terminal postscript eps enhanced color font 'Helvetica,10'
set output './plots/energy.eps' 

set key top left

plot './data/en_data.dat' u 1 w lp ps 0.15 t'pot',\
     './data/en_data.dat' u 2 w lp ps 0.15 t'kin',\
     './data/en_data.dat' u 3 w lp ps 0.15 t'tot'

