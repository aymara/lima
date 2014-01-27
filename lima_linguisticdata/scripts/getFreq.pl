#!/usr/bin/perl -s
#   Copyright 2002-2013 CEA LIST
#    
#   This file is part of LIMA.
#
#   LIMA is free software: you can redistribute it and/or modify
#   it under the terms of the GNU Affero General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   LIMA is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU Affero General Public License for more details.
#
#   You should have received a copy of the GNU Affero General Public License
#   along with LIMA.  If not, see <http://www.gnu.org/licenses/>

##############################################################
# In order to calculate frequences for bigrams and trigrams
##############################################################

use strict;

my $BiFull=$ARGV[0];
my $TriFull=$ARGV[1];
my $Bi=$ARGV[2];
my $Tri=$ARGV[3];

open(BF,"$BiFull")||die "Can't read full bigram file!\n";
open(TF,"$TriFull")||die "Can't read full trigram file!\n";
open(B,">$Bi") ||die "Can't open output bigram file.\n";
open(T,">$Tri") ||die "Can't open output trigram file.\n";

my $freqB=0;
my $freqT=0;

my $nbB=0;
my $nbT=0;
my @tri;
my @bi;

my $preB=" ";
my $preT=" ";

while (<BF>){
	$nbB++;
	chomp $_;
	if ($_ eq $preB){
		$freqB++;
		$preB=$_;
	}
	else{
		#print B ($preB." ".$freqB."\n");
#        print $preB."\n";
		push (@bi,$preB."\/".$freqB);
		$preB=$_;
		$freqB=1;
	}
    if (eof(BF))
    {
		push (@bi,$preB."\/".$freqB);
    }
}

while (<TF>){
	$nbT++;
	chomp $_;
	if ($_ eq $preT){
		$freqT++;
		$preT=$_;
	}
	else{
		#print T ($preT." ".$freqT."\n");
		push (@tri, $preT."\/".$freqT);
		$preT=$_;
		$freqT=1;
	}
    if (eof(TF))
    {
		push (@tri, $preT."\/".$freqT);
    }
}

print "bigram : ".$nbB." lines\n";
print "trigram : ".$nbT." lines\n";

foreach my $b (@bi)
{
	my ($bigram,$freq)=(split/\//,$b)[0,1];
        if ($freq > 0) {
          print B ($bigram."	");
          printf B ( "%10.8f",$freq/$nbB);
          #print B ($freq);
          print B ("\n");
        }
}

foreach my $c (@tri)
{
	my ($trigram,$freq)=(split/\//,$c)[0,1];
        if ($freq > 0) {
          print T ($trigram."	");
          printf T ( "%10.8f",$freq/$nbT);
          print T ("\n");
        }
}

close(BF);
close(TF);
close(B);
close(T);
