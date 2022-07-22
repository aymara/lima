#!/usr/bin/perl -s

# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

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
