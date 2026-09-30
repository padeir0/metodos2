set terminal pngcairo size 800,600
set output 'conj.png'

set title "Convergência do método de gradiente conjugado"
set xlabel "precisão (|log_{10}(tol)|)"
set ylabel "número de iterações"

set grid

set key top left

plot 'conj.data' using 1:2 with linespoints title 'média', \
     ''          using 1:3 with linespoints title 'máximo'
