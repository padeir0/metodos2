set terminal pngcairo size 800,600
set output 'conjtime.png'

set title "Tempo de execução do método de gradiente conjugado"
set xlabel "precisão (|log_{10}(tol)|)"
set ylabel "tempo (ms)"

set grid

set key top left

plot 'conj.data' using 1:($4*1000) with linespoints title ''
