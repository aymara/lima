#!/usr/bin/perl -s

# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

open (SOURCE,"$ARGV[0]") || die ("Impossible d ouvrir le fichier source\n");
open (CIBLE,">$ARGV[1]") || die ("Impossible d ouvrir le fichier cible\n");

# lecture de la premiere ligne pour voir le nombre de colonnes
# et savoir si on travaille avec des trigrammes ou des bigrammes

$ligne = <SOURCE>;
@premiereLigne = split(/	/,$ligne);
$longueurLigne = @premiereLigne;


#while ($ligne ne "") {
#	$somme+= $premiereLigne[$longueurLigne-1];
#	$ligne = <SOURCE>;
#	@premiereLigne = split(/	/,$ligne);
#}
#close(SOURCE);

# On normalise les frequences en divisant chaque frequence par la somme

open (SOURCE,"$ARGV[0]") || die ("Impossible d ouvrir le fichier source\n");

$ligne = <SOURCE>;

while ($ligne ne "") {

	@infos = split(/	/,$ligne);
	$freq = $infos[$longueurLigne-1];
	#$newfreq = $freq / $somme;

	if ($longueurLigne == 4) {
		#printf CIBLE ("$infos[0]	$infos[1]	$infos[2]	%11.10f\n",$newfreq);
		printf CIBLE ("$infos[0]	$infos[1]	$infos[2]	%d\n",$freq);
	}
	elsif ($longueurLigne == 3) {
	    #printf CIBLE ("$infos[0]	$infos[1]	%11.10f\n",$newfreq);
		printf CIBLE ("$infos[0]	$infos[1]	%d\n",$freq);
	}
	else {
		print "Erreur --- nombre de lignes non valable : $longueurLigne";
	}
	$ligne = <SOURCE>;
}
