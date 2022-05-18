#!/usr/bin/perl -s -W

# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

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
