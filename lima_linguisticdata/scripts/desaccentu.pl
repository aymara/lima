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
use utf8;

#############
# Desaccentue les entrées du dictionnaire trié# et sort une table cvs contenant les entrées désaccentuées suivies des entrées accentuées
#############

open (SOURCE,"<:utf8","$ARGV[0]") || die ("Impossible d'ouvrir $ARGV[0]");
open (CIBLE,">:utf8","$ARGV[1]") || die ("Impossible d'ouvrir $ARGV[1]");
open (CONVERT,"<:utf8","$ARGV[2]") || die ("Impossible d'ouvrir le fichier $ARGV[2] (convaccents.txt)");

#chargement des exclustion de désaccentuation si nécessaire.
my $exclude;
if (scalar(@ARGV)>2) {
  open (EXCLUDE,"<:utf8","$ARGV[2]") || die ("Impossible d'ouvrir $ARGV[2]");
  while (<EXCLUDE>) {
    chop();
    $exclude{$_}=1;
    print "ignore reaccentuation of $_\n";
  }
}


#chargement dans une table des caractères accentuée et de leurs correspondants désaccentuée
$infoconvert = <CONVERT>;
$infoconvert=~ s/\s+$//;
while ($infoconvert ne "") {
    @donneesconvert = split(/	/,$infoconvert);
    chop($donneesconvert[1]);
    $conversion{$donneesconvert[0]} = $donneesconvert[1];
#     print "desaccentue >$donneesconvert[0]< vers >$donneesconvert[1]<\n";
    $infoconvert = <CONVERT>;
}
close (CONVERT);
#fin du chargement

############
# Programme de désaccentuation
############

$ligne = <SOURCE>;
$nbrelignes = 1;

while ($ligne ne "") {
	# print STDERR ("INFO : Nombre de lignes traitées : $nbrelignes\n");
    @data = split(/	/,$ligne);
    $entree = $data[0];
    @lettres = split(//,$entree);
    $lglettres = @lettres;

    $motdesaccentue = "";
    for ($i=0 ; $i<$lglettres ; $i++) {
	if (exists($conversion{$lettres[$i]})) {
	    $motdesaccentue.= $conversion{$lettres[$i]};
	}
	else {
	    $motdesaccentue.= $lettres[$i];
	}
    }
    if (($motdesaccentue ne $entree) && ($exclude{$motdesaccentue} != 1)) {
	print CIBLE ("$motdesaccentue	2		<DESACCENTUATION>		$entree\n");
    }
    $ligne = <SOURCE>;
    ++$nbrelignes;
}

print STDERR ("\nINFO : Désaccentuation terminée\n");
