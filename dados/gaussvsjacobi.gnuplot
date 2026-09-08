set terminal pngcairo size 800,600
set output 'gaussvsjacobi.png'

set title "Gauss c/ pivotamento vs. Jacobi"
set xlabel "ordem do sistema"
set ylabel "tempo (s)"

set logscale xy
set grid

set key top left

plot 'gauss_vs_jacobi.data' using 1:2 with linespoints title 'jacobi', \
     ''                     using 1:3 with linespoints title 'gauss c/ pivotamento'
