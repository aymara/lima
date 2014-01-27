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
