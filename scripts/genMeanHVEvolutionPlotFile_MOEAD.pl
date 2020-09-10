#!/usr/bin/perl
use warnings;
use strict;

my $script = qq{set bmargin 4
#set xrange [0:362]
set yrange [0.79:0.88]

#set format y "%g"
#set format x "%g"

set key bottom right
#set key spacing 1.5

set xlabel "Function evaluations" font "Helvetica,22"
set ylabel "Mean hypervolume" font "Helvetica,22"
set title  "MOEA/D - 1h. - 30 runs for 20-Days MPP" font "Helvetica-Bold,22"
set term postscript eps enhanced color solid "Helvetica,20"

set xtics font "Helvetica,20"
#set xtics 0, 6000, 41400 
set ytics font "Helvetica,20"

set output "meanHV_Evolution_MOEAD_20-Days.eps"

plot "Centella_Sequential_MenuPlanning_Mathematics_MainStudy_MOEAD_MPP_Evals_3600_NumSubProblems_15_NeighSize_5_pm_0.0_pc_1.0_MenuPlanning_20_days.avgHV.evolution" with linespoints lt -1 pt 7 ps 1.5 lc rgb 'blue' title "MOEAD"};

open FILE, "> meanHV_Evolution_MOEAD_20-Days.plot";
print FILE $script;
close FILE;
