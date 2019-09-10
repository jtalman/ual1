# gnuplot > load "gnuplot/twiss/racetrackTwiss.gnuplot"

 set xlabel "s (m)"
 set ylabel "beta_x (m), beta_y (m), eta_x(m)"
 # set xrange [0:14.2401]
 # set yrange [0:30]

 set grid
 set style data linespoints
 set timestamp "%d/%m/%y %H:%M" offset 80,1.4 font "Helvetica"
 set title "Racetrack Lattice Functions"

# set terminal x11 enhanced font "Arial,15"
# set term postscript enhanced font "Arial,16" clip
set term pdfcairo enhanced font "Arial,12"
# set term png enhanced notransparent 
#  set output '| display png:-'
set output "pdf/racetrackTwiss.pdf

 plot "racetrack.twiss" using 3:4 title "racetrack-beta_x",\
      "racetrack.twiss" using 3:9 title "racetrack-beta_y",\
      "racetrack.twiss" using 3:7 title "racetrack-eta_x"