#!/usr/bin/perl

# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

use strict;
use utf8;
binmode STDOUT, ":utf8";

my($dicFile,$codeTableFile,$outFile) = @ARGV;

if ($#ARGV != 2)
{
	print "usage : convstd.pl dicFile codeTableFile outFile\n";
	exit(1);
}

open(DICO, "<:utf8", $dicFile) || die "Cannot open $dicFile\n";
open(CODE, "<:utf8", $codeTableFile) || die "Cannot open $codeTableFile\n";
open (OUTPUT, "+>:utf8", $outFile) || die "Cannot create $outFile\n";

my $debug = 1;
my $infileLineNum = 0;
while (<DICO>)
{
	$infileLineNum++;
	if ($debug && $infileLineNum % 10000 == 0)
	{
		print "$infileLineNum\n";
	}

	chomp;

	my($form,$lemma,$reading,$cat) = split "	";

	my $symbCat = getSymbCat($cat);
	
	if ($symbCat eq "0")
	{
		print "ERREUR line $infileLineNum\n";
		exit(1);
	}

	print OUTPUT "$form	$lemma	$reading	$symbCat
";
}

close (DICO);
close (CODE);
close (OUTPUT);

####################################################################
sub getSymbCat
{
	my $cat = @_[0];
	my $symbCat;
	
	seek CODE,0,0;

	my $line;
	while (($line = <CODE>) !~ /$cat/) 
	{
		if (eof(CODE))
		{
			return "0";
		}
	 }

 	chomp $line;
	($cat,$symbCat) = split ";",$line;
	return $symbCat;
}
