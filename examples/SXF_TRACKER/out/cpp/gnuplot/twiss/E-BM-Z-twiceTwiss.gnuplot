# gnuplot > load "gnuplot/twiss/E-BM-Z-twiceTwiss.gnuplot"

 set xlabel "s (m)"
 set ylabel "beta_x (m), beta_y (m), eta_x(m)"
 # set xrange [0:14.2401]
 # set yrange [0:30]

 set grid
 set style data linespoints
 set timestamp "%d/%m/%y %H:%M" offset 80,1.4 font "Helvetica"
 set title "E-BM-Z-twice Lattice Functions"

# set terminal x11 enhanced font "Arial,15"
# set term postscript enhanced font "Arial,16" clip
set term pdfcairo enhanced font "Arial,12"
# set term png enhanced notransparent 
#  set output '| display png:-'
set output "pdf/E-BM-Z-twiceTwiss.pdf"

 plot "E-BM-Z-twice.twiss" using 3:4 title "E-BM-Z-twice-beta_x",\
      "E-BM-Z-twice.twiss" using 3:9 title "E_BM-Z-twice-beta_y",\
      "E-BM-Z-twice.twiss" using 3:7 title "E_BM-Z-twice-eta_x"