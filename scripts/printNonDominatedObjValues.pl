#!/usr/bin/perl
use warnings;
use strict;

die "Incorrect number of arguments: fileName finalStep numObj" if (@ARGV != 3);

my ($fileName, $finalStep, $numObj) = @ARGV;

my $front = [];

system "gzip -d $fileName.gz";
open EVOF, "$fileName" or die "File $fileName cannot be opened";

# Reads the input file and stores all the individuals belonging to every step into a dynamic data structure
while (<EVOF>) {
  chomp;
  if ((m/Current Evaluation = (\d+)/) && ($1 == $finalStep)) {
    my $frontSizeLine = <EVOF>;
    chomp $frontSizeLine;
    $frontSizeLine =~ m/Front Size = (\d+)/;

    my $frontSize = $1;
    
    for (my $i = 0; $i < $frontSize; $i++) {
      my $individual = <EVOF>;
      chomp $individual;
     
      # This is the individual. We must discard the values for the decision variables
      # and keep those values belonging to the objective functions
      my @individual = split / /, $individual;
      my @objValues = @individual[@individual - $numObj .. @individual - 1];

      push @$front, \@objValues if (!dominated($front, \@objValues));
    }
    $finalStep += $finalStep;
  }
}

close EVOF;
system "gzip $fileName";

printFront($front);

sub printFront {
  my $front = shift;
  foreach my $ind (@$front) {
    print "@$ind\n";
  }
}

sub dominated {
  my ($front, $individual) = @_;
  my $isDominated = 0;

  foreach my $ind (@$front) {
    if (($ind->[0] <= $individual->[0]) && ($ind->[1] < $individual->[1])) {
      $isDominated = 1;
    }
    if (($ind->[0] < $individual->[0]) && ($ind->[1] <= $individual->[1])) {
      $isDominated = 1;
    }
  }
  return $isDominated;
}
