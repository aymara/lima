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
use utf8

open (SOURCE,"<:utf8","$ARGV[0]") || die ("Impossible d'ouvrir $ARGV[0]");
open (OUT,">>:utf8","$ARGV[1]") || die ("Impossible d'ouvrir $ARGV[1]");

if ($main::h || $main::help || scalar(@ARGV)!=2) {
  print "USAGE : xmlforms [OPTIONS] inputfile outputfile\n";
  print "where [OPTIONS] are :\n";
  print "  -h or -help : print usage\n";
  print "  -desacc=[yes|no] : specify desacc attribute for entries. default is none, that equals 'yes'\n"
  print "  -entryop=[add|replace|delete] : specify op attribute for entries. default is none, that equals 'add'\n";
  
}

my $concat = "";
my $form = "";
my $lemma = "";
my $norm = "";

my $count = 0;
my $icount = 0;

while (<SOURCE>)
{
  chop();
  s/&/&amp;/g;
  s/"/&quot;/g;
  s/</&lt;/g;
  s/>/&gt;/g;
  if ($_ eq "") { next;}
  @data=split(/	/);
  
  if ($data[0] ne $concat) {
    $concat=$data[0];
    if ($count > 0) {
      print OUT "</concat>\n";
    }
    print OUT "<concat>\n";
    $count++;
  }
  
  
  
}
if ($count > 0) {
  print OUT "  </i>\n";
  print OUT "</entry>\n";
}
