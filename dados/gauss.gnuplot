set terminal pngcairo size 800,600
set output 'gauss_times.png'

set title "Gauss ingênuo vs. c/ pivotamento"
set xlabel "ordem do sistema"
set ylabel "tempo (s)"

set logscale xy
set grid

set key top left

plot 'gaussComparison.data' using 1:2 with linespoints title 'ingênuo', \
     ''                     using 1:3 with linespoints title 'c/ pivotamento'
