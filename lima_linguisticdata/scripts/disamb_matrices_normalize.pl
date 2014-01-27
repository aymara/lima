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
#

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
