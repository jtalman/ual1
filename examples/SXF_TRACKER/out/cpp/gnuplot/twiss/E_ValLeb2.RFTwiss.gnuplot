# gnuplot > load "gnuplot/twiss/E_ValLeb2.RFTwiss.gnuplot"

 set xlabel "s (m)"
 set ylabel "beta_x (m), beta_y (m), eta_x(m)"
 # set xrange [0:14.2401]
 # set yrange [0:30]

 set grid
 set style data linespoints
 set timestamp "%d/%m/%y %H:%M" offset 80,1.4 font "Helvetica"
 set title "E_ValLeb2.RF Lattice Functions"

# set terminal x11 enhanced font "Arial,15"
# set term postscript enhanced font "Arial,16" clip
set term pdfcairo font "Arial,15"
# set term png enhanced notransparent 
#  set output '| display png:-'
set output "pdf/E_ValLeb2.RFTwiss.pdf"

 plot "E_ValLeb2.RF.twiss" using 3:4 title "E_ValLeb2.RF-beta_x",\
      "E_ValLeb2.RF.twiss" using 3:9 title "E_ValLeb2.RF-beta_y",\
      "E_ValLeb2.RF.twiss" using 3:7 title "E_ValLeb2.RF-eta_x"