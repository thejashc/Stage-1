#! gnuplot

# plotting the energy data
set terminal postscript eps enhanced color font 'Helvetica,10'
set output './energy.eps' 

set key top left

plot './en_data.dat' u 1 w p ps 0.45 t'pot',\
     './en_data.dat' u 2 w p ps 0.45 t'kin',\
     './en_data.dat' u 3 w p ps 0.45 t'tot',\
     './cell_list_en_data.dat' u 1 w l t'pot- cell',\
     './cell_list_en_data.dat' u 2 w l t'kin- cell',\
     './cell_list_en_data.dat' u 3 w l t'tot- cell'

reset

# plotting the g(r) function
set terminal postscript eps enhanced color font 'Helvetica,10'
set output './g_r.eps' 

set key top left

plot './gr_data.dat' u 1:2 w p ps 0.45 t'radial distribution function',\
     './cell_list_gr_data.dat' u 1:2 w l t'g(r) - cell'

reset

# plotting the pressure and temperature
set terminal postscript eps enhanced color font 'Helvetica,10'
set output './temperature.eps' 

set key top left

plot './eos_data.dat' u 2 w p ps 0.45 t'temperature',\
     './cell_list_eos_data.dat' u 2 w l t'temp - cell'

reset
# temperature
set terminal postscript eps enhanced color font 'Helvetica,10'
set output './pressure.eps' 

set key top left

plot './eos_data.dat' u 3 w p ps 0.45 t'pressure',\
     './cell_list_eos_data.dat' u 3 w l t'pres - cell'

reset
