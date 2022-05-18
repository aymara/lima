#!/usr/bin/perl

# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

use utf8;

#############
# Desaccentue les entrÃ©es du dictionnaire triÃ©# et sort une table cvs contenant les entrÃ©es dÃ©saccentuÃ©es suivies des entrÃ©es accentuÃ©es
#############

open (SOURCE,"<:utf8","$ARGV[0]") || die ("Impossible d'ouvrir $ARGV[0]");
open (CIBLE,">:utf8","$ARGV[1]") || die ("Impossible d'ouvrir $ARGV[1]");
open (CONVERT,"<:utf8","$ARGV[2]") || die ("Impossible d'ouvrir le fichier $ARGV[2] (convaccents.txt)");

#chargement des exclustion de dÃ©saccentuation si nÃ©cessaire.
my $exclude;
if (scalar(@ARGV)>2) {
  open (EXCLUDE,"<:utf8","$ARGV[2]") || die ("Impossible d'ouvrir $ARGV[2]");
  while (<EXCLUDE>) {
    chop();
    $exclude{$_}=1;
    print "ignore reaccentuation of $_\n";
  }
}


#chargement dans une table des caractÃšres accentuÃ©e et de leurs correspondants dÃ©saccentuÃ©e
$infoconvert = <CONVERT>;
$infoconvert=~ s/\s+$//;
while ($infoconvert ne "") {
    @donneesconvert = split(/	/,$infoconvert);
    chop($donneesconvert[1]);
    $conversion{$donneesconvert[0]} = $donneesconvert[1];
#     print "desaccentue >$donneesconvert[0]< vers >$donneesconvert[1]<\n";
    $infoconvert = <CONVERT>;
}
close (CONVERT);
#fin du chargement

############
# Programme de dÃ©saccentuation
############

$ligne = <SOURCE>;
$nbrelignes = 1;

while ($ligne ne "") {
	# print STDERR ("INFO : Nombre de lignes traitÃ©es : $nbrelignes\n");
    @data = split(/	/,$ligne);
    $entree = $data[0];
    @lettres = split(//,$entree);
    $lglettres = @lettres;

    $motdesaccentue = "";
    for ($i=0 ; $i<$lglettres ; $i++) {
	if (exists($conversion{$lettres[$i]})) {
	    $motdesaccentue.= $conversion{$lettres[$i]};
	}
	else {
	    $motdesaccentue.= $lettres[$i];
	}
    }
    if (($motdesaccentue ne $entree) && ($exclude{$motdesaccentue} != 1)) {
	print CIBLE ("$motdesaccentue	2		<DESACCENTUATION>		$entree\n");
    }
    $ligne = <SOURCE>;
    ++$nbrelignes;
}

print STDERR ("\nINFO : DÃ©saccentuation terminÃ©e\n");
