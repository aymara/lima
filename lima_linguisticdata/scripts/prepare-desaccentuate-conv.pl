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
