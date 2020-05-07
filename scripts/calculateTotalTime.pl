#!/usr/bin/perl
use warnings;
use strict;

my $fileName = shift || die "Please, specify one filename";

open FILE, $fileName;

my $totalTime = 0;

foreach my $time (<FILE>) {
  chomp($time);
  $totalTime += $time;
}

close FILE;
print "Total time (seconds): $totalTime\n";
$totalTime /= 3600.0;
print "Total time (hours): $totalTime\n";
