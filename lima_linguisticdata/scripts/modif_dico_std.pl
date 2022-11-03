#!/usr/bin/perl -s

# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

#################
# modif_dico_std
#################
# 30/06/2004
# Hubert Naets
#################
#
#Ce script permet d'ajouter ou de retirer des entrées dans un dictionnaire au format STD (standard).
#
# UTILISATION
# -----------
#
# ./modif_dico_std fichier_source fichier_cible table_de_modifications
#
# La table_de_modifications est composées d'une succession de lignes répondant à la syntaxe suivante:
# symbole_d'_ajout_ou_de_suppression forme lemme catégorie_std (normalisation)
# ["entre parenthèse" signifie que cette information est optionnelle;
#  les différents éléments sont séparés par des tabulations.]
#
# le symbole_d'_ajout_ou_de_suppression est "+" s'il s'agit d'ajouter une entrée ET "-" s'il s'agit de l'enlever.
#
# EXEMPLE DE TABLE_DE_MODIFICATIONS
# ---------------------------------
#
# + il il Ppspms
# - il il Ncgms
# + ha ha Ncums hectare
#
# ALGO
# ----
#
# 1/ on charge en mémoire les entrées à supprimer issues de la table_de_modifications.
# 2/ on vérifie si chaque ligne du dictionnaire source correspond à une entrée à supprimer
# 	2.1/ si c'est le cas, on ne copie pas la ligne dans le fichier cible et on recommence la boucle
# 	2.2/ sinon on copie la ligne dans le fichier cible
# 3/ On ajoute les entrées à insérer à la fin du dictionnaire cible.
#


# Programme

open (SOURCE,"$ARGV[0]") || die ("Impossible d ouvrir le fichier source $ARGV[0]");
open (CIBLE,">$ARGV[1]") || die ("Impossible d ouvrir le fichier cible $ARGV[1]");
open (TABLE,"$ARGV[2]")  || die ("Impossible d ouvrir la table de modifications $ARGV[2]");

# Chargement de la table de modifications

$ligne = <TABLE>;
while ($ligne ne "") {
	chomp($ligne);
	$ligne=~ s/\s*$//;
	if ($ligne=~ /^-/) {
		$ligne=~ s/^-\s*([^\s]+)/\1/;
		push(@tableSuppr,$ligne);
	}
	elsif ($ligne =~ /^+/) {
		$ligne =~ s/^\+	(.+)/\1/;
		push(@tableAjout,$ligne);
	}
	else {
		print ("Ligne non valide : $ligne\n");
	}
	$ligne = <TABLE>;
}
close(TABLE);

# Suppression d entrees

$ligne = <SOURCE>;

while ($ligne ne "") {
	chomp($ligne);
	$ligne=~ s/\s+$//;
	$copy = 1;
	for ($i=0 ; $i<@tableSuppr ; $i++) {
#		print ("$ligne\n");
#		print ("$tableSuppr[$i]\n");
		if ($ligne=~ /^$tableSuppr[$i]/) {
#			print ("Match : $ligne == $tableSuppr[$i]\n");
			$copy = 0;
			last;
		}
	}
	if (($copy == 1) && ($ligne ne "")) {
		print CIBLE ("$ligne\n");
	}
	$ligne = <SOURCE>;
}
close (SOURCE);


# Ajout d entrees

for ($i=0 ; $i <@tableAjout ; $i++) {
	print CIBLE ("$tableAjout[$i]\n");
}

# Fin du programme
close (CIBLE);
