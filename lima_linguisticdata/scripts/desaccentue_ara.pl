#!/usr/bin/perl

# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

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
