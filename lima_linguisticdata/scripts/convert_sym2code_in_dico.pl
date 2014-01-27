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

###############################################
# Programme de conversion de catégories :
#convertit dicostd en dicojys (codage multiplicatif des données)
###############################################

print STDERR ("INFO : Début du traitement convertjys\n");

open(SOURCE,"$ARGV[0]") || die("Impossible d'ouvrir le fichier $ARGV[0]\n");
print STDERR ("INFO : Fichier dicostd.txt ouvert\n");

open (CONVERT,"convjys.txt") || die ("Impossible d'ouvrir le fichier convjys.txt\n");
print STDERR ("INFO : Fichier ressources convjys.txt ouvert\n");

open(CIBLE,">$ARGV[1]") || die("Impossible d'ouvrir le fichier $ARGV[1]\n");
print STDERR ("INFO : Fichier dicojys.txt ouvert\n");

open(ERROR,">>error.txt") || die("Impossible d'ouvrir le fichier error.txt\n");
print STDERR ("INFO : Fichier error.txt ouvert\n");


# Initialisation du fichier d'erreur
print ERROR ("----------------------------------\n");
print ERROR ("Lignes non traitées par convertjys\n\n");
# Fin d'initialisation du fichier d'erreur

#chargement dans une table des catégories et de leurs correspondances
$infotags = <CONVERT>;
chop($infotags);
chomp($infotags);
%tags;
while ($infotags ne "") {
	@donneestags = split(/;/,$infotags);
	$tags{$donneestags[0]} = $donneestags[1];
	$infotags = <CONVERT>;	
}

while (($holder,$record) = each(%tags)) {
	print ("$holder;$record\n");
}

print STDERR ("INFO : Fin du chargement des catégories\n");
#fin du chargement des catégories et de leurs correspondances

$motstraites = 0;
$motsnontraites = 0;

$ligne = <SOURCE>;
while ($ligne ne "") {

	$motstraites++;

	print STDERR ("INFO : Lignes traitées : $motstraites\n");

	@donnees = split(/	/,$ligne);

	$entree = $donnees[0];
	$lemme = $donnees[1];
	$info = $donnees[2];
	chomp($information);
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
print STDERR ("INFO : Traitement convertjys terminé\n");
