#!/usr/bin/perl
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
