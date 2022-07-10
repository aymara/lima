#!/usr/bin/perl -s

# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

###############################################
# Programme de conversion de catégories :
#convertit dicostd en dicojys (codage multiplicatif des données)
###############################################

print STDERR ("INFO : Déut du traitement convertjys\n");

open(SOURCE,"$ARGV[0]") || die("Impossible d'ouvrir le fichier $ARGV[0]\n");
print STDERR ("INFO : Fichier $ARGV[0] ouvert\n");

open(CIBLE,">$ARGV[1]") || die("Impossible d'ouvrir le fichier $ARGV[1]\n");
print STDERR ("INFO : Fichier $ARGV[1] ouvert\n");

# convjys.txt
open (CONVERT,"$ARGV[2]") || die ("Impossible d'ouvrir la table de conversion $ARGV[2]\n");
print STDERR ("INFO : Fichier ressources $ARGV[2] ouvert\n");

open(ERROR,">>error.txt") || die("Impossible d'ouvrir le fichier error.txt\n");
print STDERR ("INFO : Fichier error.txt ouvert\n");

# Initialisation du fichier d'erreur
print ERROR ("----------------------------------\n");
print ERROR ("Lignes non traitées par convertjys\n\n");
# Fin d'initialisation du fichier d'erreur

#chargement dans une table des catégories et de leurs correspondances
$infotags = <CONVERT>;
chop($infotags);
%tags;
while ($infotags ne "") {
	@donneestags = split(/;/,$infotags);
	$tags{$donneestags[0]} = $donneestags[1];
	$infotags = <CONVERT>;
}

print STDERR ("INFO : Fin du chargement des catégories\n");
#fin du chargement des catégories et de leurs correspondances

$motstraites = 0;
$motsnontraites = 0;

$ligne = <SOURCE>;
while ($ligne ne "") {

	$motstraites++;
    if (($motstraites % 1000)==0) {
	    print STDERR ("INFO : Lignes traitées : $motstraites\n");
    }

	@donnees = split(/	/,$ligne);

	$entree = $donnees[0];
	$lemme = $donnees[1];
	$info = $donnees[2];
	$normalisation = $donnees[3];
	chomp($normalisation);
	
	# Codage des catégories

	#Fin du codage des catégories

	if ($tags{$info} ne "") {
		#$codestags = $tags{$info};
		#@codetag = split(/\+/,$codestags);
		#$tailleCodetag = @codetag;
		#for ($k=0 ; $k<$tailleCodetag ; $k++) {
			print CIBLE ("$entree	1	$lemme	$tags{$info}	$normalisation	\n");
		#}
	}
	else {
		 print ERROR ("$entree	$lemme	$info	$normalisation	\n");
		$motsnontraites++;
	}

	$ligne = <SOURCE>;
}
print STDERR ("\nINFO : Entrées non prises en compte : $motsnontraites\n");
print STDERR ("INFO : Traitement convertjys terminén");
