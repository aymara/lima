#!/usr/bin/perl

# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

use strict;
use utf8;
binmode STDOUT,":utf8";

# Recuperation des arguments
my($inFile,$codeFile,$outFile) = @ARGV;

if ($#ARGV != 2) 
{ 
	print "usage : transcode.pl inFile codeFile outFile\n";
	exit(1);
}

# Ouverture des fichiers
open (INFILE, "<:utf8", $inFile) || die "Cannot open $inFile\n";
open (CODEFILE, "<:utf8", $codeFile) || die "Cannot open $codeFile\n";
open (OUTFILE, "+>:utf8", $outFile) || die "Cannot create $outFile\n";



while (<INFILE>)
{
	chomp;
	my ($lemma,$reading,$code) = split "	";
	if ($code >= 3000)
	{
		$code -= 3000;
		$code = "00".$code;
	}
	elsif ($code >= 2000)
	{
		$code -= 2000;
		$code = "00".$code;
	}
	elsif ($code >= 1000)
	{
		$code -= 1000;
		$code = "00".$code;
	}

	$code = substr $code, 1, length($code);
	print OUTFILE "$lemma	";
	print OUTFILE "$lemma	";

	# reading is considered as the normalized form
	print OUTFILE "$reading	";
	print OUTFILE getConjugationForm($code), "
";
}

close(INFILE);
close(CODEFILE);
close(OUTFILE);


#######################################################
#######################################################
sub getConjugationForm
{
	my($motif) = @_[0];

	seek CODEFILE,0,0;	

	while (<CODEFILE>)
	{
		chomp;
		
		if (m/$motif/)
		{
			my($code,$conjugationForm) = split ";";
			
			return $conjugationForm;
		}
	}
}
