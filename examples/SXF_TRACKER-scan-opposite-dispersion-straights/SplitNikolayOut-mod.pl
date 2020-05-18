#!/usr/local/bin/perl

# Resort all lines in "./out/cpp/ring.orbit" into groups with all turns of particle 1
# then two blank lines (the required gnuplot data separator) then all turns for
# particle 1 then two blanks, and so on. 

# For bunches with other than 3 particles change "$NUMPARTICLES".

# perl SplitNikolayOut-mod.pl  NikolayOut  

$, = ' ';		# set output field separator
$\ = "\n";		# set output record separator

$NUMPARTICLES = 3;
while (<>) {
    chop;	# strip record separator
    $LineTemp = $_;
    @Fld = split(' ', $LineTemp, 9999);
    $ParamsByPartNum[$Fld[0]] .= $LineTemp . "\n";
}

open(I_XMG_0, ">I_XMG_0") || die "Could not create I_XMG_0";
open(I_XMG_1, ">I_XMG_1") || die "Could not create I_XMG_1";
open(I_XMG_2, ">I_XMG_2") || die "Could not create I_XMG_2";

print I_XMG_0   $ParamsByPartNum[0] . "\n";
print I_XMG_1   $ParamsByPartNum[1] . "\n";
print I_XMG_2  $ParamsByPartNum[2] . "\n";


