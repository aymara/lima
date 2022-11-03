#!/usr/bin/perl -s

# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

###############################################
# Programme de conversion de catï¿œories :
#convertit dicostd en dicojys (codage multiplicatif des donnï¿œs)
###############################################

print STDERR ("INFO : Debut du traitement conversion des categories par défaut\n");

open(SOURCE,"$ARGV[0]") || die("Impossible d'ouvrir le fichier $ARGV[0]\n");
open(CIBLE,">$ARGV[1]") || die("Impossible d'ouvrir le fichier $ARGV[1]\n");
open(CONVERT,"$ARGV[2]") || die ("Impossible d'ouvrir le fichier $ARGV[2]\n");
open(ERROR,">>error.txt") || die("Impossible d'ouvrir le fichier error.txt\n");


# Initialisation du fichier d'erreur
# Fin d'initialisation du fichier d'erreur

#chargement dans une table des catï¿œories et de leurs correspondances
%tags;
while (<CONVERT>) {
	@donneestags = split(/;/);
	$tags{$donneestags[0]} = $donneestags[1];
}

#fin du chargement des categories et de leurs correspondances

$motstraites = 0;
$motsnontraites = 0;

while (<SOURCE>) {
	chomp();
	@donnees = split(/	/);
	$type = $donnees[0];
	$info = $donnees[1];
	
# Codage des categories

	#Fin du codage des catï¿œories

	if ($tags{$info} ne "") {
		print CIBLE ("$type	$tags{$info}\n");
	}
	else {
		print STDERR ("in file $ARGV[0] : Invalid properties $info\n");
		print ERROR ("in file $ARGV[0] : Invalid properties $info\n");
	}
}
