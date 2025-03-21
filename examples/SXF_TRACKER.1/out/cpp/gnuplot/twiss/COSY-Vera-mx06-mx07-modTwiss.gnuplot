# gnuplot > load "gnuplot/twiss/COSY-Vera-mx06-mx07-modTwiss.gnuplot"

 set xlabel "s (m)"
 set ylabel "beta_x [m], beta_y [m]"
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

set label "deuteron,  E= 2.11 GeV,  {/Symbol b} =0.497" at 100, 28
set title "COSY-Vera-mx06-mx07-mod Beta Functions"
set arrow 1 from 63.22, 0.0 to 63.22, 3.857
set label "mx06=sf" at 60.22, 3.857 right
set arrow 2 from 69.54, 0.0 to 69.54, 8.906
set label "mx07=sd" at 72.54, 8.906 left
set output "pdf/COSY-Vera-mx06-mx07-modTwiss.pdf"
plot "ring.twiss" using 3:4 title "COSY-Vera-mx06-mx07-mod-beta_x",\
      "ring.twiss" using 3:8 title "COSY-Vera-mx06-mx07-mod-beta_y"

unset label
set label "deuteron,  E= 2.11 GeV,  {/Symbol b} =0.497" at 100, 25
set ylabel "D [m]"
set title "COSY-Vera-mx06-mx07-mod Dispersion Function"
set arrow 1 from 63.22, 0.0 to 63.22, 17.61
set label "mx06=sf" at 60.22, 17.61 right
set arrow 2 from 69.54, 0.0 to 69.54, 31.51
set label "mx07=sd" at 72.54, 31.51 left
set output "pdf/COSY-Vera-mx06-mx07-modDispersionFunction.pdf"
plot  "ring.twiss" using 3:7 title "COSY-Vera-mx06-mx07-mod-eta_x"
        




