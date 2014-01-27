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

# usage example : ../../scripts/disamb_matrices_prior.pl corpus/corpus_fr_merge.txt matrices/priorUnigramMatrix-eng.dat DIVERS_*,NOM_PROPRE*,CONJ_*,NOMBRE*


open(SOURCE,"$ARGV[0]");
open(CIBLE,">$ARGV[1]");
$excludes = $ARGV[2];

###
# programme de comptage des lemmes et leurs catégories
###

print "Loading and sorting corpus prior data (excluding $excludes)\n";
if($excludes ne "") {
  $excludes =~ s/,/|/g;
  $excludes =~ s/\*/.*/g;
}
$ligne = <SOURCE>;
$lineNum = 1;
while ($ligne ne "") {
	chomp($ligne);
	@data = split(/	/,$ligne);
	$lemma = $data[0];
	$lemma=lc $lemma;
	if ($data[1] ne "" and $data[1] !~ m/$excludes/) {
		#print "Counting $lemma / $tableauconv{$data[1]\n";
		$tableauprior{$lemma}{$data[1]}++;
	}
	$ligne = <SOURCE>;
}
for $lemma (sort keys %tableauprior) {
    for $categ (sort keys %{$tableauprior{$lemma}}) {
		#print "$lemma / $categ : $tableauprior{$lemma}{$categ}\n";
		print CIBLE ("$lemma	$categ	$tableauprior{$lemma}{$categ}\n");
	}
}

print "Done\n";
