#!/usr/bin/perl -s
# Extrait le second token de chaque ligne avec un blanc comme separateur
use strict "refs";

my $cut = shift @ARGV;
my $niemes = shift @ARGV;
my $sep = ";";
if ($osep)
{
  if ($osep eq "\\t")
  {
    $sep = "\t";
  }
  else
  {
    $sep = $osep;
  }
}

my @niemes = split ",", $niemes;
my $maxnieme = 0;

foreach my $nieme (@niemes)
{
	if ($nieme > $maxnieme) {$maxnieme = $nieme};
}

while (<>)
{
	chomp;
	if (/\#bqrdoc/) {<>;<>;<>;<>;<>;<>;}
  my @Fields = split($cut, $_, 9999);
  if ($#Fields+1 >= $maxnieme)
	{
		my $line = "";
		foreach my $nieme (@niemes)
			{$line = $line . "$Fields[$nieme-1]$sep";}
		chop $line;
		print "$line\n";
	}
}
