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

open (IN1,"<:utf8","$ARGV[0]") || die ("Impossible d'ouvrir $ARGV[0]");
open (IN2,"<:utf8","$ARGV[1]") || die ("Impossible d'ouvrir $ARGV[1]");
open (OUT1,">:utf8","$ARGV[2]") || die ("Impossible d'ouvrir $ARGV[2]");
open (OUT2,">:utf8","$ARGV[3]") || die ("Impossible d'ouvrir $ARGV[3]");

my $l1,$l2;
while (($l1=<IN1>) && ($l2=<IN2>)) {
  chop($l1);
  chop($l2);
  if (($l1 ne "") and ($l2 ne "")) {
    print OUT1 $l1."\n";
    print OUT2 $l2."\n";
  }
}
