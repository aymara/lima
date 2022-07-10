#!/usr/bin/perl -s

# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

open (SOURCE,"$ARGV[0]");
open (CIBLE,">$ARGV[1]");
open (TABLECONVERT,"$ARGV[2]");

####
# Chargement de la table de conversion
####

$info = <TABLECONVERT>;

while ($info ne "") {
	
	chomp($info);
	@data = split(/	/,$info);
	$data[0]=~ s/\s//g;
	$tableau{$data[0]} = $data[1];
	$info = <TABLECONVERT>
}

###
# programme en lui-mï¿œe
###

$ligne = <SOURCE>;
$lineNum = 1;
while ($ligne ne "") {
	chomp($ligne);
	if ($tableau{$ligne} ne "") {
		print CIBLE ("$tableau{$ligne}\n");
	}
	else {
		print ("ERREUR de disamb_matrices_retag : $ligne (ligne $lineNum)\n");
	}
	$ligne = <SOURCE>;
	$lineNum++;
}
