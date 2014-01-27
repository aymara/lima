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
# Programme de conversion de categories :
#convertit dicostd en dicojys (codage multiplicatif des donnees)
###############################################

print STDERR ("INFO : Debut du traitement convertjys\n");

open(SOURCE,"$ARGV[0]") || die("Impossible d'ouvrir le fichier $ARGV[0]\n");
print STDERR ("INFO : Fichier dicostd.txt ouvert\n");
$motsnontraites = 0;

open(CIBLE,">$ARGV[1]") || die("Impossible d'ouvrir le fichier $ARGV[1]\n");
print STDERR ("INFO : Fichier dicojys.txt ouvert");

open (CONVERT,"$ARGV[2]") || die ("Impossible d'ouvrir le fichier $ARGV[1]\n");
print STDERR ("INFO : Fichier ressources convjys.txt ouvert\n");

open(ERROR,">>error.txt") || die("Impossible d'ouvrir le fichier error.txt\n");
print STDERR ("INFO : Fichier error.txt ouvert\n");


# Initialisation du fichier d'erreur
print ERROR ("----------------------------------\n");
print ERROR ("Lignes non traitees par convertCodesComposesJys\n\n");
# Fin d'initialisation du fichier d'erreur

#chargement dans une table des categories et de leurs correspondances
$infotags = <CONVERT>;
chop($infotags);
%tags;
while ($infotags ne "") {
	@donneestags = split(/;/,$infotags);
	$tags{$donneestags[0]} = $donneestags[1];
	$infotags = <CONVERT>;
}

print STDERR ("INFO : Fin du chargement des categories\n");
#fin du chargement des categories et de leurs correspondances

$motstraites = 0;
$motsnontraites = 0;

$ligne = <SOURCE>;
while ($ligne ne "") {
	chomp($ligne);
	$motstraites++;

	print STDERR ("INFO : Lignes traitees : $motstraites\n");

	@donnees = split(/	/,$ligne);

	$type = $donnees[1];

	if ($type == 1) {

		$entree = $donnees[0];
		$lemme = $donnees[2];
		$info = $donnees[3];
		$normalisation = $donnees[4];
		chomp($normalisation);


		if ($tags{$info} ne "") {
				print CIBLE ("$entree	1	$lemme	$tags{$info}	$normalisation	
");
		}
		else {
			 print ERROR ("$entree	$lemme	$info	$normalisation	
");
			$motsnontraites++;
		}
	}
	elsif ($type == 4) {

		$entree = $donnees[0];
		$nbreComposants = $donnees[6];

		$sortie = "$entree	4					$nbreComposants	";
		$existInfo = 1;
		for ($i=0 ; $i<$nbreComposants ; $i++) {
			$entreeCompo = $donnees[7+($i*4)];
			$lemmeCompo = $donnees[8+($i*4)];
			$categCompo = $donnees[9+($i*4)];
			$normCompo = $donnees[10+($i*4)];
			if (($tags{$categCompo} ne "") && ($existInfo == 1)) {
				$sortie.= "$entreeCompo	$lemmeCompo	$tags{$categCompo}	$normCompo	";
			}
			else {
				$existInfo = 0;
				print "Je n ai pas trouve l info $categCompo\n";
			}
		}
		if ($existInfo == 1) {
			print CIBLE ("$sortie\n");
		}
		else {
			print ERROR ("$ligne\n");
			$motsnontraites++;
		}
	}
	$ligne = <SOURCE>;
}
print STDERR ("\nINFO : Entrees non prises en compte : $motsnontraites\n");
print STDERR ("INFO : Traitement convertjys termineen");
