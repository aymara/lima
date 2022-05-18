#!/usr/bin/perl -s

# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

###############################################
# Programme de vérification de la présence des
# déclencheurs d'expression idiomatiques dans le
# dictionnaire
###############################################


open(IDIOMS,"$ARGV[0]") || die("Impossible d'ouvrir le fichier $ARGV[0]\n");
open (DICOFUSION,"$ARGV[1]") || die ("Impossible d'ouvrir le fichier $ARGV[1]\n");

#chargement dans une table des catégories et de leurs correspondances
print ("INFO : Chargement des triggers\n");
$lineidiom = <IDIOMS>;
chop($lineidiom);
%triggers;
while ($lineidiom ne "") {
	@donneestags = split(/;/,$lineidiom);
        $trig=$donneestags[3];
        $trig=~ s/\[.\](.*)/\1/g;
        $trig=~ s/\&(.*)/\1/g;
        print "read trigger $trig in $donneestags[4]\n";
	$triggers{$trig} = $donneestags[4];

	$lineidiom = <IDIOMS>;
}

print ("INFO : Vérification des triggers\n");
#fin du chargement des catégories et de leurs correspondances

$motstraites = 0;

$ligne = <DICOFUSION>;
while ($ligne ne "") {

	$motstraites++;
    if (($motstraites % 10000)==0) {
	    print ("INFO : Lignes traitées : $motstraites\n");
    }

  @donnees = split(/\t/,$ligne);

  $entree = $donnees[0];
        if ($triggers{$entree} ne "") {
          #print ("found trigger $entree !\n");
          $triggers{$entree}="";
        }

  $ligne = <DICOFUSION>;
}

foreach my $key (keys %triggers) {
  if ($triggers{$key} ne "") {
    print "$key;$triggers{$key}\n";
  }
}

print STDERR ("INFO : vérification terminée\n");
