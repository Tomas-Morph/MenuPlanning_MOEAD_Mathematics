#!/usr/bin/perl
use warnings;
use strict;

my $script = qq{set bmargin 4
#set xrange [0:362]
#set yrange [650:950]

#set format y "%g"
#set format x "%g"

set key bottom right
#set key spacing 1.5

set xlabel "Function evaluations" font "Helvetica,22"
set ylabel "Mean hypervolume" font "Helvetica,22"
set title  "IBEA - 4e8 evals. - 10 runs" font "Helvetica-Bold,22"
set term postscript eps enhanced color solid "Helvetica,20"

set xtics font "Helvetica,20"
#set xtics 0, 6000, 41400 
set ytics font "Helvetica,20"

set output "meanHV_Evolution_IBEA.eps"

plot "Bolido_Sequential_MenuPlanning_EvoComp_IBEA_Adapt_PopSize_250_pm_0.1_pc_0.8_sFactor_0.002_days_20_MenuPlanning_400000000.avgHV.evolution" with linespoints lt -1 pt 7 ps 1.5 lc rgb 'blue' title "IBEA-ps-250-pm-0.1-pc-0.8", "Bolido_Sequential_MenuPlanning_EvoComp_IBEA_Adapt_PopSize_250_pm_0.1_pc_1.0_sFactor_0.002_days_20_MenuPlanning_400000000.avgHV.evolution" with linespoints lt -1 pt 7 ps 1.5 lc rgb "red" title "IBEA-ps-250-pm-0.1-pc-1.0", "Bolido_Sequential_MenuPlanning_EvoComp_IBEA_Adapt_PopSize_50_pm_0.1_pc_0.8_sFactor_0.002_days_20_MenuPlanning_400000000.avgHV.evolution" with linespoints lt -1 pt 7 ps 1.5 lc rgb 'black' title "IBEA-ps-50-pm-0.1-pc-0.8", "Bolido_Sequential_MenuPlanning_EvoComp_IBEA_Adapt_PopSize_50_pm_0.15_pc_1.0_sFactor_0.002_days_20_MenuPlanning_400000000.avgHV.evolution" with linespoints lt -1 pt 7 ps 1.5 lc rgb 'green' title "IBEA-ps-50-pm-0.15-pc-1.0", "Bolido_Sequential_MenuPlanning_EvoComp_IBEA_Adapt_PopSize_25_pm_0.1_pc_1.0_sFactor_0.002_days_20_MenuPlanning_400000000.avgHV.evolution" with linespoints lt -1 pt 7 ps 1.5 lc rgb 'yellow' title "IBEA-ps-25-pm-0.1-pc-1.0"};

open FILE, "> meanHV_Evolution_IBEA.plot";
print FILE $script;
close FILE;
