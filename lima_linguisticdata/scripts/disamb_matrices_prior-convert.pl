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
# programme de comptage des lemmes et leurs catégories
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
