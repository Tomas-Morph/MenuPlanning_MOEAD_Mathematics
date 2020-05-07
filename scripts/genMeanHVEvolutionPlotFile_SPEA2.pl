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
set title  "SPEA2 - 4e8 evals. - 10 runs" font "Helvetica-Bold,22"
set term postscript eps enhanced color solid "Helvetica,20"

set xtics font "Helvetica,20"
#set xtics 0, 6000, 41400 
set ytics font "Helvetica,20"

set output "meanHV_Evolution_SPEA2.eps"

plot "Bolido_Sequential_MenuPlanning_EvoComp_SPEA2_PopSize_100_ArchSize_100_pm_0.05_pc_0.8_days_20_MenuPlanning_400000000.avgHV.evolution" with linespoints lt -1 pt 7 ps 1.5 lc rgb 'blue' title "SPEA2-ps-100-pm-0.05-pc-0.8", "Bolido_Sequential_MenuPlanning_EvoComp_SPEA2_PopSize_50_ArchSize_50_pm_0.15_pc_0.8_days_20_MenuPlanning_400000000.avgHV.evolution" with linespoints lt -1 pt 7 ps 1.5 lc rgb "red" title "SPEA2-ps-50-pm-0.15-pc-0.8", "Bolido_Sequential_MenuPlanning_EvoComp_SPEA2_PopSize_100_ArchSize_100_pm_0.05_pc_1.0_days_20_MenuPlanning_400000000.avgHV.evolution" with linespoints lt -1 pt 7 ps 1.5 lc rgb 'black' title "SPEA2-ps-100-pm-0.05-pc-1.0", "Bolido_Sequential_MenuPlanning_EvoComp_SPEA2_PopSize_250_ArchSize_250_pm_0.05_pc_1.0_days_20_MenuPlanning_400000000.avgHV.evolution" with linespoints lt -1 pt 7 ps 1.5 lc rgb 'green' title "SPEA2-ps-250-pm-0.05-pc-1.0", "Bolido_Sequential_MenuPlanning_EvoComp_SPEA2_PopSize_25_ArchSize_25_pm_0.15_pc_0.8_days_20_MenuPlanning_400000000.avgHV.evolution" with linespoints lt -1 pt 7 ps 1.5 lc rgb 'yellow' title "SPEA2-ps-25-pm-0.15-pc-0.8"};

open FILE, "> meanHV_Evolution_SPEA2.plot";
print FILE $script;
close FILE;
