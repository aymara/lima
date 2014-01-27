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
# Categories conversion program :
# converts dicostd into dicojys (data multiplicative coding)
###############################################

print STDERR ("INFO : Start default categories conversion
");

open(SOURCE,"$ARGV[0]") || die("Cannot open $ARGV[0]\n");
open(CONVERT,"$ARGV[1]") || die ("Cannot open $ARGV[1]\n");
open(CIBLE,">$ARGV[2]") || die("Cannot open $ARGV[2]\n");
open(ERROR,">>error.txt") || die("Cannot open error.txt\n");


# Initialisation du fichier d'erreur
# Fin d'initialisation du fichier d'erreur

#chargement dans une table des categories et de leurs correspondances
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

	#Fin du codage des categories

	if ($tags{$info} ne "") {
		print CIBLE ("$type	$tags{$info}
");
	}
	else {
		print STDERR ("in file $ARGV[0] : Invalid properties $info\n");
		print ERROR ("in file $ARGV[0] : Invalid properties $info\n");
	}
}
