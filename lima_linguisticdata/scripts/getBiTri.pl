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

###############################################
# extraire des trigrammes et des bigrammes a 
# partir du corpus annote sous format Benoit
###############################################

use strict;

my $input=$ARGV[0];
my $CatMatchList=$ARGV[1];
my $bi=$ARGV[2];
my $tri=$ARGV[3];

open INPUT , "$input" || die "Cannot open input file!\n";
open CATML , "$CatMatchList"|| die "cannot open catchLis file!\n";
open TRI , ">$tri" || die "Cannot open trigram file!\n";
open BI , ">$bi" || die "Cannot open bigram file!\n";

my %Category;

my $prePCat=0;
my $preCat=0;

while(<CATML>)
{
  chomp $_;
	#chop $_;
	my @element=split(/	/,$_);
	$Category{$element[1]}=$element[3];
	#$Category{$element[0]}=$element[1];
}

my $nbL=0;
while(<INPUT>)
{
	$nbL++;
	chomp $_;
	#chop $_;
  #my ($word, $cat)=(split / \| /,$_)[1,2];
	my ($word, $cat)=split(/	/,$_);
  #print "--[$word/$cat]--\n";
	if (!exists($Category{$cat}))
	{
		print "In line ".$nbL." incorrect category name : ".$cat."\n";
	}
    if (($prePCat ne 0)&&($preCat ne 0)){    
      print TRI $Category{$prePCat}."	".$Category{$preCat}."	".$Category{$cat}."\n";
    }
    if($preCat ne 0){
      print BI $Category{$preCat}."	".$Category{$cat}."\n";
    }

	$prePCat=$preCat;
	$preCat=$cat;
}


close INPUT;
close TRI;
close BI;
close CATML;
