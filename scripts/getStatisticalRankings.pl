#!/usr/bin/perl
use warnings;
use strict;
use Data::Dumper;

my $algorithm = shift || die "You must specify an algorithm [NSGA2, SPEA2, IBEA]";
my $numberRuns = shift || die "You must specify the number of runs you performed";

my @fileNames = `ls *${algorithm}*.allHV`;
my $rankings = {};

for (my $i = 0; $i < @fileNames; $i++) {
  for (my $j = $i + 1; $j < @fileNames; $j++) {
    my $file_A = $fileNames[$i];
    my $file_B = $fileNames[$j];
    chomp $file_A;
    chomp $file_B;
      
    if (!defined($rankings->{$file_A})) {
        $rankings->{$file_A} = [0, 0, 0, 0];
    }
    if (!defined($rankings->{$file_B})) {
        $rankings->{$file_B} = [0, 0, 0, 0];
    }

    my $result = `./statisticalTests_old.pl $numberRuns $file_A $file_B`;

    if ($result =~ /Mejor: $file_A/) {
      $rankings->{$file_A}[0]++;
      $rankings->{$file_B}[1]++;
      $rankings->{$file_A}[3]++;
      $rankings->{$file_B}[3]--;
    }
    elsif ($result =~ /Mejor: $file_B/) {
      $rankings->{$file_B}[0]++;
      $rankings->{$file_A}[1]++;
      $rankings->{$file_B}[3]++;
      $rankings->{$file_A}[3]--;
    }
    elsif ($result =~ /No existe diferencia estadistica/) {
      $rankings->{$file_B}[2]++;
      $rankings->{$file_A}[2]++;
    }
    elsif ($result =~ /OJO/) {
      print "Warning: $file_A, $file_B";
    }
  }
}

#$Data::Dumper::Sortkeys = 1;
#print Dumper($rankings);

$Data::Dumper::Sortkeys = sub { [ sort { $_[0]->{$b}[3] <=> $_[0]->{$a}[3] } keys %{$_[0]} ] };
print Dumper($rankings);
