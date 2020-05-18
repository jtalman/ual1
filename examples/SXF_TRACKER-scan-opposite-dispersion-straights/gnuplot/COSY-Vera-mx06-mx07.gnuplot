# gnuplot > load "gnuplot/COSY-Vera-mx06-mx07.gnuplot"

 # set xrange [0:14.2401]
 # set yrange [0:30]

 set grid
 set style data linespoints
 set timestamp "%d/%m/%y %H:%M" offset 0,1.4 font "Helvetica"

# set terminal x11 enhanced font "Arial,15"
# set term postscript enhanced font "Arial,16" clip
set term pdfcairo enhanced font "Arial,12"
# set term png enhanced notransparent 
#  set output '| display png:-'

set label "deuteron,  E= 2.11 GeV,  {/Symbol b} =0.459" at 0.2, 100

set xlabel "(fractional) tune Q_x"
 set ylabel "FFT(Q_x)"
set title "COSY-Vera-mx06-mx07 off-momenum horizontal tune spectrum"
set output "pdf/COSY-Vera-mx06-mx07-m0.005-fft-x.pdf"
plot "del_m0.005_x.fft" using 1:2 title "COSY-Vera-mx06-mx07-fft-x"

set xlabel "(fractional) tune Q_y"
 set ylabel "FFT(Q_y)"

set title "COSY-Vera-mx06-mx07 off-momenum vertical tune spectrum"
set title "COSY-Vera-mx06-mx07 off-momenum vertical tune spectrum"
set output "pdf/COSY-Vera-mx06-mx07-m0.005-fft-y.pdf"
plot "del_m0.005_y.fft" using 1:2 title "COSY-Vera-mx06-mx07-fft-y"


