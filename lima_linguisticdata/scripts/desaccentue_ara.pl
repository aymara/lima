#!/usr/bin/perl
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
#############
# Desaccentue les entr�s du dictionnaire tri�
# et sort une table cvs contenant les entr�s d�accentu�s suivies des entr�s accentu�s
#############

open (SOURCE,"$ARGV[0]") || die ("Impossible d ouvrir $ARGV[0]");
open (CIBLE,">$ARGV[1]") || die ("Impossible d ouvrir $ARGV[1]");
open (CONVERT,"convaccents.txt") || die ("Impossible d ouvrir le fichier convaccents.txt");

#chargement dans une table des caracteres accentues et de leurs correspondants desaccentues
print ("Chargement de la table de desaccentuation - devoyellisation");
$infoconvert = <CONVERT>;
while ($infoconvert ne "") {
    chomp($infoconvert);
    @donneesconvert = split(/	/,$infoconvert);
    $conversion{$donneesconvert[0]} = $donneesconvert[1];
    $infoconvert = <CONVERT>;
}
close (CONVERT);
#fin du chargement

############
# Programme de desaccentuation
############

$ligne=<SOURCE>;

while ($ligne ne "") {

	chomp($ligne);
	@data = split(/	/,$ligne);
	$entree = $data[0];
	$entreeAConserver = $entree;
	$imprimerLaSortie = 0;
	while (($holder, $record) = each(%conversion)) {

		if ($entree=~ s/$holder/$record/g) {
			$imprimerLaSortie = 1;
		}
	}
	while (($holder, $record) = each(%conversion)) {
		$entree=~ s/$holder/$record/g;
	}
	if ($imprimerLaSortie == 1) {
		print CIBLE ("$entree	2				$entreeAConserver\n");
	}
	$ligne = <SOURCE>;
}
