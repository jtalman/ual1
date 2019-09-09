# gnuplot > load "gnuplot/twiss/E_BM_ZTwiss.gnuplot"

 set xlabel "s (m)"
 set ylabel "beta_x (m), beta_y (m), eta_x(m)"
 # set xrange [0:14.2401]
 # set yrange [0:30]

 set grid
 set style data linespoints
 set timestamp "%d/%m/%y %H:%M" offset 80,1.4 font "Helvetica"
 set title "E_BM_Z Lattice Functions"

# set terminal x11 enhanced font "Arial,15"
# set term postscript enhanced font "Arial,16" clip
set term pdfcairo enhanced font "Arial,12"
# set term png enhanced notransparent 
#  set output '| display png:-'
set output "pdf/E_BM_ZTwiss.pdf"

 plot "E_BM_Z.twiss" using 3:4 title "E_BM_Z-beta_x",\
      "E_BM_Z.twiss" using 3:9 title "E_BM_Z-beta_y",\
      "E_BM_Z.twiss" using 3:7 title "E_BM_Z-eta_x"