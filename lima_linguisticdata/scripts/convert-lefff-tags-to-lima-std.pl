#!/usr/bin/perl -w

# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

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
