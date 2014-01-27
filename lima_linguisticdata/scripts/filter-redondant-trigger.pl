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
use utf8;
###############################################
# Programme de vérification de la présence des
# déclencheurs d'expression idiomatiques dans le
# dictionnaire
###############################################


open(TRIGGERS,"<:utf8","$ARGV[0]") || die("Impossible d'ouvrir le fichier $ARGV[0]\n");
open(OUTPUT,">:utf8",$ARGV[1]) || die("Impossible d'ouvrir le fichier $ARGV[1]\n");


#chargement dans une table des catégories et de leurs correspondances
print ("INFO : Filtrage des triggers\n");
%written;
while (<TRIGGERS>) {
  if (m%^(<k>[^<]*</k>)<o>[0-9]*</o>%) {
    if (defined($written{$1})) {
      print "WARN : already existing triggers : $1 ignore it\n";
    } else {
      print OUTPUT $_;
      $written{$1}=1;
    }
  } elsif (m%^(<k>[^<]*</k><cat>[^<]*</cat>)<o>[0-9]*</o>%) {
    if (defined($written{$1})) {
      print "WARN : already existing triggers : $1 ignore it\n";
    } else {
      print OUTPUT $_;
      $written{$1}=1;
    }
  } else
  {
    print "invalid line : $_";
    print OUTPUT $_;
  }

}
close(TRIGGERS);
close(OUTPUT);
