#!/usr/bin/perl -s

# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

open (ACC,"<:utf8","$ARGV[0]") || die ("Impossible d'ouvrir $ARGV[0]");
open (DESACC,"<:utf8","$ARGV[1]") || die ("Impossible d'ouvrir $ARGV[1]");
open (DICOACC,"<:utf8","$ARGV[2]") || die ("Impossible d'ouvrir $ARGV[2]");
open (DICODESACC,"<:utf8","$ARGV[3]") || die ("Impossible d'ouvrir $ARGV[3]");
open (OUTPUT,">:utf8","$ARGV[4]") || die ("Impossible d'ouvrir $ARGV[4]");

my $acc,$desacc,$dicoacc,$dicodesacc;
while (($acc = <ACC>) && ($desacc = <DESACC>) && ($dicoacc = <DICOACC>) && ($dicodesacc = <DICODESACC>))
{
  chomp($acc);
  chomp($desacc);
  if ($dicoacc =~ /empty/) {
    $dicoacc = 0;
  } elsif ($dicoacc =~ /\sno\s/) {
    $dicoacc = 0;
  } else {
    $dicoacc = 1;
  }
  if ($dicodesacc =~ /empty/) {
    $dicodesacc = 0;
  } elsif ($dicodesacc =~ /\sno\s/) {
    $dicodesacc = 0;
  } else {
    $dicodesacc = 1;
  }
  

  if (($acc ne $desacc) and $dicodesacc) {
    print OUTPUT "$acc	$desacc\n";
  }
}
