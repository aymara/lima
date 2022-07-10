#!/usr/bin/perl -s

# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

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
