#!/usr/local/bin/perl

# perl perl/FFT-to-chromaticity-conversion.pl  COSY-Vera-detune-opposite-straights.data >! COSY-Vera-detune-opposite-straights.chrom

$, = ' ';		# set output field separator
$\ = "\n";		# set output record separator

$dlta = 0.0002178;
$xi_x_fac = 1;   # -1;  // This factor can be introduced later, in order to supply
$xi_y_fac = 1;   # -1;  // the "aliasing" correction for fractional tunes greater than 0.5.

while (<>) {
    chop;	# strip record separator
    $LineTemp = $_;
    @Fld = split(' ', $LineTemp, 9999);

    if($Fld[0] == "#") {
	print($LineTemp);
    } else {
	$sf= $Fld[0];
	$sd = $Fld[1];
	$freq_x_mi = $Fld[2];
	$freq_y_mi = $Fld[3];
	$freq_x_cent = $Fld[4];
	$freq_y_cent  = $Fld[5];
	$freq_x_pl  = $Fld[6];
	$freq_y_pl= $Fld[7];
    
	$xi_x_mi = ($freq_x_cent-$freq_x_mi)/$dlta*$xi_x_fac;
	$xi_x_pl = ($freq_x_pl-$freq_x_cent)/$dlta*$xi_x_fac;
	$xi_y_mi = ($freq_y_cent-$freq_y_mi)/$dlta*$xi_y_fac;
	$xi_y_pl = ($freq_y_pl-$freq_y_cent)/$dlta*$xi_y_fac;
 
	$xi_x = ($xi_x_pl +$xi_x_mi)/2;
	$xi_y = ($xi_y_pl +$xi_y_mi)/2;

	printf("%.6f  %.6f   %.4f    %.4f        %.4f    %.4f        %.4f    %.4f\n",  $sf, $sd, $xi_x_mi, $xi_x_pl , $xi_y_mi, $xi_y_pl, $xi_x, $xi_y);
    }
}