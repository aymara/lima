#!/usr/bin/perl -s

# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

open (IN,"<:utf8","$ARGV[0]") || die ("Impossible d'ouvrir $ARGV[0]");
open (OUT,">:utf8","$ARGV[1]") || die ("Impossible d'ouvrir $ARGV[0]");

while (<IN>)
{
  if (/^set/ or /^#/ or /^\+/ or /^use\s/ or /^include\s/ ) {
    next;
  }
  @part=split(/:/);
  for (my $i=0;$i<3;$i++)
  {
    @words=split(/\s/,$part[$i]);
    foreach $w (@words) {
      $w =~ s/^\[//;
      $w =~ s/\]$//;
      if ($w =~ /[\@\$\*]/) {
        next;
      } elsif ($w =~ /([^\&\$\@\(\)0-9]+)/) {
        print OUT $1."\n";
      }
    }
  }
}
