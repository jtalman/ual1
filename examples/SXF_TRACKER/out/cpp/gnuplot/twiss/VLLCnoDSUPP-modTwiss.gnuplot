# gnuplot > load "gnuplot/twiss/VLLCnoDSUPP-modTwiss.gnuplot"

 set xlabel "s (m)"
 set ylabel "beta_x (m), beta_y (m), eta_x(m)"
 # set xrange [0:14.2401]
 # set yrange [0:30]

 set grid
 set style data linespoints
 set timestamp "%d/%m/%y %H:%M" offset 80,1.4 font "Helvetica"
 set title "VLLCnoDSUPP-mod Lattice Functions"

# set terminal x11 enhanced font "Arial,15"
# set term postscript enhanced font "Arial,16" clip
set term pdfcairo enhanced font "Arial,12"
# set term png enhanced notransparent 
#  set output '| display png:-'
set output "pdf/VLLCnoDSUPP-modTwiss.pdf

 plot "VLLCnoDSUPP-mod.twiss" using 3:4 title "VLLCnoDSUPP-mod-beta_x",\
      "VLLCnoDSUPP-mod.twiss" using 3:9 title "VLLCnoDSUPP-mod-beta_y",\
      "VLLCnoDSUPP-mod.twiss" using 3:7 title "VLLCnoDSUPP-mod-eta_x"

set xrange [0:2000]
set output "pdf/IP-VLLCnoDSUPP-modTwiss.pdf
 plot "VLLCnoDSUPP-mod.twiss" using 3:4 title "VLLCnoDSUPP-mod-beta_x",\
      "VLLCnoDSUPP-mod.twiss" using 3:9 title "VLLCnoDSUPP-mod-beta_y",\
      "VLLCnoDSUPP-mod.twiss" using 3:7 title "VLLCnoDSUPP-mod-eta_x"
