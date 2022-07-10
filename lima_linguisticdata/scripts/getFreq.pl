#!/usr/bin/perl -s

# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

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
