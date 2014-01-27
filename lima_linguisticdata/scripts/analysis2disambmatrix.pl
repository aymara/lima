#!/usr/bin/perl -s -W

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
binmode STDOUT,":utf8";

my ($lang, $textFile) = @ARGV;

open(DICO, "<:utf8", "$ENV{LINGUISTIC_DATA_ROOT}/disambiguisationMatrices/$lang/code_symbolic2lic2m.txt") || die ("Impossible d'ouvrir le fichier de correspondance entre codes LIC2M et codes Grace");

my %dico;

while (<DICO>)
{
  chomp;
  my ($grace,$lic) = split /	/;
  $dico{$lic} = $grace;
}
close DICO;

system "analyzeText -l $lang -d textDumper $textFile";
open(IN, "<:utf8", "$textFile.out");
while (<IN>)
{
#   chomp;
  if (/\d+ \| (.*) \| .*#(.*)/)
  {
    print "$1	$dico{$2}	$2\n";
  }
}
close IN;
