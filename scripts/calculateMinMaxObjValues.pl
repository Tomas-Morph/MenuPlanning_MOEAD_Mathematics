#!/usr/bin/perl
use warnings;
use strict;

die "Incorrect number of arguments: fileName numberRuns stepSize finalStep numObj" if (@ARGV != 5);

my ($fileName, $numberRuns, $stepSize, $finalStep, $numObj) = @ARGV;
my (@minObjValues, @maxObjValues);

for (my $i = 0; $i < $numObj; $i++) {
  push @minObjValues, 99999;
  push @maxObjValues, -99999;
}

# For each input file
for my $currentRun (0..$numberRuns - 1) {
  my $currentStep = $stepSize;

  system "gzip -d $fileName.$currentRun.gz";
  open EVOF, "$fileName.$currentRun" or die "File $fileName.$currentRun cannot be opened";

  # Reads the input file and stores all the individuals belonging to every step into a dynamic data structure
  while (<EVOF>) {
    chomp;
    if ((m/Current Evaluation = (\d+)/) && ($1 == $currentStep) && ($1 <= $finalStep)) {
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

        for (my $i = 0; $i < $numObj; $i++) {
          $minObjValues[$i] = $objValues[$i] if ($objValues[$i] < $minObjValues[$i]);
          $maxObjValues[$i] = $objValues[$i] if ($objValues[$i] > $maxObjValues[$i]);
        }
      }
      $currentStep += $stepSize;
    }
  }
  close EVOF;
  system "gzip $fileName.$currentRun";
}

# Prints the min and max values for the different objective functions
for (my $i = 0; $i < $numObj; $i++) {
  print "Objective function $i:\n";
  print "Min value: $minObjValues[$i]; Max value: $maxObjValues[$i]\n";
}
