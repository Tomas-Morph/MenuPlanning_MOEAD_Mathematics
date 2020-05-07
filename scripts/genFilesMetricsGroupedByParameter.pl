#!/usr/bin/perl
use warnings;
use strict;
use List::MoreUtils qw(uniq);

sub getStats {
  my $algorithm = shift;
  my $statistics = {};

  foreach my $fileName (`ls *${algorithm}*.allHV`) {
    chomp($fileName);

    my $scriptR = "data1 = scan(\\\"$fileName\\\"); summary(data1)\n";
    my @result = `echo "$scriptR" | R --silent --no-save`;

    chomp($result[2]);
    my $data = [ split /\s+/, $result[2] ];
    shift @$data;

    $statistics->{$fileName} = $data;
  }
  return $statistics;
}

sub genFilesMetricsGroupedByParameter {
  my ($algorithm, $statHash, $paramValues, $varParam, $field) = @_;

  my $groupFileNames;

  foreach my $paramValue (@{$paramValues->{$varParam}}) {
    foreach my $fileName (`ls *${algorithm}*_${paramValue}_*.allHV`) {
      chomp($fileName);
      my $valueToPrint = $statHash->{$fileName}[$field];

      $fileName =~ s/allHV/group/g;
      $fileName =~ s/${varParam}_${paramValue}_//g;
      $fileName =~ s/ArchSize_\d+_//g;

      open (my $fileHandler, '>>', $fileName) or die "File $fileName could not be opened: $!";
      print $fileHandler "$paramValue $valueToPrint\n";
      close $fileHandler;
      push @$groupFileNames, $fileName;
    }
  }
 
  my $uniqGroupFileNames = [uniq @$groupFileNames];
  return $uniqGroupFileNames;
}

sub genPlotFile {
  my ($algorithm, $paramValues, $varParam, $groupFileNames) = @_;

  my $xtics = "(";
  foreach my $xtic (@{$paramValues->{$varParam}}) {
    $xtics .= "$xtic, ";
  }
  $xtics .= ")";

  my $script = qq{set bmargin 4

set key outside right center

set xlabel "${varParam}" font "Helvetica,22"
set ylabel "Mean hypervolume" font "Helvetica,22"
set title  "${algorithm} - 4e8 evals. - 10 runs" font "Helvetica-Bold,22"
set term postscript eps enhanced color solid "Helvetica,20"

set xtics font "Helvetica,20"
set xtics $xtics
set ytics font "Helvetica,20"

set output "meanHV_grouped_by_${varParam}_${algorithm}.eps"

plot };

  foreach my $groupFileName (@$groupFileNames) {
    $groupFileName =~ m/_${algorithm}_(.+)_days/;
    my $title = $1;
    $title =~ s/\_/-/g;
    $script .= qq{"$groupFileName" with linespoints title "$title", };
  }
 
  my $plotFileName = "meanHV_grouped_by_${varParam}_${algorithm}.plot";
  open FILE, "> $plotFileName" or die "File $plotFileName could not be opened: $!";
  print FILE $script;
  close FILE;
}

my $algorithm = shift || die "You must specify one of the following algorithms: NSGA2, SPEA2, IBEA";
my $varParam = shift || die "You must specify one of the following parameters: pm, pc, PopSize";

my $paramValues = { pm => ['0.05', '0.1', '0.15'], pc => ['0.8', '0.9', '1.0'], PopSize => ['25', '50', '100', '250'] };
my $statistics = getStats $algorithm;
my $groupFileNames = genFilesMetricsGroupedByParameter $algorithm, $statistics, $paramValues, $varParam, 3;
genPlotFile $algorithm, $paramValues, $varParam, $groupFileNames;
