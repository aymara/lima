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

# Recuperation des arguments
# my($inFile,$terminationFile,$outFile) = @ARGV;
# 
# if ($#ARGV != 2)
# {
# 	print "usage : jpnflex.pl inFile terminationFile outFile\n";
# 	exit(1);
# }
my $rootDir = $ENV{"LINGUISTIC_DATA_ROOT"};
my $inFile =
$rootDir."/analysisDictionary/jpn/convert/dictdata/flexionable.dic";
my $terminationFile = $rootDir."/analysisDictionary/jpn/flex/flex-table.txt";
#my $outFile = $rootDir."analysisDictionary/jpn/flex/flexionable.flex";
my $outFile = $ARGV[0];

# Ouverture des fichiers
open (INFILE, "<:utf8", $inFile) || die "Cannot open $inFile\n";
open (TERMINATIONFILE, "<:utf8", $terminationFile) || die "Cannot open\n$terminationFile
";
open (OUTFILE, "+>:utf8", $outFile) || die "Cannot create $outFile\n";

my $debug = 1;
my $infileLineNum = 0;
while (<INFILE>)
{
	$infileLineNum++;
	if ($debug && $infileLineNum % 1000 == 0)
	{
		print "$infileLineNum
";
	}

	chomp;
 
 	my($stem,$lemma,$reading,$conjugationType) = split "	";
	my @t_termination = get_termination($conjugationType);

	my($element,$lemmaTermination,$lemmaTerminationK);
	foreach $element(@t_termination) 
	{
		if ($element =~ /	終止/ && $` ne "だ")
		{
			$lemmaTermination = $`;
			$lemmaTerminationK =
				hiraganaToKatakana("$lemmaTermination");
		}
	}
	
	if ($conjugationType =~ /五段|上一段|下一段|名サ変/) 
	{
		$conjugationType = $&;
		
	}
	print OUTFILE "$stem	",
				 "$lemma$lemmaTermination	",
				 "$reading$lemmaTerminationK	",
				 "$conjugationType",
				 "語幹\n" ;
	
	my $entry;
	foreach $entry(@t_termination)
	{
		my($termination,$conjugationForm) = split("	",$entry);
		
		print OUTFILE "$stem$termination	",
					 "$lemma$lemmaTermination	",
					 "$reading$lemmaTerminationK	",
					"$conjugationType$conjugationForm\n";
	}
}

close (INFILE);
close (TERMINATIONFILE);
close (OUTFILE);

####################################################################
sub get_termination
{
	my($conjugationType) = @_[0];
	
	seek TERMINATIONFILE,0,0;

	if ($conjugationType =~ /上一段|下一段/)
	{
		$conjugationType = $&;
	}

	while (<TERMINATIONFILE>)
	{
		chomp;
		my $l_termination = "";

		if (/$conjugationType/)
		{
			while ((my $line = <TERMINATIONFILE>) ne "\n")
			{
				chomp $line;
				my($termination,$conjugationForm) = split(";",
				$line);
				$l_termination =
			      $l_termination."$termination	$conjugationForm,";
			}

			chop $l_termination;
			return split ",",$l_termination;
		}
	}
}

sub hiraganaToKatakana
{
	my $string = $_[0];

	$string =~ tr/ぁ-ゖゝ-ゟ/ァ-ヶヽ-ヿ/;
	return $string;
}

sub katakanaToHiragana
{
	my $string = $_[0];

	$string =~ tr/ァ-ヶヽ-ヿ/ぁ-ゖゝ-ゟ/;
	return $string;
}
