#!/usr/bin/perl -w
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

binmode(STDOUT, ":utf8");

$#ARGV >= 1 or die "Not enough args\n";

my $tagDicFile = shift @ARGV;
open (TAGS, "<:encoding(UTF-8)", $tagDicFile) or die "Cannot open $tagDicFile\n";

my %dic;
while (<TAGS>)
{
  chomp;
  my ($key,$val) = split(";", $_);
  my @values = split(",", $val);
  $dic{$key} = \@values;
}
close TAGS;

open (FILE, "<:encoding(UTF-8)", "$ARGV[0]") or die "Cannot open $ARGV[0]\n";

while (<FILE>)
{
  chomp;
  my ($tok,$lem,$tag) = split("	", $_);
  if (defined $tag && defined $dic{$tag})
  {
    foreach my $val (@{$dic{$tag}})
    {
      print "$tok	$lem		$val\n";
    }
  }
}
