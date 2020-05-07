#!/usr/bin/perl
use strict;
use warnings;

my $algorithm = shift || die "Execution: ./sortByHVValue algorithm";
my $hashValues;

foreach my $fileName (`ls *${algorithm}*.avgHV.evolution`) {
  chomp $fileName;
  open(my $fileHandler, "<", $fileName) || die "Can't open < $fileName: $!";

  my $maxValue = 0;
  foreach my $line (<$fileHandler>) {
    chomp $line;
    if ($line =~ m/(\d+\.\d+)/) {
      $maxValue = $1 if ($1 > $maxValue);
    }
  }
  $hashValues->{$fileName} = $maxValue;
}

my $num = 1;
for my $key ( sort { $hashValues->{$a} <=> $hashValues->{$b} } keys %$hashValues ) {
    print "$num - $key - $hashValues->{$key}\n";
    $num++;
}
