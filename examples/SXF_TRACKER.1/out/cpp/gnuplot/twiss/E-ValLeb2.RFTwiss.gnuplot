# gnuplot > load "gnuplot/twiss/E-ValLeb2.RFTwiss.gnuplot"

 set xlabel "s (m)"
 set ylabel "beta_x (m), beta_y (m), eta_x(m)"
 # set xrange [0:14.2401]
 # set yrange [0:30]

 set grid
 set style data linespoints
 set timestamp "%d/%m/%y %H:%M" offset 0,1.4 font "Helvetica"
 set title "E-ValLeb2.RF Lattice Functions"

# set terminal x11 enhanced font "Arial,15"
# set term postscript enhanced font "Arial,16" clip
set term pdfcairo enhanced font "Arial,15"
# set term png enhanced notransparent 
#  set output '| display png:-'
set output "pdf/E-ValLeb2.RFTwiss.pdf"

 plot "ring.twiss" using 3:4 title "E-ValLeb2.RF-beta_x",\
       "ring.twiss" using 3:8 title "E-ValLeb2.RF-beta_y",\
       "ring.twiss" using 3:7 title "E-ValLeb2.RF-eta_x"