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
# programme en lui-m�e
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
