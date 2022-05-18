#!/usr/bin/perl -s

# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

# usage example : ../../scripts/disamb_matrices_prior.pl corpus/corpus_fr_merge.txt matrices/priorUnigramMatrix-fre.dat code_symbolic2lic2m.txt DIVERS_*,NP*,PONCTU_*,CONJ*,NC_MESURE,NC_NUM_CARD

print "@ARGV\n";

open(SOURCE,"$ARGV[0]") || die "Cannot open $ARGV[0]\n";
open(CIBLE,">$ARGV[1]") || die "Cannot create $ARGV[1]\n";
open(TABLECONVERT,"$ARGV[2]") || die "Cannot open $ARGV[2]\n";
$excludes = $ARGV[3];

####
# Chargement de la table de conversion
####

print "Loading conversion table $ARGV[2]\n";
$info = <TABLECONVERT>;
while ($info ne "") {
  chomp($info);
  @data = split(/	/,$info);
  $data[0]=~ s/\s//g;
  $tableauconv{$data[1]} = $data[1];
#   print "Loading '$data[1]' / '$tableauconv{$data[1]}'\n";
  $info = <TABLECONVERT>
}
close(TABLECONVERT);
###
# programme de comptage des lemmes et leurs catÃ©gories
###

print "Loading and sorting corpus prior data (excluding $excludes)from $ARGV[0]\n";
if($excludes ne "") {
  $excludes =~ s/,/|/g;
  $excludes =~ s/\*/.*/g;
}
$lineNum = 1;

while (<SOURCE>) {
  $ligne = $_;
  while (chomp($ligne)>0) {}
  chop $ligne;
  @data = split(/	/,$ligne);
  $lemma = $data[0];
  $lemma=lc $lemma;
#   print "Handling $lemma / '$data[1]' / $tableauconv{$data[1]}\n";
  if ($tableauconv{$data[1]} ne "" and $tableauconv{$data[1]} !~ m/$excludes/) {
#     print "Counting $lemma / $tableauconv{$data[1]}\n";
    $tableauprior{$lemma}{$tableauconv{$data[1]}}++;
  }
  $ligne = <SOURCE>;
}
close (SOURCE);

for $lemma (sort keys %tableauprior) {
    for $categ (sort keys %{$tableauprior{$lemma}}) {
#     print "$lemma / $categ : $tableauprior{$lemma}{$categ}\n";
    print CIBLE ("$lemma	$categ	$tableauprior{$lemma}{$categ}\n");
  }
}
close (CIBLE);

print "Done\n";
