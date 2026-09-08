set terminal pngcairo size 800,600
set output 'jacobi.png'

set title "Convergência do método de Jacobi"
set xlabel "precisão (|log_{10}(tol)|)"
set ylabel "número de iterações"

set grid

set key top left

plot 'jacobi.data' using 1:2 with linespoints title ""
